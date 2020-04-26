#ifndef USER_H
#define USER_H

#include <deque>

#include <asio.hpp>

#include "../network-interface/netface.h"
#include "../logger.hpp"

class Server;

class User
{
    std::string name;
    bool isAlive{false};
    bool isWriting{false};
    unsigned short count = 0;
    std::deque<Stream> writeQueue;
    NetFace net;
    Server& server;
    asio::steady_timer timer {net.getSocket().get_executor(),asio::chrono::seconds(20)};
public:
    User(asio::ip::tcp::socket socket, Server& serv);
    ~User() { log(name+" is destroyed");}
    
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
