#include "client.h"

void Client::connector()
{
    boost::asio::async_connect(socket,endpoints,[this](const boost::system::error_code& error, const tcp::endpoint& endpoint)
    {
        if (!error)
        {
            intro();
            reader();
        }
        else {
            connector();
        }
    });
}

void Client::intro()
{
    writebuff.makeMsg(name,"server","name init");
    boost::asio::write(socket,boost::asio::buffer(writebuff.getdata(),Message::max_length));
}

void Client::writer(std::string receiver, std::string msg)
{
    writebuff.makeMsg(name,receiver,msg);
    boost::asio::async_write(socket,boost::asio::buffer(writebuff.getdata(), writebuff.max_length),[this](const boost::system::error_code& error, std::size_t)
    {
    });
}

void Client::reader()
{
    boost::asio::async_read(socket,boost::asio::buffer(readbuff.getdata(), readbuff.max_length),[this](const boost::system::error_code& error, std::size_t)
    {
        readbuff.remakeMsg();
        if(readbuff.getSender()== "  server" && readbuff.getMsg()=="ping")
        {
            writer("server","ack");
        }
        else
            buff.push_back(readbuff);
        reader();
    });
}

void Client::printer()
{
    while(!buff.empty())
    {
        std::cout<<"\nFrom: "<<buff.begin()->getSender()<<std::endl;
        std::cout<<"Message: "<<buff.begin()->getMsg()<<std::endl<<std::endl;
        buff.pop_front();
    }
}
