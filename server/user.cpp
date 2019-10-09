#include "../logging.h"
#include "server.h"
#include "user.h"

void User::intro()
{
    inDataSize=20;
    inData.resize(inDataSize);
    //    boost::asio::async_read(socket,boost::asio::buffer(inData),[this](const boost::system::error_code& error, std::size_t recvd)
    socket.async_receive(boost::asio::buffer(inData),[this](const boost::system::error_code& error, std::size_t recvd)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
            {
                intro();
                logError(name,": intro(): error: ", error);
            }
        }
        else
        {
            logRead(name,": intro(): received: ", recvd);
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
    inHeader.clear();
    inHeader.resize(headerLength);
    socket.async_receive(boost::asio::buffer(inHeader),[this](const boost::system::error_code& error, std::size_t recvd)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
            {
                readHeader();
                logError(name,": readHeader(): error: ", error);
            }
        }
        else
        {
            logRead(name, ": readHeader(): received: ", recvd);
            if(!inHeader.empty())
            {
                std::string temp{inHeader.begin(),inHeader.end()};
                if(temp=="ping")
                {
                    alive=true;
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
        }
    });
}

void User::readBody()
{
    inData.clear();
    inData.resize(inDataSize);
    socket.async_receive(boost::asio::buffer(inData),[this](const boost::system::error_code& error, std::size_t recvd)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
            {
                readHeader();
                logError(name,": readBody(): error: ", error);
            }
        }
        else
        {
            logRead(name, ": readBody(): received: ", recvd);
            if(!inData.empty())
            {
                std::string temp{inData.begin(),inData.end()};
                writer(Stream(temp));
            }
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
    u->socket.async_send(buffers,[this,u](const boost::system::error_code& error, std::size_t send)
//    boost::asio::async_write(u->socket,buffers,[this,u](const boost::system::error_code& error, std::size_t send)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                logError(name,": writer(): error: ", error);
        }
        else {
            logWrite(name,": writer(): sent: ", u->getName(), send);
        }
    });
}

void User::pingMe()
{
    alive = false;
    std::ostringstream os;
    os<<std::setw(headerLength)<<"ping";
    socket.async_send(boost::asio::buffer(os.str()),[this](const boost::system::error_code& error, std::size_t send)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                logError(name,": pingMe(): error: ", error);
        }
        else {
            logWrite(name,": pingMe(): sent: ", name, send);
        }
    });
}
