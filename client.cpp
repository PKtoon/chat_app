#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <list>
#include "message_string.h"

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

class Client
{
    std::string name;
    tcp::socket socket;
    tcp::resolver::results_type endpoints;
    Message writebuff,readbuff;
    std::list<Message> buff;
public:
    Client(std::string n,boost::asio::io_context& io,tcp::resolver::results_type endpoint):name{n},socket{io},endpoints{endpoint}
    {
        connector();
    }
    void connector();
    void writer();
    void reader();
    void intro();
    void printer();
};

void Client::connector()
{
    boost::asio::async_connect(socket,endpoints,[this](const boost::system::error_code& error, const tcp::endpoint& endpoint)
    {
        if (!error)
        {
            intro();
            reader();
            writer();
        }
        else {
            connector();
        }
    });
}
void Client::writer()
{
    std::string receiver,msg;
    std::cout<<"To:: ";
    std::cin>>receiver;
    std::cout<<"Message:: ";
    msg = readline();
    writebuff.makeMsg(name,receiver,msg);
    std::cout<<std::endl;
    boost::asio::async_write(socket,boost::asio::buffer(writebuff.getdata(), writebuff.max_length),[this](const boost::system::error_code& error, std::size_t)
    {
        printer();
        writer();
    });
}

void Client::reader()
{
    boost::asio::async_read(socket,boost::asio::buffer(readbuff.getdata(), readbuff.max_length),[this](const boost::system::error_code& error, std::size_t)
    {
        readbuff.remakeMsg();
        buff.push_back(readbuff);
        reader();
    });
}

void Client::intro()
{
    writebuff.makeMsg(name,"server","name init");
    boost::asio::write(socket,boost::asio::buffer(writebuff.getdata(),Message::max_length));
}

void Client::printer()
{
    while(!buff.empty())
    {
        std::cout<<"\nFrom: "<<buff.begin()->getSender()<<std::endl;
        std::cout<<"Message: "<<buff.begin()->getMsg()<<std::endl<<std::endl;
        buff.pop_front();
    }
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
