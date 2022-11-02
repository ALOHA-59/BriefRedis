#include <iostream>
#include <fstream>
#include <thread>
// #include "SkipList.h"
#include <unistd.h>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "redisDb.hpp"
#include "utils.hpp"

#define FILE_PATH    "./dumpFile"
#define PORT         8532

using namespace std;

struct Todo {
    int fd;
    function<bool()> insert;
    function<void()> del;
    function<char*()> fetch;
};

struct Msg
{
    char operation[10];
    char key[20];
    char sub_key[20];
    char value[20];
};

const int msgLength = sizeof(Msg);

mutex todo_mtx;
condition_variable todo_cv;
condition_variable cv;

/*两个队列*/
queue<function<void()>> task_queue;
queue<Todo> todo_queue;

dictType* type = (dictType*)malloc(sizeof(dictType));
redisDb*db = redisDbCreate(type, 16);

// SkipList<int, std::string> skipList(6);
queue<char*> bak_queue;

void AOF() {
    ofstream ofs(FILE_PATH, ios::binary | ios::app);
    while(!bak_queue.empty()) {
        ofs.write(bak_queue.front(), msgLength);
        bak_queue.pop();
    }
}

void doBackup(int signo) {
    thread druation(AOF);
    druation.join();
}

void deal(int fd, int flg, string res) {
    if(flg == 0) {
        char buf[msgLength];
    
        int ret = readn(fd, buf, msgLength);
        cout << "thread id: " << this_thread::get_id() << endl;
        if(ret == -1) return;
        else if(ret == 0) {
            cout << "client require to quit" << endl;
            // write(fd, "close", 6);
            close(fd);
            return;
        }

        Msg msg;
        memcpy(&msg, buf, msgLength);

        Todo todo;
        todo.fd = fd;
        if(!strcmp(msg.operation, "set")) {
            auto f = std::bind(redisDbInsert, db, msg.key, new value(msg.value), 1);
            todo.insert = f;
            bak_queue.push(buf);
        } else if(!strcmp(msg.operation, "lset")) {
            auto f = std::bind(redisDbInsert, db, msg.key, new value(msg.value), 2);
            todo.insert = f;
            bak_queue.push(buf);
        } else if(!strcmp(msg.operation, "hset")) {
            auto f = std::bind(redisDbInsert, db, msg.key, new value(msg.sub_key, msg.value), 3);
            todo.insert = f;
            bak_queue.push(buf);
        } else if(!strcmp(msg.operation, "get")) {
            auto f = std::bind(redisDbFetchValue, db, msg.key);
            todo.fetch = f;
        }
        todo_queue.push(todo);
        todo_cv.notify_one();
    } else {
        char buf[10];
        cout << "send" << endl;
        strcpy(buf, res.c_str());
        write(fd, buf, sizeof(buf));
    }
    
}

void redisDo() {
    unique_lock<mutex> todo_lock(todo_mtx);
    while(true) {
        while(todo_queue.empty()) {
            todo_cv.wait(todo_lock); // 无条件等待
        }
        auto curr = todo_queue.front();
        todo_queue.pop();
        char buf[10];
        if(curr.insert != nullptr) {
            if(curr.insert()) {
                strcpy(buf, "ok");
            }
        } else if(curr.del != nullptr) {
                curr.del();
                strcpy(buf, "ok");
        } else if(curr.fetch != nullptr) {
            char * val = curr.fetch();
            strcpy(buf, val);
        }
        auto t = std::bind(deal, curr.fd, 1, buf);
        task_queue.push(t);
        cv.notify_one();
        // write(curr.fd, buf, sizeof(buf));
    }
}

/*
void dump(int signo) {
    if(!skipList.isupdate) {
        return;
    }
    skipList.isupdate = false;
    backup = true;
    if(fork() == 0) {
        skipList.dump_file();
        backup = false;
        exit(-1);
    } else {
        //skipList.isupdate = false;
        return;
    }
}*/

void catch_child(int signo) {
    while(wait(NULL) != -1) {
        cout << "killed" << endl;
    }
}


int main() {

    type->hashFunction=myHashFunction;
    type->keyDup=myKeyDup;
    type->valDup=myValDup;
    type->keyCompare=myKeyCompare;
    type->keyDestructor=myKeyDestructor;
    type->valDestructor=myValDestructor;


    // skipList.load_file();
    // 键值中的key用int型，如果用其他类型，需要自定义比较函数
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    
        itimerval backup;
        backup.it_value.tv_sec = 5;
        backup.it_value.tv_usec = 0;
        backup.it_interval.tv_sec = 5;
        backup.it_interval.tv_usec = 0;
    
        setitimer(ITIMER_REAL, &backup, NULL);
        signal(SIGALRM, doBackup);
        // signal(SIGCHLD, catch_child);
    
    thread redisRun(redisDo);
    thread duration(AOF);

    mutex task_mtx;
    // condition_variable cv;
    /* basic steps, set ip : port */
    sockaddr_in serv_addr, clien_addr;
    socklen_t clien_len = sizeof(sockaddr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(lfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(lfd, 3); 

    char clien_ip[20];

    /* epoll */
    int epfd = epoll_create(1024);
    struct epoll_event events[20], ev; //
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev); // add listen event to R-B treee

    /*thread pool*/
    vector<thread> threadpool;
    for(int i = 0; i < thread::hardware_concurrency(); i++) {
        threadpool.emplace_back(thread([&]() {
            function<void()> task;
            while(true) {
                {
                    unique_lock<mutex> lock(task_mtx);
                    while(task_queue.empty()) {
                        cv.wait(lock);
                    }
                    task = task_queue.front();
                    task_queue.pop();
                }
                task();
            }
        }));
    } 

    //threadpool.clear();
   
    while(1) {
        int nfds = epoll_wait(epfd, events, 20, -1);  // minotor the R-B tree
        for(int i = 0; i < nfds; i++) {
            if(events[i].data.fd == lfd) {  // new client
                auto new_conn_fd = accept(lfd, (sockaddr*)&clien_addr, &clien_len);
                //threadpool.emplace_back(thread(deal,fd));   // new thread, deal new client
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = new_conn_fd;
                fcntl(new_conn_fd, F_SETFL, fcntl(new_conn_fd, F_GETFL, 0) | O_NONBLOCK);
                int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, new_conn_fd, &ev);
                inet_ntop(AF_INET, &clien_addr.sin_addr.s_addr, clien_ip, 20);
                if(ret == 0) {
                    cout << "new client: " << clien_ip << " " << ntohs(clien_addr.sin_port) << endl;
                }
            } else if (events[i].events & EPOLLIN) { // client message
                // handle events[i].events
                cout << "client msg: ";
                int fd = events[i].data.fd;
                auto t = std::bind(deal, fd, 0, "");
                task_queue.push(t);
                cv.notify_one();
            }
        }
    }
    cv.notify_all();
    // for(auto & t : threadpool) {
    //     if(t.joinable()) {
    //         t.join();
    //     }
    // }
    close(lfd);
    //auto_save.join();
    return 0;
}

