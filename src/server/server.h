#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <deque>

#include <asio.hpp>

#include "user.h"

class Server
{
    asio::io_context io;
    asio::ip::tcp::endpoint endpoint;
    asio::ip::tcp::acceptor acceptor;
     
    std::vector<std::unique_ptr<User>> userList;
    std::deque<Stream> deliveryQueue;
    bool isDelivering{false};
    
    void accept();
    void deliverMessages();

public:
    Server(unsigned short port): endpoint{asio::ip::tcp::v6(),port}, acceptor{io,endpoint} 
    {
        accept();
        io.run();
    }
  
    User* getUser(std::string);
    void removeMe(User*);
    void queueDelivery(Stream);
};

#endif // SERVER_H
