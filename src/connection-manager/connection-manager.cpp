#include "connection-manager.h"
#include <iomanip>
#include <iostream>

void ConnectionManager::connector(std::function<void(boost::system::error_code)> callBack)
{
    boost::asio::async_connect(socket,endpoints,[this,callBack](const boost::system::error_code& error, const tcp::endpoint&)
        {
            callBack(error);
        });
}

void ConnectionManager::writer(const std::string outData, std::function<void (boost::system::error_code, std::size_t)> callBack)
{   
    boost::asio::async_write(socket,boost::asio::buffer(outData),[callBack](boost::system::error_code error, std::size_t sent)
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
