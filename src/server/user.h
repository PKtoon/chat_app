#ifndef USER_H
#define USER_H

#include <iostream>
#include <deque>

#include "../network-interface/netface.h"

class Server;

class User
{
    std::string name,name2;
    bool isAlive{false};
    bool isWriting{false};
    unsigned short count = 0;
    std::deque<Stream> writeQueue;
    NetFace net;
    Server& server;
    asio::steady_timer timer {net.getSocket()->get_executor(),asio::chrono::seconds(20)};
public:
    User(asio::ip::tcp::socket socket, Server& serv);
    ~User() { std::clog<<name2+" is destroyed"<<std::endl;}
    
    const std::string& getName() const { return name; }
    
    void initialize();
    void writer();
    void reader();
    void queueMessage(Stream);
    void processData(Stream);
    void pingMe();
    void checkPulse();
};

#endif
