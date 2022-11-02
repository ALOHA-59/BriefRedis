#include "Socket.h"
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <iostream>

Socket::Socket() {
    this->fd = ::socket(AF_INET, SOCK_STREAM, 0);
    // int flag = fcntl(fd, F_GETFL, 0);
    // flag |= O_NONBLOCK;
    // fcntl(fd, F_SETFL, flag);
    assert(this->fd != -1);
}

Socket::Socket(int fd) {
    assert(fd != -1);
    this->fd =fd;
}

int Socket::bind(InetAddress* inetAddress) {
    return ::bind(AF_INET, (sockaddr*)&inetAddress->addr, inetAddress->addr_len);
}

void Socket::listen(int maxListenConn) {
    ::listen(this->fd, maxListenConn);
}

int Socket::accept(InetAddress* client) {
    return ::accept(this->fd, (sockaddr*)&client->addr, &client->addr_len);
}

int Socket::connect(InetAddress* server) {
    return ::connect(this->fd, (sockaddr*)&server->addr, server->addr_len);
}

void Socket::send(char msg[], int msgLength) {
    int res = ::write(this->fd, msg, msgLength);
    if(res == 0) {
        std::cout << "faild" << std::endl;
    }
}

void Socket::receive(char recv[], int recvLength) {
    ::read(this->fd, recv, recvLength);
}
Socket::~Socket() {
    if(this->fd != -1) {
        ::close(this->fd);
    }
}