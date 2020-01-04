#ifndef CONTACTLISTITEM_H
#define CONTACTLISTITEM_H

#include <QListWidgetItem>

#include "messageitem.h"

class ContactListItem : public QListWidgetItem
{

public:
    ContactListItem(QListWidget *parent = nullptr);
    ContactListItem(QString text, QListWidget *parent = nullptr);
    void pushMsg(MessageItem message) { msg.push_back(message); }
    
    std::vector<MessageItem> msg;
};

#endif // CONTACTLISTITEM_H
