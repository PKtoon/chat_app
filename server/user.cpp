#include "server.h"
#include "user.h"


void User::intro()
{
    boost::asio::async_read(socket,boost::asio::buffer(readbuff.getdata(),Message::max_length),[this](const boost::system::error_code&, std::size_t)
    {
        readbuff.remakeMsg();
        name=readbuff.getSender();
        std::cout<<name<<" connected"<<std::endl;
        reader();
    });
}

void User::reader()
{
    boost::asio::async_read(socket,boost::asio::buffer(readbuff.getdata(),readbuff.max_length),[this](const boost::system::error_code& error, std::size_t)
    {
        if (!error)
        {
            writebuff.remakeMsg(readbuff.getdata());
            if(writebuff.getReceiver()=="  server" && writebuff.getMsg()=="ack")
                alive=true;
            else
                writer();
            reader();
        }
    });
}

void User::writer()
{
    if(!s->searchUser(writebuff.getReceiver()))
    {
        std::string msg = writebuff.getReceiver()+" not found";
        writebuff.makeMsg("server",name,msg);
    }
    User* u = s->getUser(writebuff.getReceiver());
    boost::asio::async_write(u->socket,boost::asio::buffer(writebuff.getdata(),writebuff.max_length),[](const boost::system::error_code& error, std::size_t)
    {
    });
}

void User::pingMe()
{
    writebuff.makeMsg("server",name,"ping");
    writer();
    alive = false;
}
