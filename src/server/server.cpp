#include "server.hpp"
#include "user.hpp"

void Server::accept()
{
    acceptor.async_accept([this](asio::error_code error, asio::ip::tcp::socket socket)
    {
        if (!error)
        {
            std::lock_guard<std::mutex> lock(userListMutex);
            userList.emplace_back(std::make_unique<User>(std::move(socket),*this));
        }
#ifndef NDEBUG
        else{
            std::cerr<<"Server::accept()::async_accept(): "<<error.message()<<std::endl;
        }
#endif
        accept();

    });
}

void Server::removeMe(User* user)
{
    std::lock_guard<std::mutex> lock(userListMutex);
    for(int i=0; i<userList.size(); i++)
    {
        if(userList[i].get()==user)
        {
            userList.erase(userList.begin()+i);
        }
    }
}

void Server::deliverMessages()
{
    std::list<Stream> immediateList;
    {
        std::lock_guard<std::mutex> lock(deliveryListMutex);
        immediateList.swap(deliveryList);
    }
    while(!immediateList.empty())
    {
        Stream data = (*immediateList.begin());
        User* user = getActiveUser(data.receiver);
        if(!user)
        {
            user = getActiveUser(data.sender);
            data.data1 = data.receiver+" not found";
            data.receiver = data.sender;
            data.sender = "server";
        }
        user->queueMessage(data);
        immediateList.pop_front();
    }
    deliveryScheduler();
}

void Server::deliveryScheduler()
{
    deliveryTimer.expires_after(asio::chrono::seconds(1));
    deliveryTimer.async_wait([this](const asio::error_code& error)
    {
        if(error != asio::error::operation_aborted)
        {
            deliverMessages();
        }
    });
}

void Server::queueDelivery(Stream data)
{
    std::lock_guard<std::mutex> lock(deliveryListMutex);
    deliveryList.push_back(data);
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
    //We surely do not want a pointer to erased User
    std::lock_guard<std::mutex> lock(userListMutex);
    for(auto& a:userList)
        if(a->getName()==name)
            return a.get();
    return nullptr;
}
