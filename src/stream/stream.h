#ifndef STREAM_H
#define STREAM_H

#include <vector>
#include <string>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/asio.hpp>

enum Header
{
    EMPTY = 0,
    ACK = 1,
    ERROR = 1<<1,
    CLOSE = 1<<2,
    INIT = 1<<3,
    PING = 1<<4,
    MESSAGE = 1<<5,
    GROUP = 1<<6,
    LOCAL_FILE = 1<<7
};

class Stream
{
    friend class boost::serialization::access;
public:
    Header head = Header::EMPTY;
    std::string sender;
    std::string receiver;
    std::string data1;
    unsigned int msgID;
    unsigned int currentPart;
    unsigned int totalParts;
    std::vector<char> data2;
    
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar & head;
        switch(head)
        {
            case Header::INIT:
                ar & sender;
                break;
            case Header::INIT | Header::ACK:
                break;
            default:
                break;
        }
//         ar & head;
//         ar & sender;
//         ar & receiver;
//         ar & data1;
    }

    Stream(){}
    Stream(std::string s) { getUnSerialized(s);}
    Stream(std::string s, std::string r, std::string msg): sender{std::move(s)}, receiver{std::move(r)}, data1{std::move(msg)}{}
    std::string getSender() { return sender; }
    std::string getReceiver() { return receiver;}
    std::string getData1() { return data1; }
    std::string getSerialized();
    void getUnSerialized(std::string&);
};

#endif // STREAM_H
