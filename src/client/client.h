#ifndef CLIENT_H
#define CLIENT_H

#include <list>
#include <boost/asio.hpp>
#include "../stream/stream.h"

using boost::asio::ip::tcp;

class Client
{
    std::string name;
    tcp::socket socket;
    tcp::resolver::results_type endpoints;
    
    std::list<Stream> buff;
    
    std::vector<char> inHeader;
    std::vector<char> inData;
    unsigned int inDataSize=0;
    unsigned int headerLength=4;

public:
    Client(std::string& n,boost::asio::io_context& io,tcp::resolver::results_type& endpoint)
        :name{std::move(n)},socket{io},endpoints{std::move(endpoint)}
    {
        connector();
    }
    std::string getName() { return name; }
    void connector();
    void intro();
    void writer(Stream);
    void readHeader();
    void readBody();
    void printer();
};

#endif // CLIENT_H
