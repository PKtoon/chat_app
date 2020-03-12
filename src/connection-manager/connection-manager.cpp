#include "connection-manager.h"

void ConnectionManager::connector(std::function<void(boost::system::error_code, tcp::endpoint)> callBack)
{
    boost::asio::async_connect(socket,endpoints,[callBack](const boost::system::error_code& error, const tcp::endpoint& endpoint)
        {
            callBack(error,endpoint);
        });
}

void ConnectionManager::writer(const std::vector<char> outData, std::function<void (boost::system::error_code, std::size_t)> callBack)
{   
    boost::asio::async_write(socket,boost::asio::buffer(outData),[callBack](boost::system::error_code error, std::size_t sent)
    {
        callBack(error,sent);
    });
}

void ConnectionManager::writer(const std::vector<boost::asio::const_buffer> buffer, std::function<void (boost::system::error_code, std::size_t)> callBack)
{
    boost::asio::async_write(socket,buffer,[callBack](boost::system::error_code error, std::size_t sent)
    {
        callBack(error,sent);
    });
}

void ConnectionManager::reader(int length, std::function<void (std::vector<char> , boost::system::error_code, std::size_t)> callBack)
{
    inData.clear();
    inData.resize(length);
    boost::asio::async_read(socket,boost::asio::buffer(inData),[this,callBack](const boost::system::error_code& error, std::size_t read)
    {
        callBack(inData,error,read);
    });
}
