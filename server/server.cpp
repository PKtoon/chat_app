#include "server.h"
#include "user.h"

void Server::accept()
{
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket)
    {
        if (!ec)
        {
            User* u = new User{this,std::move(socket)};
            u->intro();
            userList.push_back(u);
        }
        accept();
    });
}

void Server::removeUser()
{
    t.expires_after(boost::asio::chrono::seconds(timeout));
    t.async_wait([this](const boost::system::error_code& error)
    {
        std::cout<<"removal instance"<<std::endl;
        for(int i=0; i<userList.size(); i++)
            if(!userList[i]->getStatus())
            {
                userList[i]->getSocket()->close();
                std::cout<<userList[i]->getName()<<" being kicked out"<<std::endl;
                userList.erase(userList.begin()+i);
            }
        isAlive();
    });
}

void Server::isAlive()
{
    t.expires_after(boost::asio::chrono::seconds(timeout));
    t.async_wait([this](const boost::system::error_code& error)
    {
        for(auto a:userList)
            a->pingMe();
        removeUser();
    });
}

bool Server::searchUser(std::string name)
{
    for(auto a:userList)
    {
        if(name==a->getName())
            return true;
    }
    return false;
}

User* Server::getUser(std::string name)
{
    for(int i=0; i<userList.size(); i++)
        if(name==userList[i]->getName())
            return userList[i];
    return nullptr;
}
