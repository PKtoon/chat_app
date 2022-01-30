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

    QVector<std::tuple<QString,int,bool>> contactList_;

public:
    Client* client_;

    enum ContactRole {
        NameRole = Qt::UserRole,
        TypeRole,
        CheckRole
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
        return std::get<0>(contactList_[index]);
    }
    int getType(int index) {
        return std::get<1>(contactList_[index]);
    }
    bool check(int index)
    {
        return std::get<2>(contactList_[index]);
    }
    void check(int index, bool state)
    {
        std::get<2>(contactList_[index]) = state;
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
        roles[CheckRole] = "check";
        return roles;
    }

};



#endif // CONTACTLISTMODEL_H
