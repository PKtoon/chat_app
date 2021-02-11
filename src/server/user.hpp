#ifndef USER_H
#define USER_H

#ifndef NDEBUG
#include <iostream>
#endif

#include <list>

#include "network-interface/netface.hpp"

class Server;

class User
{
    std::string name,name2;
    bool isAlive{false};
    unsigned short count = 0;
    std::list<Stream> writeQueue;
    std::mutex writeQueueMutex;
    unsigned int currentQueueIndex = 0;
    NetFace net;
    Server& server;
    asio::steady_timer pulseTimer {net.getSocket()->get_executor(),asio::chrono::seconds(300)};
    asio::steady_timer writeTimer {net.getSocket()->get_executor()};
public:
    User(asio::ip::tcp::socket socket, Server& serv);
#ifndef NDEBUG
    ~User()
    {
        std::clog<<name2+" is destroyed"<<std::endl;
    }
#endif
    const std::string& getName() const { return name; }
    void queueMessage(Stream);
private:
    void authHandler(Stream data);
    void reader();
    void writer();
    void writeScheduler();
    void processData(Stream);
    void pingMe();
    void checkPulse();
    void findContact(Stream data);
    void checkPendingMessages();
};

#endif
