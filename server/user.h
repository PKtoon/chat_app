#ifndef USER_H
#define USER_H

#include <boost/asio.hpp>
#include <iostream>
#include "../stream/stream.h"

class Server;

using boost::asio::ip::tcp;


class User
{
    std::string name {"uninitialized"};
    tcp::socket socket;
    bool alive{true};
    unsigned int headerLength = 4;
    std::vector<char> inHeader;
    std::vector<char> inData;
    unsigned long inDataSize=0;
    Server *serv;
public:
    User(Server* q,tcp::socket s):socket{std::move(s)},serv{q}{}
    ~User(){ socket.close(); std::cerr<<name<<" is destroyed"<<std::endl; }
    void intro();
    void readHeader();
    void readBody();
    void writer(Stream st);
    const std::string& getName() const { return name; }
    tcp::socket* getSocket() { return &socket; }
    bool getStatus() const { return alive; }
    void pingMe();

};

#endif // USER_H
