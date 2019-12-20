#include "../logging.h"
#include "server.h"
#include "user.h"

User::User(boost::asio::ip::tcp::socket socket, Server* serv) : net{std::move(socket)}, server{serv}
{
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
                name = data.sender;
                server->addMe(this);
                Stream reply;
                reply.head = (Header)(Header::INIT | Header::ACK);
                queueMessage(reply);
                reader();
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
            server->queueDelivery(data);
            break;
        case Header::PING:
            isAlive = true;
            break;
        default:
            break;
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
