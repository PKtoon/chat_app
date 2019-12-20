#include "netface.h"
#include <iostream>

NetFace::NetFace(boost::asio::io_context& io) : connMan{io} {}

NetFace::NetFace(tcp::socket sock) : connMan{std::move(sock)} {}

NetFace::NetFace(boost::asio::io_context& io, std::string host, std::string portNum) : hostname{host}, port{portNum}, connMan{io} {}


void NetFace::connect(std::string host, std::string portNum, std::function<void(boost::system::error_code)> callBack)
{
    hostname = host;
    port = portNum;
    connMan.setEndpoints(hostname,port);
    connMan.connector([this,callBack](boost::system::error_code error)
    {
        callBack(error);
    });
}

void NetFace::connect(std::function<void(boost::system::error_code)> callBack)
{
    connMan.setEndpoints(hostname,port);
    connMan.connector([this,callBack](boost::system::error_code error)
    {
        callBack(error);
    });
}

void NetFace::disconnect()
{
    connMan.getSocket().close();
}


void NetFace::send(Stream data, std::function<void(boost::system::error_code, std::size_t)> callBack)
{
    std::string payload(data.getSerialized());
    unsigned long length = payload.size();
    std::ostringstream header;
    header<<std::setw(headerLength)<<std::hex<<length;
    connMan.writer((header.str()+payload),[this,payload,callBack](boost::system::error_code error,std::size_t sent)
    {
        if(error)
        {
                callBack(error,sent);
        }
        else
        {
                callBack(error,sent);
        }
    });
}

void NetFace::receive(std::function<void(Stream, boost::system::error_code, std::size_t)> callBack)
{
    connMan.reader(headerLength,[this,callBack](std::vector<char> data, boost::system::error_code error,std::size_t read)
    {
        if (error)
        {
            callBack(Stream(),error,read);
        }
        else
        {
            std::string header(data.begin(),data.end());
            std::stringstream headerStream(header);
            unsigned int dataLength;
            headerStream>>dataLength;
            connMan.reader(dataLength,[callBack](std::vector<char> data, boost::system::error_code error, std::size_t read)
            {
                if(error)
                {
                    callBack(Stream(),error,read);
                }
                else
                {
                    callBack(Stream(std::string(data.begin(),data.end())),error,read);
                }
            });
        }
    });
}

// void NetFace::receiveBody(unsigned int length, std::function<void (Stream, boost::system::error_code, std::size_t)> callBack)
// {
//     connMan.reader(length,[this,callBack](std::vector<char> data, boost::system::error_code error,std::size_t read)
//     {
//         if (error)
//         {
//             callBack(Stream(),error,read);
//         }
//         else
//         {
// //             std::cerr<<"netface receive data "<<data.size()<<std::endl;
// //             std::string header(data.begin(),data.end());
// //             std::stringstream headerStream(header);
// //             unsigned int dataLength;
// //             headerStream>>dataLength;
// //             std::cerr<<"netface receive header init"<<std::endl;
//             callBack(Stream(std::string(data.begin(),data.end())),error,read);
//         }
//     });
// }


void NetFace::runIOThread()
{
    ioThread = std::thread([this](){ io.run(); });
}

void NetFace::stopIOThread()
{
    ioThread.join();
}
