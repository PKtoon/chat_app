#include <iomanip>

#include "network-interface/netface.hpp"

NetFace::NetFace(asio::io_context& io) : connMan(new ConnectionManager{io}),timer{io}
{
}

NetFace::NetFace(asio::ip::tcp::socket sock) : connMan(new ConnectionManager{std::move(sock)}), timer{connMan->getSocket().get_executor()}
{
}

void NetFace::connect(std::string host, std::string portNum, std::function<void(asio::error_code)> callBack)
{
    std::string hostname = host;
    std::string port = portNum;

    connMan->setEndpoints(hostname,port);
    connMan->connector([callBack](asio::error_code error, asio::ip::tcp::endpoint)
    {
        callBack(error);
    });
}

asio::error_code NetFace::disconnect()
{
    if(!isTimerRunning)
    {
        removeConnMan();
    }
    socketQueue.emplace_back(std::unique_ptr<ConnectionManager>(connMan));
    asio::error_code error = connMan->disconnect();
    connMan = nullptr;
    return error;
}

void NetFace::removeConnMan()
{
    isTimerRunning = true;
    timer.expires_after(asio::chrono::seconds(5));
    timer.async_wait([this](const asio::error_code& error)
    {
        if(error != asio::error::operation_aborted)
        {
            if(!socketQueue.empty())
            {
                socketQueue.pop_front();
                if(!socketQueue.empty())
                    removeConnMan();
                else
                    isTimerRunning = false;
            }
            else
                isTimerRunning = false;
        }
    });
}


void NetFace::newConnection(asio::io_context &io)
{
    if(connMan)
        disconnect();
    connMan = new ConnectionManager{io};
//     connMan->newConnection(io);
}

void NetFace::newConnection(asio::ip::tcp::socket sock)
{
    if(connMan)
        disconnect();
    connMan = new ConnectionManager{std::move (sock)};
//     connMan->newConnection(std::move(sock));
}

asio::ip::tcp::socket * NetFace::getSocket()
{
    if(connMan)
        return &connMan->getSocket();
    return nullptr;
}


void NetFace::send(Stream data, std::function<void(asio::error_code, std::size_t)> callBack)
{
    std::string payload(data.getSerialized());
    payload = "HEAD" + payload + "TAIL";
    uint32_t length = payload.size();
    std::ostringstream header;
    header<<std::setw(headerLength)<<std::hex<<length;
    payload = header.str()+payload;
    std::vector<char> buffer{payload.begin(),payload.end()};
    connMan->writer(buffer,[callBack](asio::error_code error,std::size_t sent)
    {
        callBack(error,sent);
    });
}

void NetFace::receive(std::function<void(Stream, asio::error_code, std::size_t)> callBack)
{
    connMan->reader(headerLength,[this,callBack](std::vector<char> data, asio::error_code error,std::size_t read)
    {
        if (error)
        {
            callBack(Stream(),error,read);
        }
        else
        {
            std::string header(data.begin(),data.end());
            std::stringstream headerStream(header);
            uint32_t dataLength;
            headerStream>>std::hex>>dataLength;
            connMan->reader(dataLength,[callBack](std::vector<char> data, asio::error_code error, std::size_t read)
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
                    else
                    {
                        Stream data;
                        data.head = static_cast<Header>(Header::error|Header::message_corrupt);
                        callBack(data,error,read);
                    }
                }
            });
        }
    });
}
