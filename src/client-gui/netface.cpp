#include "netface.h"
#include <iostream>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
// #include "../stream/stream.h"

NetFace::NetFace()
{
    
}

void NetFace::connect(std::string hostname, std::string port)
{
    connMan.setEndpoints(hostname,port);
    connMan.connector([this](boost::system::error_code error)
    {
        emit connected(error);
    });
    ioThread = std::thread([this](){ io.run(); });
//     futuree = QtConcurrent::run([this](){ io.run(); });
}

void NetFace::send(Stream data)
{
    std::string payload(data.getSerialized());
    unsigned long length = payload.size();
    std::ostringstream header;
    header<<std::setw(headerLength)<<std::hex<<length;
    connMan.writer(header.str(),[this,payload](boost::system::error_code error,std::size_t sent)
    {
        if(error)
        {
            if(error != boost::asio::error::operation_aborted)
                emit sendError(error,sent);
        }
        else
        {
            connMan.writer(payload,[this](boost::system::error_code error, std::size_t sent)
            {
                if(error)
                {
                    if(error != boost::asio::error::operation_aborted)
                        emit sendError(error,sent);
                }
                else
                {
                    emit sentData(sent);
                }
            });
        }
    });
}

void NetFace::receive()
{
    connMan.reader(headerLength,[this](std::vector<char>& data, boost::system::error_code error,std::size_t read)
    {
        if (error)
        {
            if (error != boost::asio::error::operation_aborted)
                emit receiveError(error,read);
        }
        else
        {
            std::string header(data.begin(),data.end());
            std::stringstream headerStream(header);
            unsigned long dataLength;
            headerStream>>dataLength;
            connMan.reader(dataLength,[this](std::vector<char>& data, boost::system::error_code error, std::size_t read)
            {
                if(error)
                {
                    if (error != boost::asio::error::operation_aborted)
                        emit receiveError(error,read);
                }
                else
                {
                    inData = data;
                    emit receivedData(Stream(std::string(inData.begin(),inData.end())));
                }
            });
        }
    });
}
