#ifndef NETFACE_H
#define NETFACE_H

#include "../connection-manager/connection-manager.h"
#include "../stream/stream.h"

#include <deque>

class NetFace
{
    int headerLength = 4;
    ConnectionManager* connMan = nullptr;
    asio::steady_timer timer;
    std::deque<std::unique_ptr<ConnectionManager>> socketQueue;
    bool isTimerRunning {false};
public:
    NetFace(asio::io_context& io);
    NetFace(asio::ip::tcp::socket sock);
    ~NetFace()
    {
        if(connMan)
            delete connMan;
    }
    
    asio::ip::tcp::socket* getSocket();
    ConnectionManager* getConnMan() { return connMan; }
    void connect(std::string hostname, std::string port, std::function<void(asio::error_code)> callBack);
    void disconnect();
    void removeConnMan();
    void newConnection(asio::io_context& io);
    void newConnection(asio::ip::tcp::socket sock);
    void send(Stream data, std::function<void(asio::error_code,std::size_t)> callBack);
    void receive(std::function<void(Stream,asio::error_code,std::size_t)> callBack);
};

#endif // NETFACE_H
