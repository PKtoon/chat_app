#ifndef USER_H
#define USER_H

#include <boost/asio.hpp>

#include "../stream/stream.h"

class Server;

using boost::asio::ip::tcp;


class User
{
    std::string name;
    tcp::socket socket;
    bool alive{true};
    unsigned int headerLength = 4;
    std::vector<char> inHeader;
    std::vector<char> inData;
    unsigned long inDataSize=0;
public:
    Server *serv;

//    User(std::string n, tcp::socket s):name{n},socket{std::move(s)}{}
//    User(tcp::socket s):socket{std::move(s)}{}
    User(Server* q,tcp::socket s):socket{std::move(s)},serv{q}{}
    void intro();
    void readHeader();
    void readBody();
    void writer(Stream st);
    std::string getName() const { return name; }
    tcp::socket* getSocket() { return &socket; }
    bool getStatus() const { return alive; }
    void pingMe();

};

#endif // USER_H
