#ifndef STREAM_H
#define STREAM_H

#include <vector>
#include <string>

enum Header
{
    EMPTY = 0,
    ACK = 1,
    ERROR = 1<<1,
    SOCKET_CLOSE = 1<<2,
    PING = 1<<3,
    INIT = 1<<4,
    MESSAGE = 1<<5,
    GROUP_MESSAGE = 1<<6,
    LOCAL_FILE = 1<<7,
    MESSAGE_OUT_OF_ORDER = 1<<8
};

class Stream
{
    public:
    Header head = Header::EMPTY;
    std::string sender;
    std::string receiver;
    std::string data1;
    unsigned int msgID;
    unsigned int currentPart;
    unsigned int totalParts;
    std::vector<char> data2;

    Stream(){}
    Stream(std::string s) { getUnSerialized(s);}
    Stream(std::string s, std::string r, std::string msg): sender{std::move(s)}, receiver{std::move(r)}, data1{std::move(msg)}{}
    std::string getSerialized();
    void getUnSerialized(std::string&);
};

#endif // STREAM_H
