#ifndef CONTACTLISTITEM_H
#define CONTACTLISTITEM_H

#include <QListWidgetItem>

class ContactListItem : public QListWidgetItem
{

public:
    ContactListItem(QListWidget *parent = nullptr);
    ContactListItem(QString text, QListWidget *parent = nullptr);
};

#endif // CONTACTLISTITEM_H
