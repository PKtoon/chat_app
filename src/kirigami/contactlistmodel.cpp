#include "contactlistmodel.hpp"

ContactListModel::ContactListModel(QObject *parent)
{

}

int ContactListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return contactList_.size();
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    switch(role){
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case NameRole:
        return contactList_[index.row()];
        break;
    }
    return QVariant();
}

bool ContactListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool ContactListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

void ContactListModel::resetList()
{
    std::vector<std::string> vlist;

    client_->getContactList(vlist);

    beginResetModel();
    contactList_.clear();
    for(auto& a : vlist){
        contactList_.push_back(a.c_str());
    }
    endResetModel();
}

bool ContactListModel::findContact(QString name)
{
    return contactList_.contains(name);
}

void ContactListModel::resetModel()
{
    resetList();
}
