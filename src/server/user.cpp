#include "server.h"
#include "user.h"

User::User(asio::ip::tcp::socket socket, Server& serv) : net{std::move(socket)}, server{serv}
{
    timer.async_wait([this](const asio::error_code& error)
    {
        if(error != asio::error::operation_aborted)
            checkPulse();
    });
    initialize();
}

void User::initialize()
{
    if(count < 5)
    {
        count++;
        net.receive([this](Stream data, asio::error_code error, std::size_t read)
        {
            Stream reply;
            reply.head = static_cast<Header>(Header::INIT|Header::ERROR);
            reply.sender = "server";
            if(error)
            {
                if(error != asio::error::operation_aborted)
                {
                    std::cerr<<"User::initialize()::net.receive(): "<<error.message()<<std::endl;
                    initialize();
                }
            }
            else
            {
                if(data.head == Header::INIT)
                {
                    isAlive = true;
                    pqxx::result res = server.getUser(data.sender);
                    switch (res.size())
                    {
                    case 1:
                        if(server.authUser(data.sender,data.data1))
                        {
                            name = data.sender;
                            reply.receiver = name;
                            reply.head = static_cast<Header>(Header::INIT|Header::ACK);
                            reader();
                        }
                        break;
                    case 0:
                        server.addUser(data.sender,data.data1);
                        name = data.sender;
                        reply.receiver = name;
                        reply.head = static_cast<Header>(Header::INIT|Header::ACK);
                        reader();
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    if(data.head == SOCKET_CLOSE)
                        net.disconnect();
                    else
                    {
                        reply.head = static_cast<Header>(Header::ERROR);
                        reply.receiver = data.sender;
                        initialize();
                    }
                }
                queueMessage(reply);
            }
        });
    }
}

void User::processData(Stream data)
{
    switch(data.head)
    {
        case Header::MESSAGE:
            server.queueDelivery(data);
            break;
        case Header::PING:
            isAlive = true;
            break;
        case Header::SOCKET_CLOSE:
            isAlive = false;
            name = "";
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
        timer.expires_after(asio::chrono::seconds(20));
        timer.async_wait([this](const asio::error_code& error)
        {
            if(error != asio::error::operation_aborted)
            {
                std::cerr<<name<<": User::checkPulse()::timer.async_wait(): "<<error.message()<<std::endl;
                checkPulse();
            }
        });
    }
}

void User::pingMe()
{
    Stream ping;
    ping.head = Header::PING;
    queueMessage(ping);
    isAlive = false;
}

void User::queueMessage(Stream data)
{
    writeQueue.push_back(data);
    if(!isWriting)
        writer();
}

void User::reader()
{
    net.receive([this](Stream data, asio::error_code error, std::size_t read)
    {
        if(error)
        {
            if(error != asio::error::operation_aborted)
            {
                std::cerr<<name<<": User::reader()::net.receive(): "<<error.message()<<std::endl;
                reader();
            }
        }
        else
        {
            processData(data);
            reader();
        }
    }
    );
}

//todo:
//writer if unable to write due to error will repeat indefinitely and other messages will stall rather than sending others
//one thing I can do is make a pending buffer which will store failed messages. then whenever next invocation of writer occurs all pending will be appended to writeQueue
//well appending at front can make writer face same error over and over again, and appending at end will mess up message order unless a sequence number for message is used
//multiple failure can result in long wait time for failed message and beyond that removal of message
//todo end

void User::writer()
{
    isWriting = true;
    if(!writeQueue.empty())
    {
        net.send(*writeQueue.begin(),[this](asio::error_code error, std::size_t sent)
        {
            if(error)
            {
                if(error != asio::error::operation_aborted)
                {
                    std::cerr<<name<<": User::writer()::net.send(): "<<error.message()<<std::endl;
                    writer();
                }
            }
            else
            {
                writeQueue.pop_front();
                writer();
            }
        });
    }
    else
        isWriting = false;
}
