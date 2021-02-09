#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include <QAbstractListModel>

#include "../client/client.hpp"
class MessageListModel : public QAbstractListModel
{
    Q_OBJECT
    QVector<QPair<QString,QString>> messageList_;

public:
    Client* client;
    enum MessageRole {
        SenderRole = Qt::UserRole+1,
        MessageRole
    };

    explicit MessageListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void resetList(QString subject);

public slots:
    void resetModel(QString subject);
private:

protected:
    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[SenderRole] = "sender";
        roles[MessageRole] = "message";
        return roles;
    }
};

#endif // MESSAGELISTMODEL_H
