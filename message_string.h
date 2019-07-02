#ifndef MESSAGE_STRING_H
#define MESSAGE_STRING_H

#include <iostream>
#include <stddef.h>
#include <string>
#include <cstring>
#include <boost/lexical_cast.hpp>

class Message
{
    std::string msg, sender,receiver;
    char data[512];
public:
    static constexpr std::size_t max_length=512, msg_length=496,name_length=8;
    Message(){}
    Message(std::string name)
    {
        sender=name;
    }
    void makeMsg(std::string,std::string);
    void makeMsg(std::string,std::string,std::string);
    void remakeMsg();
    void remakeMsg(const char*);
    std::string getSender() { return sender; }
    std::string getReceiver() { return receiver; }
    std::string getMsg() { return msg; }
    char* getdata() { return data; }
    void print() { std::cout<<"sender:: "<<sender<<std::endl<<"receiver:: "<<receiver<<std::endl<<"msg:: "<<msg<<std::endl<<"data:: "<<data<<std::endl; }
};

void Message::makeMsg(std::string rec, std::string mes)
{
    receiver=rec;
    msg=mes;
    sprintf(data,"%8s%8s%s",sender.c_str(),receiver.c_str(),msg.c_str());
}

void Message::makeMsg(std::string sen,std::string rec, std::string mes)
{
    sender=sen;
    receiver=rec;
    msg=mes;
    sprintf(data,"%8s%8s%s",sender.c_str(),receiver.c_str(),msg.c_str());
}

void Message::remakeMsg()
{
    std::string temp{data};
    sender.assign(temp,0,8);
    receiver.assign(temp,8,8);
    msg=temp.substr(16,temp.size());
}

void Message::remakeMsg(const char *s)
{
    memcpy(data,s,max_length);
    remakeMsg();
}

#endif // MESSAGE_STRING_H
