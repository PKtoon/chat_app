#include "guibridge.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

GuiBridge::GuiBridge(QObject *parent) : QObject(parent)
{
    QObject::connect(this,&GuiBridge::resetContactModel, &contactListModel_, &ContactListModel::resetModel,Qt::QueuedConnection);
    contactListModel_.client_ = &client;
    QObject::connect(this,&GuiBridge::resetMessageModel, &messageListModel_, &MessageListModel::resetModel,Qt::QueuedConnection);
    messageListModel_.client = &client;
    player.setMedia(QUrl("qrc:/resources/assets/Done_for_You/done-for-you-612.ogg"));
}

GuiBridge::~GuiBridge()
{
    if(ioThread.joinable()){
        client.disconnect();
        ioThread.join();
    }
}

ContactListModel *GuiBridge::contactListModel()
{
    return &contactListModel_;
}

MessageListModel *GuiBridge::messageListModel()
{
    return &messageListModel_;
}

void GuiBridge::initSignIn(QString userName, QString passwd)
{
    client.userAuthInit(userName.toStdString(),passwd.toStdString(),Header::signin);
}

void GuiBridge::initSignUp(QString userName, QString passwd)
{
    client.userAuthInit(userName.toStdString(),passwd.toStdString(),Header::signup);
}

void GuiBridge::findContact(QString contactName)
{
    if(!client.getSocket()->is_open() || client.name().empty())
    {
//        newContactDialog->setInform("Client is not connected");
        return;
    }
    client.findEntity(contactName.toStdString(), Header::find_contact);
}

void GuiBridge::findGroup(QString groupName)
{
    if(!client.getSocket()->is_open() || client.name().empty())
    {
//        newContactDialog->setInform("Client is not connected");
        return;
    }
    client.findEntity(groupName.toStdString(),Header::find_group);
}

void GuiBridge::reader()
{
    client.reader([this](Stream data, asio::error_code error, std::size_t read)
    {
#ifndef NDEBUG
        std::cerr<<"received: "<<data.getSerialized()<<std::endl;
#endif
        if(error)
        {

        }
        else
            processData(data);
        reader();
    });
}

void GuiBridge::writer(int receiverIndex, QString message)
{
    Stream data;
    data.sender = client.name();
    data.receiver = contactListModel_.getContact(receiverIndex).toStdString();
    if(contactListModel_.getType(receiverIndex) == Client::ContactType::group)
        data.head = Header::group_message;
    else
        data.head = Header::message;
    data.data1 = message.toStdString();
    client.queueMessage(data);
#ifndef NDEBUG
    std::cerr<<"queued  : "<<data.getSerialized()<<std::endl;
#endif
    emit resetMessageModel(data.receiver.c_str());
}

void GuiBridge::processData(Stream data)
{
    switch(data.head)
    {
        case Header::message:
            emit resetContactModel();
            processMessage(data);
            emit messageReceivedSignal(data.sender.c_str());
            player.play();
            break;
        case Header::group_message:
            emit resetContactModel();
            processMessage(data);
            emit messageReceivedSignal(data.sender.c_str());
            player.play();
            break;
        case Header::signin|Header::ack:
        case Header::signup|Header::ack:
            emit setSignInUpInformSignal(QString("Successfully authenticated"));
            emit resetContactModel();
            emit contactListModelChanged(&contactListModel_);
            break;
        case Header::signin|Header::error:
        case Header::signup|Header::error:
            emit setSignInUpInformSignal(QString(data.data1.c_str()));
            break;
        case Header::find_contact|Header::ack:
            emit findContactSuccessSignal(QString(data.data1.c_str()));
            break;
        case Header::find_contact|Header::error:
            emit findContactFailureSignal(QString(std::string(data.data1+" not found").c_str()));
            break;
        case Header::find_group|Header::ack:
            emit findGroupSuccessSignal(QString(data.data1.c_str()));
            break;
        case Header::find_group|Header::error:
            emit findGroupFailureSignal(QString(std::string(data.data1+" not found").c_str()));
            break;
        default:
            break;
    }
}

void GuiBridge::processMessage(Stream data)
{

}

void GuiBridge::currentUser(int index)
{
    emit resetMessageModel(contactListModel_.getContact(index));
}

void GuiBridge::insertContact(QString name)
{
    client.insertContact(name.toStdString(),Client::ContactType::individual);
    emit resetContactModel();
}

void GuiBridge::insertGroup(QString name)
{
    client.insertContact(name.toStdString(),Client::ContactType::group);
    emit resetContactModel();
}

void GuiBridge::connect(QString host, QString port)
{
    if(host.isEmpty() || port.isEmpty())
    {
        return;
    }

    client.connect(host.toStdString(),port.toStdString(),[this](asio::error_code error)
    {
        if(error)
        {
            emit setConnectInformSignal(std::string("Connection Error: "+error.message()).c_str());
        }
        else
        {
            reader();
            emit setConnectInformSignal(QString("Connection Successful"));
        }
    }
    );
    if(!isThreadRunning)
    {
        ioThread = std::thread([this]()
        {
            isThreadRunning = true;
            client.runIOContext();
            //isThreadRunning = false;
        });
    }
}
