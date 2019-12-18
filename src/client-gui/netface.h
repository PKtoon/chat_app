#ifndef NETFACE_H
#define NETFACE_H

#include<QObject>
#include <QFuture>

#include "connectionmanager.h"
#include <src/stream/stream.h>

class NetFace : public QObject
{
    Q_OBJECT
    int headerLength = 4;
    std::vector<char> inData;
    boost::asio::io_context io;
    ConnectionManager connMan{io};
    std::thread ioThread;
//     QFuture<void> futuree;
    
signals:
    void connected(const boost::system::error_code result);
    void sentData(std::size_t sent);
    void receivedData(Stream stream);
    void sendError(boost::system::error_code error, std::size_t size);
    void receiveError(boost::system::error_code error, std::size_t size);
    
public:
    NetFace();
    
    void connect(std::string hostname, std::string port);
    void send(Stream data);
    void receive();
};

#endif // NETFACE_H
