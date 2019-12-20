#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <vector>
#include <deque>

#include "user.h"

using  boost::asio::ip::tcp;

class Server
{
    boost::asio::io_context io;
    tcp::acceptor acceptor;
    int timeout = 5;
    boost::asio::steady_timer t{io,boost::asio::chrono::seconds(0)};
    std::vector<std::unique_ptr<User>> userList;
    std::deque<Stream> deliveryQueue;
    bool isDelivering{false};
    void accept();
    void removeUser();
    void isAlive();

public:
    Server(tcp::endpoint& endpoint):acceptor(io,endpoint)
    {
        accept();
        isAlive();
        io.run();
    }
    User* getUser(std::string);
    void addMe(User*);
    void queueDelivery(Stream);
    void deliverMessages();
};

#endif // SERVER_H
