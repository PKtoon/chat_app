#include "server.h"
#include "user.h"
#include "../logging.h"

void Server::accept()
{
    acceptor.async_accept([this](boost::system::error_code error, tcp::socket socket)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                accept();
            logError("server",": accept(): error: ", error);
        }
        else
        {
            User* u = new User{this,std::move(socket)};
            u->intro();
            userList.push_back(u);
            accept();
        }
    });
}

void Server::removeUser()
{
    t.expires_after(boost::asio::chrono::seconds(timeout));
    t.async_wait([this](const boost::system::error_code& error)
    {
        logIt("server: ","removal instance");
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                removeUser();
            logError("server: ",": removeUser(): error: ", error);
        }
        else
        {
            for(unsigned long i=0; i<userList.size(); i++)
                if(!userList[i]->getStatus())
                {
                    logIt("server: ",userList[i]->getName()+" being kicked out");
                    userList.erase(userList.begin()+i);
                }
            isAlive();
        }
    });
}

void Server::isAlive()
{
    t.expires_after(boost::asio::chrono::seconds(timeout));
    t.async_wait([this](const boost::system::error_code& error)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                isAlive();
            logError("server: ",": isAlive(): error: ", error);
        }
        else
        {
            for(auto a:userList)
                a->pingMe();
            removeUser();
        }
    });
}

User* Server::getUser(std::string name)
{
    for(unsigned long i=0; i<userList.size(); i++)
        if(name==userList[i]->getName())
            return userList[i];
    return nullptr;
}
