#include <chrono>
#include <map>
#include <iostream>

#include "../src/client/client.hpp"

std::vector<std::unique_ptr<Client>> clients;
std::map<std::string,Stream> sent;
bool failure{false};
int maxClients = 10;
int connectedClients = 0;

bool processData(std::string name, Stream data)
{
    Stream msg = sent[data.sender];
    if(msg.sender != data.sender && msg.receiver != data.receiver && msg.data1 != data.data1)
    {
        return false;
    }
    else
    {
        std::cout<<msg.sender<<" : "<<data.sender<<'\n';
        std::cout<<msg.data1<<" : "<<data.data1<<'\n';
    }
    return true;
}

void writer(Client& c)
{
    int index = rand()%clients.size();
    Stream data;
    data.head = Header::message;
    data.sender = c.name();
    data.receiver = clients[index]->name();
    data.data1 = "test message";
    sent[c.name()] = data;
    c.queueMessage(data);
}

void reader(Client& c)
{
    if(failure)
    {
        c.disconnect();
        return;
    }

    c.reader([&c](Stream data, asio::error_code error, std::size_t read)
    {
        if(error)
        {

        }
        else
            if(data.head == Header::ping)
            {
                c.ping();
                reader(c);
                if(connectedClients == maxClients)
                    writer(c);

            }
            else
                if(!processData(c.name(),data))
                {
                    failure = true;
                }
                else{
//                    c.disconnect();
                }
    });
}

int main()
{
    asio::io_context io;

    srand(time(0));

    for(int i = 0; i < maxClients; i++)
    {
        int nameLength = rand() % 10 + 1;
        std::string name = "";
        for(int j=0; j<nameLength; j++)
        {
            name += ((rand()%26)+97);

        }
        clients.emplace_back(std::make_unique<Client>(io,name));
    }

    for(auto& a:clients)
    {
        std::cout<<a->name()<<'\n';
        a->connect("localhost","1098",[&a](asio::error_code){
            a->start("111",[&a](asio::error_code error, Stream data){
                reader(*a);
                connectedClients++;
            });
        });
    }

    io.run();
    return !failure;
}
