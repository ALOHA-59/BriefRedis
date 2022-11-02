#include "Socket.h"
#include "InetAddress.h"
#include <unistd.h>
#include "string.h"
#include "OrderHandler.h"
#include <iostream>

#define MSG_LENGTH sizeof(Msg)

int main() {
    char buf[MSG_LENGTH];
    char recvbuf[100];
    Socket client;
    InetAddress server("127.0.0.1", 8532);
    if(client.connect(&server) == -1) {
        std::cout << "connecting error" << std::endl;
        return 0;
    }
    // client.connect(&server);
    
    OrderHandler orderhandler;
    
    while(1) {
        std::cout << "redis:8532>";
        int ret = orderhandler.getOrder(buf);
        if(ret == 0) {
            std::cout << "Bye!" << std::endl;
            return 0;
        } else if(ret == -1) {
            std::cout << "Invalid order" << std::endl;
        } else {
            client.send(buf, MSG_LENGTH);
            memset(recvbuf, 0, sizeof(recvbuf));
            client.receive(recvbuf, sizeof(recvbuf));
            std::cout << recvbuf << std::endl;
        }
    }
    return 0;
}