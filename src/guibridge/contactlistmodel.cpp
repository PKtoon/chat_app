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
        return std::get<0>(contactList_[index.row()]);
    case TypeRole:
        return std::get<1>(contactList_[index.row()]);
    case CheckRole:
        return std::get<2>(contactList_[index.row()]);
    }
    return QVariant();
}

bool ContactListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool ContactListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

void ContactListModel::resetList()
{
    std::vector<std::pair<std::string,int>> vlist;

    client_->getContactList(vlist);

    beginResetModel();
    contactList_.clear();
    for(auto& a : vlist){
        contactList_.push_back({a.first.c_str(),a.second,false});
    }
    endResetModel();
}

bool ContactListModel::findContact(QString name)
{
    for(auto& a : contactList_) {
        if(std::get<0>(a) == name)
            return true;
    }
    return false;
}

void ContactListModel::resetModel()
{
    resetList();
}
