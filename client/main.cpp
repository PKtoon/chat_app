#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <list>
#include <thread>
#include "../message.h"
#include "client.h"

using boost::asio::ip::tcp;

std::string readline()
{
    std::string temp="";
    char i;
    std::cin>>std::noskipws>>i;
    if(i!='\n')
        std::cin.unget();
    while(std::cin>>std::noskipws>>i)
    {
        if(i=='\n')
            break;
        temp+=i;
    }
    return temp;
}

int main(int argc, char* argv[])
{
    if (argc!=4)
    {
        std::cout<<"Usage: client id hostname port"<<std::endl;
        exit(1);
    }

    std::string id{argv[1]};
    std::string machine_name(argv[2]);
    std::string port{argv[3]};
    std::string receiver,msg;

    boost::asio::io_context io;
    tcp::resolver resolver (io);
    tcp::resolver::results_type endpoints(resolver.resolve(machine_name,port));

    Client c{id,io,endpoints};

    std::thread t {[&io](){ io.run(); }};

    while(true)
    {
        std::cout<<"To:: ";
        std::cin>>receiver;
        std::cout<<"Message:: ";
        msg = readline();
        c.writer(receiver,msg);
        c.printer();
    }

    t.join();
    return 0;
}
