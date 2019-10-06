#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <vector>

#include "user.h"

using  boost::asio::ip::tcp;

class Server
{
    boost::asio::io_context io;
    tcp::acceptor acceptor;
    int timeout = 5;
    boost::asio::steady_timer t{io,boost::asio::chrono::seconds(0)};
    std::vector<std::unique_ptr<User>> userList;
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
};

#endif // SERVER_H
