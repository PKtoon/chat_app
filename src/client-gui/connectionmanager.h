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
    
    void setEndpoints(std::string hostname, std::string port)
    {
        endpoints = resolver.resolve(hostname,port);
    }
    
    void connector(std::function<void(boost::system::error_code)> callBack);
    void writer(const std::string outData
    , std::function<void (boost::system::error_code,std::size_t)> callBack);
    void reader(int length, std::function<void(std::vector<char>&,boost::system::error_code,std::size_t)> callBack);
};



// template<class T> void ConnectionManager::connector(T* obj,void (*callBack)(T* ,const boost::system::error_code))
// {
//     boost::asio::async_connect(socket,endpoints,[obj,callBack](const boost::system::error_code& error, const tcp::endpoint&)
//          {
//              callBack(obj,error);
//          });
// }
// 
// template<class T> void ConnectionManager::writer(T* obj, const std::string outData, void (&callBack)(T*,boost::system::error_code,std::size_t))
// {
//     unsigned long length = outData.size();
//     std::ostringstream header;
//     header<<std::setw(headerLength)<<std::hex<<length;
//     
//     std::vector<boost::asio::const_buffer> buffers;
//     buffers.push_back(boost::asio::buffer(header.str()));
//     buffers.push_back(boost::asio::buffer(outData));
//     
//     boost::asio::async_write(socket,boost::asio::buffer(outData),[obj,&callBack](boost::system::error_code error, std::size_t sent)
//     {
//         callBack(obj,error,sent);
//     });
// }
// 
// template<class T> void ConnectionManager::reader(T* obj, int length,void (&callBack)(T*, std::vector<char>&,boost::system::error_code,std::size_t))
// {
//     inData.clear();
//     inData.resize(length);
//     boost::asio::async_read(socket,boost::asio::buffer(inData),[this,obj,&callBack](const boost::system::error_code& error, std::size_t read)
//     {
//         callBack(obj,inData,error,read);
//     });
// }


#endif // CONNECTIONMANAGER_H
