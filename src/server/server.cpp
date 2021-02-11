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
        switch(data.head) {
            case Header::message:
                sendUserMessage(data);
                break;
            case Header::group_message:
                sendGroupMessage(data);
                break;
            default:
                break;
        }
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

void Server::storePendingMessage(Stream data)
{
    db.execCommit("INSERT INTO pending (name,message,timestamp) VALUES ('"+data.receiver+"','"+data.getSerialized()+"','now()');");
}

std::list<Stream> Server::getPendingMessages ( std::string name )
{
    std::list<Stream> list;
    pqxx::result res = db.exec("SELECT message FROM pending WHERE name = '"+name+"' ORDER BY timestamp;");
    for(unsigned int i = 0; i < res.size(); i++) {
        Stream data(res[i][0].c_str());
        list.push_back(data);
    }
    db.execCommit("DELETE FROM pending WHERE name = '"+name+"';");
    return list;
}

std::list<std::string> Server::getGroupMembers ( std::string groupName )
{
    std::list<std::string> list;
    pqxx::result res = db.exec("SELECT username FROM group_members WHERE groupname = '"+groupName+"';");
    for(unsigned int i =0; i < res.size(); i++) {
        list.push_back(res[i][0].c_str());
    }
    return list;
}

void Server::sendUserMessage ( Stream data )
{
    User* user = getActiveUser(data.receiver);
    if(user)
        user->queueMessage(data);
    else {
        pqxx::result res = getUser(data.receiver);
        if(res.size() == 1 && res[0][0].c_str() == data.receiver) {
            storePendingMessage(data);
        }
        else {
            data.data1 = data.receiver+" not found";
            data.receiver = data.sender;
            data.sender = "server";
            queueDelivery(data);
        }
    }
}

void Server::sendGroupMessage ( Stream data )
{
    std::list<std::string> list {getGroupMembers(data.receiver)};
    for(auto& username : list) {
        User* user = getActiveUser(username);
        if(user)
            user->queueMessage(data);
        else {
            pqxx::result res = getUser(username);
            if(res.size() == 1 && res[0][0].c_str() == username) {
                storePendingMessage(data);
            }
        }
    }
}

void Server::createGroup ( Stream data )
{
    Stream reply;
    pqxx::result res = db.exec("SELECT groupname FROM group_members WHERE groupname = '"+data.data1+"';");
    if(res.size() != 0) {
        reply.head = static_cast<Header>(Header::group_create|Header::error);
        reply.sender = "server";
        reply.receiver = data.sender;
        reply.data1 = "Group already exist.";
    }
    else {
        std::list<std::string> list;
        std::string temp{""};
        for(auto itr = data.data2.begin(); itr != data.data2.end(); itr++) {
            if(*itr != ';') {
                temp += *itr;
            }
            else {
                list.push_back(temp);
                temp.clear();
            }
        }
        db.execCommit("INSET INTO group_members (username,groupname) VALUES ('"+data.sender+"','"+data.data1+"');");
        for(auto& username : list) {
            pqxx::result res = getUser(username);
            if(res.size() == 1 && res[0][0].c_str() == username) {
                db.execCommit("INSET INTO group_members (username,groupname) VALUES ('"+username+"','"+data.data1+"');");
            }
        }
        reply.head = Header::group_message;
        reply.sender = "server";
        reply.receiver = data.data1;
        reply.data1 = "Group created";
    }
    queueDelivery(reply);
}
