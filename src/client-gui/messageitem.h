#ifndef MESSAGEITEM_H
#define MESSAGEITEM_H

#include <string>

class MessageItem
{
public:
    MessageItem(std::string frm, std::string msg):from{frm},message{msg}{}
    
    std::string from;
    std::string message;

};

#endif // MESSAGEITEM_H
