#include "client.h"

Client::Client(std::string clientName, std::string hostname, std::string port, asio::io_context& io) : name{clientName}, net{io}
{
    connect(hostname,port);
}

void Client::connect(std::string hostname, std::string port)
{
    net.connect(hostname,port,[this,hostname,port](asio::error_code error)
    {
        if(error)
        {
            if(error != asio::error::operation_aborted)
                connect(hostname,port);
        }
        else
            initialize();
    }
    );
}

void Client::initialize()
{
    Stream initPack;
    initPack.head = Header::INIT;
    initPack.sender = name;
    net.send(initPack,[this](asio::error_code error, std::size_t sent)
    {
        if(error)
        {
            if(error !=asio::error::operation_aborted)
                initialize();
        }
        else
        {
            net.receive([this](Stream initAck, asio::error_code error, std::size_t read)
            {
                if(error)
                {
                    if(error !=asio::error::operation_aborted)
                        initialize();
                }
                else
                {
                    if(initAck.head ==(Header) (Header::INIT | Header::ACK))
                        reader();
                    else
                        initialize();
                }
            }
            );
        }
    }
    );
}

void Client::processData(Stream data)
{
    switch(data.head)
    {
        case Header::PING:
            ping();
            break;
        default:
            break;
    }
}

void Client::queueMessage(Stream data)
{
    writeQueue.push_back(data);
    if(!isWriting)
        writer();
}

void Client::ping()
{
    Stream ping;
    ping.head = Header::PING;
    queueMessage(ping);
}

void Client::reader()
{
    net.receive([this](Stream data, asio::error_code error, std::size_t read)
    {
        if(error)
        {
            if(error !=asio::error::operation_aborted)
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

void Client::writer()
{
    isWriting = true;
    if(!writeQueue.empty())
    {
        net.send(*writeQueue.begin(),[this](asio::error_code error, std::size_t sent)
        {
            if(error)
            {
                if(error != asio::error::operation_aborted)
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
