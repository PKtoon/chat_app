#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <deque>

#include "user.hpp"
#include "psql-wrap.hpp"

class Server
{
    asio::io_context io;
    asio::ip::tcp::endpoint endpoint;
    asio::ip::tcp::acceptor acceptor;
     
    std::vector<std::unique_ptr<User>> userList;
    std::deque<Stream> deliveryQueue;
    bool isDelivering{false};
    pk::PSQLdb db;

    void accept();
    void deliverMessages();

public:
    Server(unsigned short port, std::string dbUri) : endpoint{asio::ip::tcp::v6(),port}, acceptor{io,endpoint}, db{dbUri}
    {
        accept();
        io.run();
    }
  
    User* getActiveUser(std::string);
    void removeMe(User*);
    void queueDelivery(Stream);

    void addUser(std::string name, std::string passwd);
    pqxx::result getUser(std::string name);
    bool authUser(std::string name, std::string passwd);
};

#endif // SERVER_H
