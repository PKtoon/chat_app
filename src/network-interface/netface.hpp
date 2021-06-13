#ifndef NETFACE_H
#define NETFACE_H

/**************************************************************************************
 * NetFace is network interface and implements the protocol for communication
 *
 * Author: Pratik Khanke
 *
 * Tasks of NetFace:
 *      Provide convenient wrapper for sending and receiving protocol
 *      Handles socket
 *      Handles connecting to server
 * 
 * TODO:
 *      Properly handle ConnectionManager
 *      A good structure (multiple ConnectionManager or not)
 *      Remove magic numbers and take them as arguement
 *
 **************************************************************************************
 */

#include "connection-manager/connection-manager.hpp"
#include "stream/stream.hpp"

#include <deque>

class NetFace
{
    int headerLength = 4;                                               //Header length to store size of payload
    ConnectionManager* connMan = nullptr;
    asio::steady_timer timer;
    std::deque<std::unique_ptr<ConnectionManager>> socketQueue;
    bool isTimerRunning {false};
    
public:
    NetFace(asio::io_context& io);
    NetFace(asio::ip::tcp::socket sock);
    ~NetFace()
    {
        if(connMan)
            delete connMan;
    }
    
    asio::ip::tcp::socket* getSocket();                                                                             //Returns pointer to socket
    ConnectionManager* getConnMan() { return connMan; }                                                             //Returns pointer to ConnectionManager
    void connect(std::string hostname, std::string port, std::function<void(asio::error_code)> callBack);           //Connects to given host and port
    asio::error_code disconnect();                                                                                  //disconnects
    void removeConnMan();
    void newConnection(asio::io_context& io);
    void newConnection(asio::ip::tcp::socket sock);
    void send(Stream data, std::function<void(asio::error_code,std::size_t)> callBack);                             //Sends data using protocol
    void receive(std::function<void(Stream,asio::error_code,std::size_t)> callBack);                                //Receives data using protocol
};

#endif // NETFACE_H
