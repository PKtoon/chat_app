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
    int inDataSize=0;
    int headerLength=4;

public:
    Client(std::string n,boost::asio::io_context& io,tcp::resolver::results_type endpoint)
        :name{n},socket{io},endpoints{endpoint}
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
