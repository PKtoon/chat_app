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
        //TODO: if there are any active user with same name then first disconnect them
        //TODO: after five counts disconnect

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
        }
            break;
        default:
            break;
        }
    }
    queueMessage(reply);
}

void User::processData(Stream data)
{
    switch(data.head)
    {
        case Header::message:
            server.queueDelivery(data);
            break;
        case Header::ping:
            isAlive = true;
            break;
        case Header::socket_close:
            isAlive = false;
            name = "";
//             net.disconnect();
            break;
        case Header::signin:
        case Header::signup:
            authHandler(data);
            break;
        default:
            break;
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
        if(error)
        {
            if(error != asio::error::operation_aborted)
            {
                std::cerr<<name2<<": User::reader()::net.receive(): "<<error.message()<<std::endl;
                reader();
            }
        }
        else
        {
            reader();
            processData(data);
        }
    }
    );
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
            if(error == asio::error::operation_aborted)
                return;

            if(error)
            {
                std::cerr<<name2<<": User::writer()::net.send(): "<<error.message()<<std::endl;
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
