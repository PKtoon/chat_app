#ifndef STREAM_H
#define STREAM_H

#include <vector>
#include <string>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/asio.hpp>

enum {MESSAGE, LOCAL_FILE};

class Stream
{
    friend class boost::serialization::access;
    char header='m';
    //char head;
    std::string sender;
    std::string receiver;
    std::string data1;
    std::vector<char> data2;
    
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar & header;
        ar & sender;
        ar & receiver;
        ar & data1;
        if(header!='m')
        {
            ar & data2;
        }
    }
    
public:
    Stream(){}
    Stream(std::string s) { getUnSerialized(s);}
    Stream(std::string s, std::string r, std::string msg): sender{s}, receiver{r},data1{msg}{}
    std::string getSender() { return sender; }
    std::string getReceiver() { return receiver;}
    std::string getData1() { return data1; }
    std::string getSerialized();
    void getUnSerialized(std::string&);
};

#endif // STREAM_H