#include <iostream>
#include <boost/asio.hpp>
#include <string>
//#include "old/2019-07-01/message.h"
#include "message_string.h"

using boost::asio::ip::tcp;

class Client
{
    std::string name;
    tcp::socket socket;
    tcp::resolver::results_type endpoints;
    Message writebuff,readbuff;
public:
    Client(std::string n,boost::asio::io_context& io,tcp::resolver::results_type endpoint):name{n},socket{io},endpoints{endpoint}
    {
        connector();
    }
    void connector();
    void writer();
    void reader();
    void intro();
};

void Client::connector()
{
    boost::asio::async_connect(socket,endpoints,[this](const boost::system::error_code& error, const tcp::endpoint& endpoint)
    {
        intro();
        reader();
        writer();
    });
}
void Client::writer()
{
    std::string receiver,msg;
    std::cout<<"To:: "<<std::endl;
    std::cin>>receiver;
    std::cout<<"Message::"<<std::endl;
    std::cin>>msg;
    writebuff.makeMsg(name,receiver,msg);
    boost::asio::async_write(socket,boost::asio::buffer(writebuff.getdata(), writebuff.max_length),[this](const boost::system::error_code& error, std::size_t t)
    {
        writer();
    });
}

void Client::reader()
{
    boost::asio::async_read(socket,boost::asio::buffer(readbuff.getdata(), readbuff.max_length),[this](const boost::system::error_code& error, std::size_t t)
    {
        readbuff.remakeMsg();
        std::cout<<"From:: "<<readbuff.getSender()<<std::endl<<"Message:: "<<readbuff.getMsg()<<std::endl;
        reader();
    });
}

void Client::intro()
{
    writebuff.makeMsg(name,"server","name init");
    boost::asio::write(socket,boost::asio::buffer(writebuff.getdata(),Message::max_length));
}

int main(int argc, char* argv[])
{
    if (argc!=4)
    {
        std::cout<<"Usage: client id hostname port"<<std::endl;
        exit(1);
    }
    boost::asio::io_context io;

    std::string id{argv[1]};
    std::string machine_name(argv[2]);
    std::string port{argv[3]};

    tcp::resolver resolver (io);
    tcp::resolver::results_type endpoints(resolver.resolve(machine_name,port));

    Client c{id,io,endpoints};

    io.run();
    return 0;

}
