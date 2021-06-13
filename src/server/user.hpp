#ifndef USER_H
#define USER_H

/**************************************************************************************
 * User handles clients.
 *
 * Author: Pratik Khanke
 *
 * Tasks of User:
 *      Handles clients.
 *      Handles socket.
 *      Actually sends and receive messages.
 *      Uses ping to check if client is connected.
 *
 * TODO:
 *      Properly handle situation where client is already signed in
 *      Group related operations
 *
 **************************************************************************************
 */


#ifndef NDEBUG
#include <iostream>
#endif

#include <list>

#include "network-interface/netface.hpp"

class Server;

class User
{
    std::string name;       //Name of client connected
#ifndef NDEBUG
    std::string name2;
#endif
    bool isAlive{false};                    //Tells if client is still connected
    unsigned short count = 0;               //Retries counter
    std::list<Stream> writeQueue;           //Messages to be sent
    std::mutex writeQueueMutex;
    unsigned int currentQueueIndex = 0;     //Necessary for making circular queue
    NetFace net;                            //Network Interface
    Server& server;                         //Server reference to access services
    asio::steady_timer pulseTimer {net.getSocket()->get_executor(),asio::chrono::seconds(300)};         //Wait between successive pings
    asio::steady_timer writeTimer {net.getSocket()->get_executor()};                                    //Wait between successive write intervals
public:
    User(asio::ip::tcp::socket socket, Server& serv);
#ifndef NDEBUG
    ~User()
    {
        std::clog<<name2+" is destroyed"<<std::endl;
    }
#endif
    const std::string& getName() const { return name; }
    void queueMessage(Stream);              //Queues message to send
private:
    void authHandler(Stream data);          //Authenticates the client
    void reader();                          //Receives message from client
    void writer();                          //Sends message
    void writeScheduler();                  //Schedules next write loop
    void processData(Stream);               //Processes received message
    void pingMe();                          //Pings client
    void checkPulse();                      //Schedules next ping
    void findContact(Stream data);          //Tells client if contact exists
    void checkPendingMessages();            //Checks for pending messages when offline
    void findGroup(Stream data);            //Tells client if group exists
};

#endif
