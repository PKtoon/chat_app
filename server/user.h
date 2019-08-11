#ifndef USER_H
#define USER_H

#include <iostream>
#include <boost/asio.hpp>

#include "../message.h"

class Server;

using boost::asio::ip::tcp;

//enum class UserSig {SUCCESS, NAME_NOT_RECEIVED};

class User
{
    std::string name;
    tcp::socket socket;
    bool alive{true};
public:
    Server *s;
    Message readbuff,writebuff;
    User(std::string n, tcp::socket s):name{n},socket{std::move(s)}{}
    User(tcp::socket s):socket{std::move(s)}{}
    User(Server* q,tcp::socket s):socket{std::move(s)},s{q}{}
    void intro();
    void reader();
    void writer();
    std::string getName() const { return name; }
    tcp::socket* getSocket() { return &socket; }
    bool getStatus() const { return alive; }
    void pingMe();
};

#endif // USER_H
