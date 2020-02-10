#ifndef USER_H
#define USER_H

#include <deque>

#include <boost/asio.hpp>

// #include "../stream/stream.h"
#include "../network-interface/netface.h"

class Server;

class User
{
    std::string name{"uninitialized"};
    bool isAlive{true};
    bool isWriting{false};
    std::deque<Stream> writeQueue;
    NetFace net;
    Server *server;
public:
    User(boost::asio::ip::tcp::socket socket, Server* serv);
    
    const std::string& getName() const { return name; }
    bool getStatus() const { return isAlive; }
    
    void initialize();
    void writer();
    void reader();
    void queueMessage(Stream);
    void processData(Stream);
    void pingMe();
};

#endif
