#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include <stddef.h>
#include <string>
#include <cstring>
#include <boost/lexical_cast.hpp>

class Message
{
public:
    static constexpr std::size_t max_length=512, msg_length=496,name_length=8;
    char data[max_length], msg[msg_length], sender[name_length],receiver[name_length];
    Message(){}
    Message(char *name)
    {
        std::memcpy(sender,name,name_length);
    }
    void makeMsg(char*,char*);
    void remakeMsg();
    void remakeMsg(char*);
    char* getdata(){return data; }
    void print() { std::cout<<"sender:: "<<sender<<std::endl<<"receiver:: "<<receiver<<std::endl<<"msg:: "<<msg<<std::endl<<"data:: "<<data<<std::endl; }
};

//void Message::makeMsg(char* s)
//{
//    std::strcat(msg,s.c_str());
//    length=s.size();
//    std::sprintf(header, "%4d", length);
//    std::strcpy(data,header);
//    std::strcat(data,msg);
////    sprintf(data,"%s%s",header,msg);
//     std::cout<<"make msg header::"<<s<<"msg::"<<msg<<std::endl;
//}

void Message::makeMsg(char* rec,char* s)
{
    std::memcpy(msg,s,msg_length);
    std::memcpy(receiver,rec,name_length);
    std::sprintf(data,"%8s%8s%s",sender,receiver,msg);
}

void Message::remakeMsg()
{
}

void Message::remakeMsg(char *data2)
{
    std::memcpy(data,data2,max_length);
    remakeMsg();
//    int i=0;
//    for(;i<8;i++)
//        sender[i]=data[i];
//    for(i=8;i<16;i++)
//        receiver[i-8]=data[i];
//    for(i=16;i<max_length;i++)
//        msg[i-16]=data[i];
}

void pkprint(char *p)
{
    std::cout<<p<<std::endl;
}

#endif // MESSAGE_H
