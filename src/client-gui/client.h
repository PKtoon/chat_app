#ifndef CLIENT_H
#define CLIENT_H

#include <deque>

#include <.hpp>

#include "src/network-interface/netface.h"
/**
 * @todo write docs
 */
class Client
{
    NetFace net;
    bool isWriting{true};
    std::deque<Stream> writeQueue;
public:
    Client(QListWidgetItem::io_context& io): net{io} {}
    
    void reader();
    void writer();
    void processData(Stream);
    void queueMessage(Stream);
    void ping();
};

#endif // CLIENT_H
