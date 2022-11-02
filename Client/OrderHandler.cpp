#include<iostream>
#include<string.h>
#include<regex>
#include "OrderHandler.h"

using namespace std;

OrderHandler::OrderHandler() {}

int OrderHandler::getOrder(char buf[]) {
    while(true) {
        string order;
        cin.clear();
        while(order.back() != ';') {
            string tmp;
            getline(cin, tmp);
            order += (" " + tmp);
        }
        if(order == " quit;") {
            return 0;
        } else {
            return this->orderParse(order, buf);
        }
    }
}

int OrderHandler::orderParse(string& order, char buf[]) {
    string pattern("(\\s)*\\b(((((zset(\\s)+[0-9a-zA-Z]+)|(set|lpush))(\\s)+[0-9a-zA-Z]+))|get)(\\s)+[0-9a-zA-Z]+(\\s)*;");
    regex r(pattern);

    if(!regex_match(order, r)) {
        return -1;
    }
    smatch result;
    regex each("[0-9a-zA-Z]+");

    vector<string> keywords;

    if(regex_search(order, result, each)) {
        for(sregex_iterator it(order.begin(),order.end(),each), end_it; it != end_it; it++) {
            keywords.push_back(it->str());
        }
    }
    Msg msg;
    strcpy(msg.operation, keywords[0].c_str());
    if(keywords.size() == 2) {
        strcpy(msg.key, keywords[1].c_str());
    } else if(keywords.size() == 4) {
        strcpy(msg.key, keywords[1].c_str());
        strcpy(msg.subkey, keywords[2].c_str());
        strcpy(msg.value, keywords[3].c_str());
    } else {
        if(keywords[0] == "set") {
            strcpy(msg.key, keywords[1].c_str());
            strcpy(msg.value, keywords[2].c_str());
        } else {
            strcpy(msg.key, keywords[1].c_str());
            strcpy(msg.value, keywords[2].c_str());
        }
    }
    memcpy(buf, &msg, sizeof(Msg));
    // cout << msg.operation << " " << msg.container << " " << msg.key << " " << msg.value;
    return 1;
}