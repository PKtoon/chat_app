#include "server.h"
#include "user.h"

User::User(boost::asio::ip::tcp::socket socket, Server& serv) : net{std::move(socket)}, server{serv}
{
    timer.async_wait([this](const boost::system::error_code& error)
    {
       checkPulse();
    });
    initialize();
}

void User::initialize()
{
    net.receive([this](Stream data, boost::system::error_code error, std::size_t read)
    {
        if(error)
        {
            if(error != boost::asio::error::operation_aborted)
                initialize();
        }
        else
        {
            if(data.head == Header::INIT)
            {
                if(!server.getUser(data.sender))
                {
                    isAlive=true;
                    name = data.sender;                    
                    Stream reply;
                    reply.head = (Header)(Header::INIT | Header::ACK);
                    queueMessage(reply);
                    reader();
                }
            }
            else
                initialize();
        }
    });
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
    timer.expires_after(boost::asio::chrono::seconds(20));
    timer.async_wait([this](const boost::system::error_code& error)
    {
       checkPulse();
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
    net.receive([this](Stream data, boost::system::error_code error, std::size_t read)
    {
        if(error)
        {
            if(error != boost::asio::error::operation_aborted)
                reader();
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
        net.send(*writeQueue.begin(),[this](boost::system::error_code error, std::size_t sent)
        {
            if(error)
            {
                if(error != boost::asio::error::operation_aborted)
                    writer();
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
