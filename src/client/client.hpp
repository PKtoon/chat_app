#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <deque>

#include "sqlite3_wrap.h"
#include "network-interface/netface.hpp"
/**
 * @todo write docs
 */

class Client
{
    std::string name_;
    asio::io_context io_;
    NetFace net;
    bool isWriting{false};
    std::deque<Stream> writeQueue;
    SQLite3DB db{"storage"};
    bool isConnected {false};
    
public:
     enum ContactType {
        individual,
        group 
    };
    
    Client(std::string name = "") : net{io_} {}

    Client(asio::io_context& io,std::string name = ""): net{io} {}
    
    enum class Error{
        ok, db_error
    };
    //client
    void userAuthInit(std::string name, std::string password, Header head);
    std::string name() { return name_; }
    void reader(std::function<void (Stream data,asio::error_code,std::size_t)> callBack);
    void writer();
    void processData(Stream data);
    void processMessage(Stream data);
    Client::Error queueMessage(Stream data);
    void ping();

    //networking
    void connect(std::string host, std::string port, std::function<void(asio::error_code)> callBack);
    void disconnect();
    void runIOContext();
    asio::ip::tcp::socket* getSocket();
    void newSocket();

    //db
    std::string getDBError();
    void initDB();
    bool getContact(std::string name, bool& result);
    bool getContactList(std::vector<std::pair<std::string, int> > &list);
    bool getMessages(std::string subject, std::vector<std::pair<std::string,std::string>>& msg);
    bool insertContact(std::string name, ContactType type);
    bool insertMessage(std::string subject, std::string sender, std::string msg);
};

#endif // CLIENT_HPP
