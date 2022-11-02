#ifndef InetAddress_hpp
#define InetAddress_hpp

#include <arpa/inet.h>

class InetAddress {
private:
    int serv_ip;
    
public:
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    InetAddress(const char*, uint16_t);
    InetAddress(uint16_t);
    ~InetAddress();
};

#endif /* InetAddress_hpp */