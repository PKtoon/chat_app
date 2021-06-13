#ifndef CLIENT_HPP
#define CLIENT_HPP

/**************************************************************************************
 * Client provides basic client functions and handles database
 *
 * Author: Pratik Khanke
 *
 * Tasks of Client:
 *      Provide convenient wrapper for sending and receiving messages
 *      Provide functions for finding contact and groups
 *      Handles database
 **************************************************************************************
 */

#include <deque>

#include "sqlite3_wrap.h"
#include "network-interface/netface.hpp"
/**
 * @todo write docs
 */

class Client
{
    std::string name_;                      //Name of client
    asio::io_context io_;                   //Executor for networking
    NetFace net;                            //Network interface
    bool isWriting{false};                  //Status of writing loop
    std::deque<Stream> writeQueue;          //List of messages to be sent
    SQLite3DB db{"storage"};                //SQLite3 db handler
    bool isConnected {false};               //Is client connected to server
    
public:
    //Types of contact enum
    enum ContactType {
        individual,
        group 
    };
    
    Client(std::string name = "") : net{io_} {}

    Client(asio::io_context& io,std::string name = ""): net{io} {}
    
    enum class Error{
        ok, db_error
    };
    
    //client
    void userAuthInit(std::string name, std::string password, Header head);                         //Initializes name_ and makes message for sign in/up
    std::string name() { return name_; }
    void reader(std::function<void (Stream data,asio::error_code,std::size_t)> callBack);           //Receives message
    void writer();                                                                                  //Sends message
    void processData(Stream data);                                                                  //Process data according to header of Stream data
    void processMessage(Stream data);                                                               //Called by Client::processData, processes messages adds unknown contacts to database, insert messages to database
    Client::Error queueMessage(Stream data);                                                        //Queues message to be sent
    void ping();                                                                                    //pings back server

    //networking
    void connect(std::string host, std::string port, std::function<void(asio::error_code)> callBack);
    void disconnect();
    void runIOContext();                                                                                    //Runs the 'default' executor
    asio::ip::tcp::socket* getSocket();
    void newSocket();

    //db
    std::string getDBError();                                                                               //Returns SQLite3 error message
    void initDB();                                                                                          //Initializes database for specific user
    bool getContact(std::string name, bool& result);                                                        //Checks if contact exists
    bool getContactList(std::vector<std::pair<std::string, int> > &list);                                   //Get list of pair of contact and its type
    bool getMessages(std::string subject, std::vector<std::pair<std::string,std::string>>& msg);            //Gets sender and message pair for specific contact
    bool insertContact(std::string name, ContactType type);                                                 //Insert contact with type into database
    bool insertMessage(std::string subject, std::string sender, std::string msg);                           //Insert message into database
};

#endif // CLIENT_HPP
