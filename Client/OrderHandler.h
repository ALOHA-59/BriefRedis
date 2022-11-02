#ifndef ORDERHANDLER_H
#define ORDERHANDLER_H
#include <string>

struct Msg
{
    char operation[10];
    char key[20];
    char subkey[20];
    char value[20];
};

class OrderHandler {
public:
    OrderHandler();
    int getOrder(char []);

private:
    int orderParse(std::string&, char []);
};

#endif