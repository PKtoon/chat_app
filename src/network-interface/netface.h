#ifndef NETFACE_H
#define NETFACE_H

#include "src/connection-manager/connection-manager.h"
#include <src/stream/stream.h>

class NetFace
{
    std::string hostname;
    std::string port;
    int headerLength = 4;
//     std::vector<char> inData;
    ConnectionManager connMan;
    boost::asio::io_context io;
    std::thread ioThread;
    
public:
    NetFace(boost::asio::io_context& io);
    NetFace(boost::asio::io_context& io, std::string hostname, std::string port);
    NetFace(tcp::socket sock);
    
    void connect(std::string hostname, std::string port, std::function<void(boost::system::error_code)> callBack);
    void connect(std::function<void(boost::system::error_code)> callBack);
    void disconnect();
    void send(Stream data, std::function<void(boost::system::error_code,std::size_t)> callBack);
    void receive(std::function<void(Stream,boost::system::error_code,std::size_t)> callBack);
    void receiveBody(unsigned int length,std::function<void(Stream,boost::system::error_code,std::size_t)> callBack);
    void runIOThread();
    void stopIOThread();
};

#endif // NETFACE_H