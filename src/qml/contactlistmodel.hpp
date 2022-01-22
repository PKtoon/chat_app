#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include <qqml.h>

#include "../client/client.hpp"

class ContactListModel : public QAbstractListModel
{
    Q_OBJECT
#if (QT_VERSION > QT_VERSION_CHECK(5, 12, 10))
    QML_ELEMENT
#endif

    QVector<QPair<QString,int>> contactList_;

public:
    Client* client_;

    enum ContactRole {
        NameRole = Qt::UserRole,
        TypeRole
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
        return contactList_[index].first;
    }
    int getType(int index) {
        return contactList_[index].second;
    }
    bool findContact(QString name);

public slots:
    void resetModel();

private:

protected:
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[TypeRole] = "type";
        return roles;
    }

};



#endif // CONTACTLISTMODEL_H
