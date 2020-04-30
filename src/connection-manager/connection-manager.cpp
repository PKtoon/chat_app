#include "connection-manager.h"

void ConnectionManager::newConnection(asio::io_context &io)
{
    disconnect();
    socket = new asio::ip::tcp::socket{io};
}

void ConnectionManager::newConnection(asio::ip::tcp::socket sock)
{
    disconnect();
    socket = new asio::ip::tcp::socket{std::move(sock)};
}

void ConnectionManager::disconnect()
{
    if(socket)
    {
        if(socket->is_open())
        {
            asio::error_code error;
            socket->shutdown(asio::ip::tcp::socket::shutdown_both, error);
            socket->close();
        }
        delete socket;
        socket = nullptr;
    }
}

void ConnectionManager::connector(std::function<void(asio::error_code, asio::ip::tcp::endpoint)> callBack)
{
    asio::async_connect(*socket,endpoints,[this,callBack](const asio::error_code& error, const asio::ip::tcp::endpoint& endpoint)
        {
            if(error)
            {
                if(error != asio::error::operation_aborted)
                {
                    if(endpoints == endpoints.end())
                    {
                        callBack(error,endpoint);
                    }
                    else
                    {
                        endpoints++;
                        connector(callBack);
                    }
                }
            }
            else
                callBack(error,endpoint);
        });
}

void ConnectionManager::writer(const std::vector<char> outData, std::function<void (asio::error_code, std::size_t)> callBack)
{   
    asio::async_write(*socket,asio::buffer(outData),[callBack](asio::error_code error, std::size_t sent)
    {
        if(error != asio::error::operation_aborted)
            callBack(error,sent);
    });
}

void ConnectionManager::writer(const std::vector<asio::const_buffer> buffer, std::function<void (asio::error_code, std::size_t)> callBack)
{
    asio::async_write(*socket,buffer,[callBack](asio::error_code error, std::size_t sent)
    {
        if(error != asio::error::operation_aborted)
        callBack(error,sent);
    });
}

void ConnectionManager::reader(int length, std::function<void (std::vector<char> , asio::error_code, std::size_t)> callBack)
{
    inData.clear();
    inData.resize(length);
    asio::async_read(*socket,asio::buffer(inData),[this,callBack](const asio::error_code& error, std::size_t read)
    {
        if(error != asio::error::operation_aborted)
        callBack(inData,error,read);
    });
}
