#ifndef NETFACE_H
#define NETFACE_H

#include "../connection-manager/connection-manager.h"
#include "../stream/stream.h"

class NetFace
{
    int headerLength = 4;
    ConnectionManager* connMan;
    asio::io_context io;
    
public:
    NetFace(asio::io_context& io);
    NetFace(asio::ip::tcp::socket sock);
    ~NetFace()
    {
        if(connMan)
            delete connMan;
    }
    
    asio::ip::tcp::socket& getSocket() { return connMan->getSocket(); }
    void connect(std::string hostname, std::string port, std::function<void(asio::error_code)> callBack);
    void disconnect();
    ConnectionManager* getConnection() { return connMan;}
    void newConnection(asio::io_context& io);
    void newConnection(asio::ip::tcp::socket sock);
    void send(Stream data, std::function<void(asio::error_code,std::size_t)> callBack);
    void receive(std::function<void(Stream,asio::error_code,std::size_t)> callBack);
    void receiveBody(unsigned int length,std::function<void(Stream,asio::error_code,std::size_t)> callBack);
};

#endif // NETFACE_H
