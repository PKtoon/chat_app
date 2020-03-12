#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <deque>

#include <boost/asio.hpp>

#include "user.h"

using  boost::asio::ip::tcp;

class Server
{
    boost::asio::io_context io;
    tcp::endpoint endpoint;
    tcp::acceptor acceptor;
     
    std::vector<std::unique_ptr<User>> userList;
    std::deque<Stream> deliveryQueue;
    bool isDelivering{false};
    
    void accept();
    void deliverMessages();

public:
    Server(unsigned short port): endpoint{tcp::v6(),port}, acceptor{io,endpoint} 
    {
        accept();
        io.run();
    }
  
    User* getUser(std::string);
    void removeMe(User*);
    void queueDelivery(Stream);
};

#endif // SERVER_H
