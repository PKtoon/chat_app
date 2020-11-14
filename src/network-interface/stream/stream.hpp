#ifndef STREAM_H
#define STREAM_H

#include <vector>
#include <string>

enum Header
{
    empty = 0,
    ack = 1,
    error = 1<<1,
    socket_close = 1<<2,
    ping = 1<<3,
    init = 1<<4,
    message = 1<<5,
    group_message = 1<<6,
    local_file = 1<<7,
    message_out_of_order = 1<<8,
    message_corrupt = 1<<9
};

class Stream
{
    public:
    Header head = Header::empty;
    std::string sender;
    std::string receiver;
    std::string data1;
    unsigned int msgID = 0;
    unsigned int currentPart = 0;
    unsigned int totalParts = 0;
    std::string data2;

    Stream(){}
    Stream(std::string s) { getUnSerialized(s);}
    std::string getSerialized();
    void getUnSerialized(std::string&);
};

#endif // STREAM_H
