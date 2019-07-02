#include <iostream>
#include <boost/asio.hpp>
#include <vector>
#include <boost/lexical_cast.hpp>
//#include "old/2019-07-01/message.h"
#include "message_string.h"

using boost::asio::ip::tcp;

int searchUser(std::string );

class User
{
public:
    std::string name;
    tcp::socket socket;
    Message readbuff,writebuff;
    User(std::string n, tcp::socket s):name{n},socket{std::move(s)}{}
    User(tcp::socket s):socket{std::move(s)}{}
    void reader();
    void writer();
    void intro();
    void run()  { intro(); reader(); }
};

std::vector<User*> user_list;

void User::intro()
{
    boost::asio::read(socket,boost::asio::buffer(readbuff.getdata(),Message::max_length));
    readbuff.remakeMsg();
    name=readbuff.getSender();
}

void User::reader()
{
    boost::asio::async_read(socket,boost::asio::buffer(readbuff.getdata(),readbuff.max_length),[this](const boost::system::error_code& error, std::size_t t)
    {
        writebuff.remakeMsg(readbuff.getdata());
        writer();
        reader();
    });
}

void User::writer()
{
    int i=searchUser(writebuff.getReceiver());
    boost::asio::async_write(user_list[i]->socket,boost::asio::buffer(writebuff.getdata(),writebuff.max_length),[this](const boost::system::error_code& error, std::size_t t)
    {
//        writer();
    });
}

int count=0;

class Server
{
    tcp::acceptor acceptor;
    Message msg;
    void accept();
public:
    Server(boost::asio::io_context& io,tcp::endpoint& endpoint):acceptor(io,endpoint)
    {
        accept();
    }
};

void Server::accept()
{
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket)
    {
        if (!ec)
        {
            User* u = new User{"kp",std::move(socket)};
            u->run();
            user_list.push_back(u);
            std::cout<<"connected"<<std::endl;
        }
        accept();
    });
}

int searchUser(std::string s)
{
    for(int i = 0;i<user_list.size();i++)
        if(s==user_list[i]->name)
            return i;
    return -1;
}

int main(int argc, char* argv[])
{
    boost::asio::io_context io;

    tcp::endpoint endpoint(tcp::v4(),std::atoi(argv[1]));       //needs ip version to use and port number

    Server s(io,endpoint);
    io.run();
    return 0;

}
