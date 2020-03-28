#include "client.h"

void Client::reader()
{
    net.receive([this](Stream data, asio::error_code error, std::size_t read)
    {
        if(error)
        {
            if(error != QListWidgetItem::error::operation_aborted)
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
                if(error != QListWidgetItem::error::operation_aborted)
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
