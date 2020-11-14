#include "connection-manager.hpp"

void ConnectionManager::disconnect()
{
    if(socket.is_open())
    {
        asio::error_code error;
        socket.shutdown(asio::ip::tcp::socket::shutdown_both, error);
        socket.close();
    }
}

void ConnectionManager::connector(std::function<void(asio::error_code, asio::ip::tcp::endpoint)> callBack)
{
    asio::async_connect(socket,endpoints,[this,callBack](const asio::error_code& error, const asio::ip::tcp::endpoint& endpoint)
        {
            if(error != asio::error::operation_aborted)
            {
                callBack(error,endpoint);
            }
        });
}

void ConnectionManager::writer(const std::vector<char> outData, std::function<void (asio::error_code, std::size_t)> callBack)
{   
    asio::async_write(socket,asio::buffer(outData),[callBack](asio::error_code error, std::size_t sent)
    {
        if(error != asio::error::operation_aborted)
            callBack(error,sent);
    });
}

void ConnectionManager::writer(const std::vector<asio::const_buffer> buffer, std::function<void (asio::error_code, std::size_t)> callBack)
{
    asio::async_write(socket,buffer,[callBack](asio::error_code error, std::size_t sent)
    {
        if(error != asio::error::operation_aborted)
        callBack(error,sent);
    });
}

void ConnectionManager::reader(int length, std::function<void (std::vector<char> , asio::error_code, std::size_t)> callBack)
{
    inData.clear();
    inData.resize(length);
    asio::async_read(socket,asio::buffer(inData),[this,callBack](const asio::error_code& error, std::size_t read)
    {
        if(error != asio::error::operation_aborted)
        callBack(inData,error,read);
    });
}
