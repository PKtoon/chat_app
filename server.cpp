#include <iostream>
#include <boost/asio.hpp>
#include <deque>
#include "message_string.h"
#include "user.h"

using boost::asio::ip::tcp;

class Server
{
    boost::asio::io_context& io_;
    tcp::acceptor acceptor;
    boost::asio::steady_timer t{io_,boost::asio::chrono::seconds(30)};
    Message msg;
    void accept();
    void removeUser();

public:
    Server(boost::asio::io_context& io,tcp::endpoint& endpoint):acceptor(io,endpoint),io_{io}
    {
        accept();
        removeUser();
    }
};

void Server::accept()
{
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket)
    {
        if (!ec)
        {
            User* u = new User{std::move(socket)};
            u->run();
            user_list.push_back(u);
        }
        accept();
    });
}

void Server::removeUser()
{
    t.expires_after(boost::asio::chrono::seconds(30));
    t.async_wait([this](const boost::system::error_code& error)
    {
        std::cout<<"removal instance"<<std::endl;
        if(!rem_list.empty())
        {
            for (auto i : rem_list)
            {
                int j=0;
                while(j<user_list.size())
                {
                    if(i==user_list[j]->name)
                    {
                        std::cout<<user_list[j]->name<<" being deleted"<<std::endl;
                        user_list.erase(user_list.begin()+j);
                        break;
                    }
                    j++;
                }
            }
        }
        removeUser();
    });
}

int main(int argc, char* argv[])
{
    if (argc!=2)
    {
        std::cout<<"Usage: server port"<<std::endl;
        exit(1);
    }
    boost::asio::io_context io;

    tcp::endpoint endpoint(tcp::v4(),std::atoi(argv[1]));       //needs ip version to use and port number

    Server s(io,endpoint);
    io.run();
    return 0;
}
