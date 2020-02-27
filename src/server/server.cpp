#include "server.h"
#include "user.h"

void Server::accept()
{
    acceptor.async_accept([this](boost::system::error_code error, tcp::socket socket)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                accept();
        }
        else
        {
            new User{std::move(socket),*this};
            accept();
        }
    });
}

void Server::addMe(User* user)
{
    userList.emplace_back(user);
}

void Server::removeMe(User* user)
{
    for(int i=0; i<userList.size(); i++)
    {
        if(userList[i].get()==user)
            userList.erase(userList.begin()+i);
    }
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

User* Server::getUser(std::string name)
{
    for(auto& a:userList)
        if(a->getName()==name)
            return a.get();
    return nullptr;
}
