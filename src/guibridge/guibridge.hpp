#ifndef GUIBRIDGE_HPP
#define GUIBRIDGE_HPP

#include <QObject>
#include <QtQml>
#include <QtMultimedia/QMediaPlayer>

#include "../client/client.hpp"
#include "contactlistmodel.hpp"
#include "messagelistmodel.hpp"
class GuiBridge : public QObject
{
    Q_OBJECT
#if (QT_VERSION > QT_VERSION_CHECK(5, 12, 10))
    QML_ELEMENT
#endif
    Q_PROPERTY(MessageListModel* messageListModel READ messageListModel NOTIFY messageListModelChanged)
    Q_PROPERTY(ContactListModel* contactListModel READ contactListModel NOTIFY contactListModelChanged)
public:
    explicit GuiBridge(QObject *parent = nullptr);
    ~GuiBridge();

    ContactListModel* contactListModel();
    MessageListModel* messageListModel();

signals:
    void setConnectInformSignal(QString text);
    void setSignInUpInformSignal(QString text);
    void findContactSuccessSignal(QString text);
    void findContactFailureSignal(QString error);
    void findGroupSuccessSignal(QString text);
    void findGroupFailureSignal(QString error);
    void messageReceivedSignal(QString name);
    void resetContactModel();
    void resetMessageModel(QString subject);
    void contactListModelChanged(ContactListModel* contactList);
    void messageListModelChanged(MessageListModel* messageListModel);

private:
    Client client{};
    std::thread ioThread;
    bool isThreadRunning{false};
    QMediaPlayer player;

    ContactListModel contactListModel_;
    MessageListModel messageListModel_;

public slots:
    //client.h
    void connect(QString host, QString port);
    void initSignIn(QString userName, QString passwd);
    void initSignUp(QString userName, QString passwd);
    void findContact(QString contactName);
    void findGroup(QString groupName);
    void reader();
    void writer(int receiverIndex, QString message);
    void processData(Stream data);
    void processMessage(Stream data);
    void currentUser(int index);
    void insertContact(QString name);
    void insertGroup(QString name);
    void createGroup(QString groupName);
    void setCheck(int index, bool state);
};

#endif // GUIBRIDGE_HPP
