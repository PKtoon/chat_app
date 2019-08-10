#include <iostream>
#include <boost/asio.hpp>
#include <deque>
#include "server.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    if (argc!=2)
    {
        std::cout<<"Usage: server port"<<std::endl;
        exit(1);
    }
//    boost::asio::io_context io;

    tcp::endpoint endpoint(tcp::v4(),std::atoi(argv[1]));       //needs ip version to use and port number

    Server s(endpoint);
//    io.run();
    return 0;
}
