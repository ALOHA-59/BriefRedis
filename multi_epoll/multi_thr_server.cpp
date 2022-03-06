#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <ctype.h>
#include <vector>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

#define PORT 8532

using namespace std;

queue<function<void()>> task_queue;
mutex mtx;
condition_variable cv;

void deal(int fd) {
    char buf[100];
    while(1) {
        int ret = read(fd, buf, sizeof(buf));
        if(ret == 0) break;
        write(STDOUT_FILENO, buf, ret);
        for(auto & c : buf) {
            c = toupper(c);
        }
        write(fd, buf, ret);
    }
    close(fd);
}

int main() {
    /* basic steps, set ip : port */
    sockaddr_in serv_addr, clien_addr;
    socklen_t clien_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(lfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(lfd, 3);

    char buf[1024];
    vector<thread> thr_vec; // thread pool

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
            {
                unique_lock<mutex> lock(mtx);
                while(task_queue.empty()) {
                    cv.wait(lock);
                }
                task = task_queue.front();
                task_queue.pop();
            }
            task();
        }));
    }

    while(1) {
        int nfds = epoll_wait(epfd, events, 20, -1);  // minotor the R-B tree
        for(int i = 0; i < nfds; i++) {
            if(events[i].data.fd == lfd) {  // new client
                auto new_conn_fd = accept(lfd, (sockaddr*)&clien_addr, &clien_len);
                // threadpool.emplace_back(thread(deal,fd));   // new thread, deal new client
                ev.events = EPOLLIN | EPOLLET; 
                ev.data.fd = new_conn_fd;
                fcntl(new_conn_fd, F_SETFL, fcntl(new_conn_fd, F_GETFL, 0) | O_NONBLOCK);
                int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, new_conn_fd, &ev);
            } else if (events[i].events & EPOLLIN) { // client message
                // handle events[i].events
                cout << "client msg :" << endl;
                int fd = events[i].data.fd;
                auto t = std::bind(deal, fd);
                task_queue.push(t);
                cv.notify_one();
            }
        }
    }

    // while(1) {
    //     int cfd = accept(lfd, (sockaddr*)&clien_addr, &clien_len);
    //     if(cfd > 0) {
    //         thr_vec.push_back(thread(deal, cfd));
    //     }
    // }
    for(auto & t : thr_vec) {
        if(t.joinable()) {
            t.join();
        }
    }
    close(lfd);
    return 0;
}