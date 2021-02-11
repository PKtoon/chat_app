#include "server.hpp"
#include "user.hpp"

User::User(asio::ip::tcp::socket socket, Server& serv) : net{std::move(socket)}, server{serv}
{
    //First invocation of pulseTimer acts as sign in timeout. That is why it is 300 seconds for first time and then 20 seconds
    pulseTimer.async_wait([this](const asio::error_code& error)
    {
        if(error != asio::error::operation_aborted)
            checkPulse();
    });
    reader();
    writeScheduler();
}

void User::authHandler(Stream data)
{
    Stream reply;
    reply.head = static_cast<Header>(data.head|Header::error);
    reply.sender = "server";

    if(count < 5 && name.empty())
    {
        //TODO: if there are any active user with same name then first disconnect them after five counts disconnect

        count++;
        switch (data.head)
        {
        case Header::signin:
            if(server.authUser(data.sender,data.data1))
            {
                name = data.sender;
                name2 = name;
                reply.receiver = name;
                reply.head = static_cast<Header>(Header::signin|Header::ack);
                isAlive = true;
            }
            else{
                reply.data1 = "Username or password is incorrect";
            }
            queueMessage(reply);
            checkPendingMessages();
            break;
        case Header::signup:
        {
            pqxx::result res = server.getUser(data.sender);
            if(res.size() == 0){
                server.addUser(data.sender,data.data1);
                name = data.sender;
                name2 = name;
                reply.receiver = name;
                reply.head = static_cast<Header>(Header::signup|Header::ack);
                isAlive = true;
            }
            else{
                reply.data1 = "Username not available";
            }
            queueMessage(reply);
        }
            break;
        default:
            break;
        }
    }
}

void User::processData(Stream data)
{
    if(name.empty())
    {
        switch(data.head)
        {
            case Header::signin:
            case Header::signup:
                authHandler(data);
                break;
            default:
                break;
        }
    }
    else
    {
        switch(data.head)
        {
            case Header::message:
            case Header::group_message:
                server.queueDelivery(data);
                break;
            case Header::ping:
                isAlive = true;
                break;
            case Header::socket_close:
                isAlive = false;
                name = "";
                {
                    std::lock_guard<std::mutex> lock(writeQueueMutex);
                    for(auto& a : writeQueue) {
                        server.storePendingMessage(a);
                    }
                    writeQueue.clear();
                }
                //net.disconnect();
                break;
            case Header::find_contact:
                findContact(data);
                break;
            case Header::find_group:
                findGroup(data);
                break;
            case Header::group_create:
                server.createGroup(data);
                break;
            default:
                break;
        }
    }
}

void User::checkPulse()
{
    if(!isAlive)
        server.removeMe(this);
    else
    {
        pingMe();
        pulseTimer.expires_after(asio::chrono::seconds(20));
        pulseTimer.async_wait([this](const asio::error_code& error)
        {
            if(error != asio::error::operation_aborted)
            {
                checkPulse();
            }
        });
    }
}

void User::findContact(Stream data)
{
    Stream reply;
    reply.head = static_cast<Header>(Header::find_contact|Header::error);
    reply.sender = "server";
    reply.receiver = name;
    reply.data1 = data.data1;

    pqxx::result res = server.getUser(data.data1);
    if(res.size() == 1 && res[0][0].c_str() == data.data1){
        reply.head = static_cast<Header>(Header::find_contact|Header::ack);
    }
    queueMessage(reply);
}

void User::writeScheduler()
{
    writeTimer.expires_after(asio::chrono::seconds(1));
    writeTimer.async_wait([this](const asio::error_code& error)
    {
        if(error != asio::error::operation_aborted)
        {
            writer();
        }
    });
}

void User::pingMe()
{
    Stream ping;
    ping.head = Header::ping;
    queueMessage(ping);
    isAlive = false;
}

void User::queueMessage(Stream data)
{
    std::lock_guard<std::mutex> lock(writeQueueMutex);
    writeQueue.push_back(data);
}

void User::reader()
{
    net.receive([this](Stream data, asio::error_code error, std::size_t read)
    {
        if(!error)
            processData(data);
#ifndef NDEBUG
         if(error)
            std::cerr<<name2<<": User::reader()::net.receive(): "<<error.message()<<std::endl;
         std::cerr<<name2<<" received: "<<data.getSerialized()<<std::endl;
#endif
        reader();
    });
}

void User::writer()
{
    if(!writeQueue.empty())
    {
        if(currentQueueIndex >= writeQueue.size())
            currentQueueIndex = 0;

        //here writeQueue is not in any danger of multithreading
        //because write function will execute in only one thread
        //and this is the only function that removes the element from list which can invalidate the iterator.
        auto itr = writeQueue.begin();
        for(int i = 0; i < currentQueueIndex; i++)
            itr++;
        net.send(*itr,[this,itr](asio::error_code error, std::size_t sent)
        {
#ifndef NDEBUG
            std::cerr<<name2<<" sent    : "<<itr->getSerialized()<<std::endl;
#endif            
            if(error)
            {
                if(error == asio::error::operation_aborted)
                    return;
#ifndef NDEBUG
                std::cerr<<name2<<": User::writer()::net.send(): "<<error.message()<<std::endl;
#endif
                currentQueueIndex++;
            }
            else
            {
                std::lock_guard<std::mutex> lock(writeQueueMutex);
                writeQueue.erase(itr);
            }
            writer();
        });
    }
    else
    {
        currentQueueIndex = 0;
        writeScheduler();
    }
}

void User::checkPendingMessages()
{
    std::list<Stream> list;
    list = server.getPendingMessages(name);
    {
        std::lock_guard<std::mutex> lock(writeQueueMutex);
        if ( writeQueue.empty())
            writeQueue.swap(list);
        else
            for(auto& a : list) {
                writeQueue.push_back(a);
            }
    }
}

void User::findGroup(Stream data)
{
    Stream reply;
    reply.head = static_cast<Header>(Header::find_group|Header::error);
    reply.sender = "server";
    reply.receiver = name;
    reply.data1 = data.data1;

    pqxx::result res = server.getGroup(data.data1);
    if(res.size() != 0 && res[0][0].c_str() == data.data1){
        reply.head = static_cast<Header>(Header::find_group|Header::ack);
    }
    queueMessage(reply);
}
