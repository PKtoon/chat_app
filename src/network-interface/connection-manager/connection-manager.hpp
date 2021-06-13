#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

/**************************************************************************************
 * Connection Manager wraps ASIO funtions for convenience
 *
 * Author: Pratik Khanke
 *
 * Tasks of Connection Manager:
 *      Provide convenient wrapper for sending and receiving data
 *      Handles socket
 *      Handles connecting to server
 *
 **************************************************************************************
 */


#ifdef __WIN32__

#include <winsock2.h>

#endif

#include <functional>

#include <asio.hpp>

class ConnectionManager
{
    asio::ip::tcp::socket socket;
    asio::ip::tcp::resolver resolver;
    asio::ip::tcp::resolver::results_type endpoints;

    std::vector<char> inData;           //Input buffer
    unsigned int inDataSize=0;          //Input size

public:
    ConnectionManager(asio::io_context& io):socket{io}, resolver{io}{}
    ConnectionManager(asio::ip::tcp::socket sock) : socket{std::move(sock)}, resolver{socket.get_executor()} {}
    ~ConnectionManager(){ disconnect(); }

    asio::ip::tcp::socket& getSocket() { return socket; }           //Returns reference to socket
    void setEndpoints(std::string hostname, std::string port)       //Sets endpoint to connect
    {
        endpoints = resolver.resolve(/*asio::ip::tcp::v4(),*/hostname,port);
    }
    
    asio::error_code disconnect();                                                                                                  //Disconnects
    void connector(std::function<void(asio::error_code, asio::ip::tcp::endpoint)> callBack);                                        //Connects to endpoint
    void writer(const std::vector<char> outData, std::function<void (asio::error_code,std::size_t)> callBack);                      //Sends data (takes vector of char)
    void writer(const std::vector<asio::const_buffer> buffer, std::function<void (asio::error_code,std::size_t)> callBack);         //Sends data (takes vector of asio::const_buffer)
    void reader(int length, std::function<void(std::vector<char>,asio::error_code,std::size_t)> callBack);                          //Receives data
};

#endif // CONNECTIONMANAGER_H
