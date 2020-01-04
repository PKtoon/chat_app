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
            new User{std::move(socket),this};
            std::cout<<"new connect"<<std::endl;
            accept();
        }
    });
}

void Server::addMe(User* user)
{
    userList.emplace_back(user);
}

void Server::deliverMessages()
{
    isDelivering = true;
    while(!deliveryQueue.empty())
    {
        Stream data = (*deliveryQueue.begin());
        User* user = getUser(data.receiver);
        if(!user)
        {
            user = getUser(data.sender);
            data.data1 = data.receiver+" not found";
            data.receiver = data.sender;
            data.sender = "server";
        }
        user->queueMessage(data);
        deliveryQueue.pop_front();
    }
    isDelivering = false;
}

void Server::queueDelivery(Stream data)
{
    deliveryQueue.push_back(data);
    if(!isDelivering)
        deliverMessages();
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
            //for vector subscripting is better
//            for(auto iter = userList.begin(); iter!=userList.end();)
//                if(!(*iter)->getStatus())
//                {
//                    logIt("server: ",(*iter)->getName()+" being kicked out");
//                    auto rem = iter;
//                    iter++;
//                    userList.erase(rem);
//                }
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
            for(auto& a:userList)
                a->pingMe();
            removeUser();
        }
    });
}

User* Server::getUser(std::string name)
{
    for(auto& a:userList)
        if(a->getName()==name)
            return a.get();
    return nullptr;
}
