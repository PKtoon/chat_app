#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <deque>

#include "external/SQLite/sqlite3_wrap.h"
#include "src/network-interface/netface.h"
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
    Client(std::string name = "") : net{io_}
    {
        if(!name.empty())
            init(name);
    }
    Client(asio::io_context& io,std::string name = ""): net{io}
    {
        if(!name.empty())
            init(name);
    }
    
    //client
    void start(std::string password,std::function<void (asio::error_code, Stream data)> callBack);
    std::string name() { return name_; }
    void reader(std::function<void (Stream data,asio::error_code,std::size_t)> callBack);
    void writer();
    void processData(Stream data);
    void processMessage(Stream data);
    void queueMessage(Stream data);
    void ping();
    void init(std::string name);

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
    bool getContactList(std::vector<std::string>& list);
    bool getMessages(std::string subject, std::vector<std::pair<std::string,std::string>>& msg);
    bool insertContact(std::string name);
    bool insertMessage(std::string subject, std::string sender, std::string msg);
};

#endif // CLIENT_HPP
