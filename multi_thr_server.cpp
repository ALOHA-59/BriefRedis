#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <ctype.h>
#include <vector>
#include <iostream>

#define PORT 8532

using namespace std;

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
    sockaddr_in serv_addr, clien_addr;
    socklen_t clien_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(lfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(lfd, 3);
    char buf[1024];
    vector<thread> thr_vec;
    while(1) {
        int cfd = accept(lfd, (sockaddr*)&clien_addr, &clien_len);
        if(cfd > 0) {
            thr_vec.push_back(thread(deal, cfd));
        }
    }
    for(auto & t : thr_vec) {
        if(t.joinable()) {
            t.join();
        }
    }
    close(lfd);
    return 0;
}