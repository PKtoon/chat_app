#ifndef NETFACE_H
#define NETFACE_H

#include "../connection-manager/connection-manager.h"
#include "../stream/stream.h"

class NetFace
{
    int headerLength = 4;
    ConnectionManager connMan;
    boost::asio::io_context io;
    
public:
    NetFace(boost::asio::io_context& io);
    NetFace(tcp::socket sock);
    
    tcp::socket& getSocket() { return connMan.getSocket(); }
    void connect(std::string hostname, std::string port, std::function<void(boost::system::error_code)> callBack);
    void disconnect();
    void send(Stream data, std::function<void(boost::system::error_code,std::size_t)> callBack);
    void receive(std::function<void(Stream,boost::system::error_code,std::size_t)> callBack);
    void receiveBody(unsigned int length,std::function<void(Stream,boost::system::error_code,std::size_t)> callBack);
};

#endif // NETFACE_H
