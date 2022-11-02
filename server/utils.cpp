#include "utils.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int readn(int fd, void *buf, int count) {
    int ret;
    int nleft = count;
    int nread;
    char *bufp = (char *) buf;
    while(nleft > 0) {
        if((nread = ::read(fd, bufp, nleft)) <0) {
                return -1;
        } else if(nread == 0) { /* 对等方关闭 */
            return count - nleft;
        } 
        bufp += nread;
        nleft -= nread;
    }
    return count;	
}