#include "messagelistmodel.hpp"

MessageListModel::MessageListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int MessageListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return messageList_.size();
}

QVariant MessageListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    switch(role){
    case SenderRole:
        return messageList_[index.row()].first;
    case MessageRole:
        return messageList_[index.row()].second;
    }

    return QVariant();
}

bool MessageListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool MessageListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

void MessageListModel::resetList(QString subject)
{
    std::vector<std::pair<std::string,std::string>> list;
    client->getMessages(subject.toStdString(),list);

    beginResetModel();
    messageList_.clear();
    for(auto& a : list){
        messageList_.push_back({a.first.c_str(),a.second.c_str()});
    }
    endResetModel();
}

void MessageListModel::resetModel(QString subject)
{
    resetList(subject);
}
