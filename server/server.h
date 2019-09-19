#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <deque>
#include <list>

#include "user.h"

class User;

using  boost::asio::ip::tcp;

class Server
{
    boost::asio::io_context io;
    tcp::acceptor acceptor;
    int timeout = 5;
    boost::asio::steady_timer t{io,boost::asio::chrono::seconds(0)};
    std::deque<User*> userList;
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

    bool searchUser(std::string);
    User* getUser(std::string);
};

#endif // SERVER_H
