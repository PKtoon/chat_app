#include "server.h"
#include "user.h"

void Server::accept()
{
    acceptor.async_accept([this](asio::error_code error, asio::ip::tcp::socket socket)
    {
        if (error)
        {
            if (error != asio::error::operation_aborted)
                accept();
        }
        else
        {
            userList.emplace_back(std::make_unique<User>(std::move(socket),*this));
            accept();
        }
    });
}

void Server::removeMe(User* user)
{
    for(int i=0; i<userList.size(); i++)
    {
        if(userList[i].get()==user)
            userList.erase(userList.begin()+i);
    }
}

//this deliverMessages function has same problems as User::writer()
//debatable architecture question here. rather than giving message delivery to server and making one more buffer, just make user class to access other user class' message queue like old design.
//but this will have problem when all of this become concurrent when many Users try to access same User's message queue. this problem will persist even if server is used for message delivery

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
