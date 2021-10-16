#ifndef SERVER_H
#define SERVER_H

/*
 **********************************************************************************************************************************************************************
 * Server serves 'Users' created by Server itself.
 * 
 * Author: Pratik Khanke
 * 
 * Tasks of Server:
 *      Handles IOContext
 *      Accepts incoming connections
 *      Maintains list of active users
 *      Maintains database of all users, pending messages, groups and group members
 *      Delivers messages to appropriate active User object
 *      Stores messages of inactive users
 *      Creates and manages groups of user
 *      Give required services to User class
 *
 * Required Tables in PostgreSQL database:
 *      create table users (username text PRIMARY KEY NOT NULL, passwd text NOT NULL);
 *      create table pending (ID serial PRIMARY KEY NOT NULL, username text NOT NULL, message text NOT NULL, timestamp TIMESTAMPTZ NOT NULL);
 *      create table group_members (ID integer PRIMARY KEY NOT NULL, username text NOT NULL, groupname text NOT NULL);
 *
 *  TODO:
 *      1. In createGroup(), usernames are separated using ';', so prevent use of that symbol in username or do something to avoid potential error in username.
 *      2. In createGroup(), make sure every members get message of "Group Created".
 *      3. Make Users in userList a shared_ptr. This will prevent accessing a nullptr.
 *      4. Admin system for group.
 **********************************************************************************************************************************************************************
 */


#include <vector>

#include "user.hpp"
#include "psql-wrap.hpp"

class Server
{
    asio::io_context io;                        //Executor for network operations
    asio::ip::tcp::endpoint endpoint;           //Endpoint to bind server
    asio::ip::tcp::acceptor acceptor;           //Accepts connections
     
    std::vector<std::unique_ptr<User>> userList;        //List of active users
    std::mutex userListMutex;
    std::list<Stream> deliveryList;                     //List of messages to be delievered
    std::mutex deliveryListMutex;
    asio::steady_timer deliveryTimer {io,asio::chrono::seconds(1)};     //Wait between successive delievery loop

    pk::PSQLdb db;              //Handles database operations

    void accept();              //Accepts incoming connections
    void deliverMessages();     //Loop that delivers or stores pending messages
    void deliveryScheduler();   //Schedule execution of delivery loop after specified time

public:
    Server(unsigned short port, std::string dbUri) : endpoint{asio::ip::tcp::v6(),port}, acceptor{io,endpoint}, db{dbUri}
    {
        accept();
        deliveryScheduler();
        io.run();
    }
  
    User* getActiveUser(std::string);                                   //Gives User pointer of currently active user
    void removeMe(User*);                                               //Users call this function when they disconnects from client which removes them from active users list and destroys them
    void queueDelivery(Stream);                                         //queues the message to deliveryList of server which later delivers it

    void addUser(std::string name, std::string passwd);                 //User object calls this function if the client attached to it is not is in server's database that is client is new
    pqxx::result getUser(std::string name);                             //Finds a user in database and returns the database result
    bool authUser(std::string name, std::string passwd);                //Checks if user is present in database and compares the password. If both matched then it returns true else false
    std::list<Stream> getPendingMessages(std::string name);             //User object calls this function to get pending messages from database
    void storePendingMessage(std::string subject,Stream data);          //Stores messages which were unable to sent to client
    void sendUserMessage(Stream data);                                  //Send message to a User object or stores as pending message
    void sendGroupMessage(Stream data);                                 //Sends a message to members of a group or stores as pending message of inactive members
    std::list<std::string> getGroupMembers(std::string groupName);      //Retrieves a list of members belonging to a particular group
    void createGroup(Stream data);                                      //Creates a group
    pqxx::result getGroup(std::string groupName);                       //Get name of group from database
};

#endif // SERVER_H
