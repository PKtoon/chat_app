#include "../logging.h"
#include "server.h"
#include "user.h"

void User::intro()
{
    inDataSize=20;
    inData.resize(inDataSize);
    boost::asio::async_read(socket,boost::asio::buffer(inData),[this](const boost::system::error_code& error, std::size_t recvd)
    {
        logRead(name,": intro(): received: ", recvd);
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                intro();
            logError(name,": intro(): error: ", error);
        }
        else
        {
            std::string temp{inData.begin(),inData.end()};
            std::istringstream is {temp};
            is>>name;
            logIt(name,": connected");
            readHeader();
        }
    });
}

void User::readHeader()
{
    inHeader.resize(headerLength);
    boost::asio::async_read(socket,boost::asio::buffer(inHeader),[this](const boost::system::error_code& error, std::size_t recvd)
    {
        logRead(name, ": readHeader(): received: ", recvd);
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                readHeader();
            logError(name,": readHeader(): error: ", error);
        }
        else
        {
            std::string temp{inHeader.begin(),inHeader.end()};
            if(temp=="ping")
            {
                alive=true;
                std::string msg {name+": received ping"};
                logIt(name,": received ping");
                readHeader();
            }
            else
            {
                std::istringstream is{temp};
                is>>std::hex>>inDataSize;
                logRead(name,": readHeader(): expected data: ", inDataSize);
                readBody();
            }
        }
    });
}

void User::readBody()
{
    inData.resize(inDataSize);
    boost::asio::async_read(socket,boost::asio::buffer(inData),[this](const boost::system::error_code& error, std::size_t recvd)
    {
        logRead(name, ": readBody(): received: ", recvd);
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                readHeader();
            logError(name,": readBody(): error: ", error);
        }
        else
        {
            std::string temp{inData.begin(),inData.end()};
            writer(Stream(temp));
            logIt(name,": received data");
            readHeader();
        }
    });
}

void User::writer(Stream st)
{
    User* u;
    if(!(u = serv->getUser(st.getReceiver())))
    {
        std::string msg = st.getReceiver()+" not found";
        st = Stream("server", name, msg);
        u=this;
    }

    std::string serialized{st.getSerialized()};
    std::ostringstream header;
    header<<std::setw(headerLength)<<std::hex<<serialized.size();
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(header.str()));
    buffers.push_back(boost::asio::buffer(serialized));

    boost::asio::async_write(u->socket,buffers,[this,u](const boost::system::error_code& error, std::size_t send)
    {
        logWrite(name,": writer(): sent: ", u->getName(), send);
        if (error)
        {
            logError(name,": writer(): error: ", error);
        }
    });
}

void User::pingMe()
{
    std::ostringstream os;
    os<<std::setw(headerLength)<<"ping";
    boost::asio::async_write(socket,boost::asio::buffer(os.str()),[this](const boost::system::error_code& error, std::size_t send)
    {
        logWrite(name,": pingMe(): sent: ", name, send);
        if (error)
        {
            logError(name,": pingMe(): error: ", error);
        }
    });
    alive = false;
}
