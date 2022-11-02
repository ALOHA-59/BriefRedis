#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include "InetAddress.h"

class Socket {
public:
    int fd;
    Socket();
    Socket(int);
    ~Socket();

    int bind(InetAddress*);
    void listen(int);
    int accept(InetAddress*);
    int connect(InetAddress*);
    void send(char[], int);
    void receive(char [], int);
};


#endif