#ifndef GUIBRIDGE_HPP
#define GUIBRIDGE_HPP

#include <QObject>
#include <qqml.h>

#include "../client/client.hpp"
#include "contactlistmodel.hpp"
#include "messagelistmodel.hpp"
class GuiBridge : public QObject
{
    Q_OBJECT
#if (QT_VERSION > QT_VERSION_CHECK(5, 12, 10))
    QML_ELEMENT
#endif
    Q_PROPERTY(MessageListModel* messageListModel READ messageListModel WRITE setMessageListModel NOTIFY messageListModelChanged)
    Q_PROPERTY(ContactListModel* contactListModel READ contactListModel WRITE setContactListModel NOTIFY contactListModelChanged)
public:
    explicit GuiBridge(QObject *parent = nullptr);
    ~GuiBridge(){
        if(ioThread.joinable()){
            client.disconnect();
            ioThread.join();
        }
    }

    ContactListModel* contactListModel() const
    {
        return contactListModel_;
    }

    MessageListModel* messageListModel() const
    {
        return messageListModel_;
    }

signals:
    void setConnectInformSignal(QString text);
    void setSignInUpInformSignal(QString text);
    void findContactSuccessSignal();
    void findContactFailureSignal(QString error);
    void messageReceivedSignal(QString name);
    void resetContactModel();
    void resetMessageModel(QString subject);
    void contactListModelChanged(ContactListModel* contactList);

    void messageListModelChanged(MessageListModel* messageListModel);

private:
    Client client{};
    std::thread ioThread;
    bool isThreadRunning{false};
    QString contactName;

    ContactListModel* contactListModel_;

    MessageListModel* messageListModel_;

public slots:
    //client.h
    void connect(QString host, QString port);
    void initSignIn(QString userName, QString passwd);
    void initSignUp(QString userName, QString passwd);
    void findContact(QString contactName);
    void reader();
    void writer(int receiver, QString message);
    void processData(Stream data);
    void processMessage(Stream data);
    void currentUser(int index);
    void insertContact(QString name);

    //gui
    void setContactListModel(ContactListModel* contactListModel);
    void setMessageListModel(MessageListModel* messageListModel)
    {
        if (messageListModel_ == messageListModel)
            return;

        messageListModel_ = messageListModel;
        messageListModel_->client = &client;
        QObject::connect(this,&GuiBridge::resetMessageModel, messageListModel_, &MessageListModel::resetModel,Qt::QueuedConnection);
        emit messageListModelChanged(messageListModel_);
    }
};

#endif // GUIBRIDGE_HPP
