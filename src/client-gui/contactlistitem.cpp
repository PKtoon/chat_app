#include "contactlistitem.h"

ContactListItem::ContactListItem(QListWidget* parent) : QListWidgetItem(parent,QListWidgetItem::UserType)
{
}

ContactListItem::ContactListItem(QString text,QListWidget* parent) : QListWidgetItem(parent,QListWidgetItem::UserType)
{
    setText(text);
}
