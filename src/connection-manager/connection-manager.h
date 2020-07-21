#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <functional>

#include "asio.hpp"

class ConnectionManager
{
    asio::ip::tcp::socket socket;
    asio::ip::tcp::resolver resolver;
    asio::ip::tcp::resolver::results_type endpoints;

    std::vector<char> inData;
    unsigned int inDataSize=0;

public:
    ConnectionManager(asio::io_context& io):socket{io}, resolver{io}{}
    ConnectionManager(asio::ip::tcp::socket sock) : socket{std::move(sock)}, resolver{socket.get_executor()} {}
    ~ConnectionManager(){ disconnect(); }
    asio::ip::tcp::socket& getSocket() { return socket; }
    void setEndpoints(std::string hostname, std::string port)
    {
        endpoints = resolver.resolve(asio::ip::tcp::v6(),hostname,port);
    }
    
    void disconnect();
    void connector(std::function<void(asio::error_code, asio::ip::tcp::endpoint)> callBack);
    void writer(const std::vector<char> outData, std::function<void (asio::error_code,std::size_t)> callBack);
    void writer(const std::vector<asio::const_buffer> buffer, std::function<void (asio::error_code,std::size_t)> callBack);
    void reader(int length, std::function<void(std::vector<char>,asio::error_code,std::size_t)> callBack);
};

#endif // CONNECTIONMANAGER_H
