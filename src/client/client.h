#ifndef CLIENT_H
#define CLIENT_H

#include <deque>

#include <asio.hpp>

#include "../stream/stream.h"
#include "../network-interface/netface.h"

class Client
{
    std::string name;
    NetFace net;
    std::deque<Stream> writeQueue;
    bool isWriting{false};
    
public:
    Client(std::string clientName, std::string hostname, std::string port, asio::io_context& io);
    
    void connect(std::string hostname,std::string port);
    void initialize();
    void reader();
    void writer();
    void processData(Stream);
    void queueMessage(Stream);
    void ping();
};

#endif
