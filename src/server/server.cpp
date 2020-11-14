#include "server.hpp"
#include "user.hpp"

void Server::accept()
{
    acceptor.async_accept([this](asio::error_code error, asio::ip::tcp::socket socket)
    {
        if (error)
        {
            if (error != asio::error::operation_aborted)
            {
                std::cerr<<"Server::accept()::async_accept(): "<<error.message()<<std::endl;
                accept();
            }
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
        User* user = getActiveUser(data.receiver);
        if(!user)
        {
            user = getActiveUser(data.sender);
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

void Server::addUser(std::string name, std::string passwd)
{
    db.execCommit("INSERT INTO users (name,passwd) VALUES ('"+name+"','"+passwd+"');");
}

pqxx::result Server::getUser(std::string name)
{
    return db.exec("SELECT name FROM users WHERE name = '"+name+"';");
}

bool Server::authUser(std::string name, std::string passwd)
{
    pqxx::result res = db.exec("SELECT name,passwd FROM users WHERE name = '"+name+"';");
    if(res.size() == 1)
        if(res[0][0].c_str() == name && res[0][1].c_str() == passwd)
            return true;
    return false;
}

User* Server::getActiveUser(std::string name)
{
    for(auto& a:userList)
        if(a->getName()==name)
            return a.get();
    return nullptr;
}
