#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <boost/asio.hpp>
#include <functional>

using boost::asio::ip::tcp;

class ConnectionManager
{
    tcp::socket socket;
    tcp::resolver resolver;
    tcp::resolver::results_type endpoints;

    std::vector<char> inData;
    unsigned int inDataSize=0;

public:
    ConnectionManager(boost::asio::io_context& io):socket{io}, resolver{io}{}
    ConnectionManager(tcp::socket sock) : socket{std::move(sock)}, resolver{socket.get_executor()} {}
    
    tcp::socket& getSocket() { return socket; }
    void setEndpoints(std::string hostname, std::string port)
    {
        endpoints = resolver.resolve(hostname,port);
    }
    
    void connector(std::function<void(boost::system::error_code)> callBack);
    void writer(const std::vector<char> outData
    , std::function<void (boost::system::error_code,std::size_t)> callBack);
    void writer(const std::vector<boost::asio::const_buffer> buffer, std::function<void (boost::system::error_code,std::size_t)> callBack);
    void reader(int length, std::function<void(std::vector<char>,boost::system::error_code,std::size_t)> callBack);
};

#endif // CONNECTIONMANAGER_H
