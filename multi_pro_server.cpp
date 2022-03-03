#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 8532

int main() {
    sockaddr_in serv_addr, client_addr;
    socklen_t clien;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    int serv_ip;
    //inet_pton(AF_INET, "192.168.199.128", (void*)serv_ip);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(listenfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);

    int connfd;

    char buf[1024];
    int ret = 0;
    while(1) {
        connfd = accept(listenfd, (sockaddr*)&client_addr, &clien);
        if(fork() == 0) {
            close(listenfd); 
            while(1) {
                ret = read(connfd, buf, sizeof(buf)); //return the actual bytes read from fd
                write(STDOUT_FILENO, buf, ret);
                write(connfd, buf, ret);
            }
        } 
        close(connfd);
    }

    close(connfd);
    close(listenfd);

    return 0;
}