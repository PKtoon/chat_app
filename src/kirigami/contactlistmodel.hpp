#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include <qqml.h>

#include "../client/client.hpp"

class ContactListModel : public QAbstractListModel
{
    Q_OBJECT
//    QML_ELEMENT
    QVector<QString> contactList_;

public:
    Client* client_;

    enum ContactRole {
        NameRole = Qt::UserRole+1
    };

    explicit ContactListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void resetList();
    QString getContact(int index){
        return contactList_[index];
    }
    bool findContact(QString name);

public slots:
    void resetModel();

private:

protected:
    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        return roles;
    }

};



#endif // CONTACTLISTMODEL_H
