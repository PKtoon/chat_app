#ifndef CLIENT_H
#define CLIENT_H

#include <list>
#include <boost/asio.hpp>
#include "../message.h"

using boost::asio::ip::tcp;

class Client
{
    std::string name;
    tcp::socket socket;
    tcp::resolver::results_type endpoints;
    Message writebuff,readbuff;
    std::list<Message> buff;
public:
    Client(std::string n,boost::asio::io_context& io,tcp::resolver::results_type endpoint)
        :name{n},socket{io},endpoints{endpoint}
    {
        connector();
    }
    void connector();
    void intro();
    void writer(std::string, std::string);
    void reader();
    void printer();
};

#endif // CLIENT_H
