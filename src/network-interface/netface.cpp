#include <iomanip>

#include "netface.h"

NetFace::NetFace(boost::asio::io_context& io) : connMan{io} {}

NetFace::NetFace(tcp::socket sock) : connMan{std::move(sock)} {}

void NetFace::connect(std::string host, std::string portNum, std::function<void(boost::system::error_code)> callBack)
{
    std::string hostname = host;
    std::string port = portNum;

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
    payload = "HEAD" + payload + "TAIL";
    unsigned long length = payload.size();
    std::ostringstream header;
    header<<std::setw(headerLength)<<std::hex<<length;
    payload = header.str()+payload;
    std::vector<char> buffer{payload.begin(),payload.end()};
    connMan.writer(buffer,[callBack](boost::system::error_code error,std::size_t sent)
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
            headerStream>>std::hex>>dataLength;
            connMan.reader(dataLength,[callBack](std::vector<char> data, boost::system::error_code error, std::size_t read)
            {
                if(error)
                {
                    callBack(Stream(),error,read);
                }
                else
                {
                    std::string temp {data.begin(),data.end()};
                    if(temp.substr(0,4)=="HEAD" && temp.substr(temp.size()-4,4) == "TAIL")
                        callBack(Stream(temp.substr(4,temp.size()-8)),error,read);
                }
            });
        }
    });
}
