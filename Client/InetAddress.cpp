#include "InetAddress.h"

InetAddress::InetAddress(const char* serv_ip, uint16_t port) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, serv_ip, &addr.sin_addr.s_addr);
}

InetAddress::InetAddress(uint16_t port) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

InetAddress::~InetAddress() {
    
}