#include <iostream>
#include <sstream>
#include "client.h"

void Client::connector()
{
    boost::asio::async_connect(socket,endpoints,[this](const boost::system::error_code& error, const tcp::endpoint&)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                connector();
        }
        else {
            intro();
        }
    });
}

void Client::intro()
{
    std::ostringstream os;
    os<<std::setw(20)<<name;
    std::string out{os.str()};

    boost::asio::async_write(socket,boost::asio::buffer(out),[this](const boost::system::error_code& error, std::size_t){
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                intro();
        }
        else
            readHeader();
    });
}

void Client::writer(Stream outData)
{
    std::string serialized{outData.getSerialized()};
    unsigned long length = serialized.size();
    std::ostringstream header;
    header<<std::setw(headerLength)<<std::hex<<length;
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(header.str()));
    buffers.push_back(boost::asio::buffer(serialized));

    boost::asio::async_write(socket,buffers,[](const boost::system::error_code&, std::size_t){});
}

void Client::readHeader()
{
    inHeader.resize(headerLength);
    boost::asio::async_read(socket,boost::asio::buffer(inHeader),[this](const boost::system::error_code& error, std::size_t)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                readHeader();
        }
        else
        {
            std::string temp{inHeader.begin(),inHeader.end()};
            if(temp=="ping")
            {
                std::ostringstream os;
                os<<std::setw(headerLength)<<"ping";
                boost::asio::async_write(socket,boost::asio::buffer(os.str()),[](const boost::system::error_code&, std::size_t){});
                readHeader();
            }
            else
            {
                std::istringstream is{temp};
                is>>std::hex>>inDataSize;
                readBody();
            }
        }
    });
}

void Client::readBody()
{
    inData.resize(inDataSize);
    boost::asio::async_read(socket,boost::asio::buffer(inData),[this](const boost::system::error_code& error, std::size_t)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
            readHeader();
        }
        else
        {
            std::string temp{inData.begin(),inData.end()};
            buff.push_back(Stream(temp));
            readHeader();
        }
    });
}

void Client::printer()
{
    while(!buff.empty())
    {
        std::cout<<std::endl<<"\""<<buff.begin()->getSender()<<" : "<<buff.begin()->getData1()<<"\""<<std::endl;
        buff.pop_front();
    }
}
