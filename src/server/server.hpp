#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include "user.hpp"
#include "psql-wrap.hpp"

class Server
{
    asio::io_context io;
    asio::ip::tcp::endpoint endpoint;
    asio::ip::tcp::acceptor acceptor;
     
    std::vector<std::unique_ptr<User>> userList;
    std::mutex userListMutex;
    std::list<Stream> deliveryList;
    std::mutex deliveryListMutex;
    asio::steady_timer deliveryTimer {io,asio::chrono::seconds(1)};

    pk::PSQLdb db;

    void accept();
    void deliverMessages();
    void deliveryScheduler();

public:
    Server(unsigned short port, std::string dbUri) : endpoint{asio::ip::tcp::v6(),port}, acceptor{io,endpoint}, db{dbUri}
    {
        accept();
        deliveryScheduler();
        io.run();
    }
  
    User* getActiveUser(std::string);
    void removeMe(User*);
    void queueDelivery(Stream);

    void addUser(std::string name, std::string passwd);
    pqxx::result getUser(std::string name);
    bool authUser(std::string name, std::string passwd);
    std::list<Stream> getPendingMessages(std::string name);
    void storePendingMessage(Stream data);
    void sendUserMessage(Stream data);
    void sendGroupMessage(Stream data);
    std::list<std::string> getGroupMembers(std::string groupName);
    void createGroup(Stream data);
};

#endif // SERVER_H
