#include "guibridge.hpp"

GuiBridge::GuiBridge(QObject *parent) : QObject(parent)
{

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
    Stream data;
    data.head = Header::find;
    data.sender = client.name();
    data.receiver = "server";
    data.data1 = contactName.toStdString();
    client.queueMessage(data);
}

void GuiBridge::reader()
{
    client.reader([this](Stream data, asio::error_code error, std::size_t read)
    {
        if(error)
        {

        }
        else
            processData(data);
        reader();
    });
}

void GuiBridge::writer(int receiver, QString message)
{
    Stream data;
    data.head = Header::message;
    data.sender = client.name();
    data.receiver = contactListModel_->getContact(receiver).toStdString();
    data.data1 = message.toStdString();
    client.queueMessage(data);
    emit resetMessageModel(data.receiver.c_str());
}

void GuiBridge::processData(Stream data)
{
    switch(data.head)
    {
        case Header::message:
            if(!contactListModel_->findContact(data.sender.c_str()))
                emit resetContactModel();
            processMessage(data);
            emit messageReceivedSignal(data.sender.c_str());
            break;
        case Header::signin|Header::ack:
        case Header::signup|Header::ack:
            emit setSignInUpInformSignal(QString("Successfully authenticated"));
            emit resetContactModel();
            emit contactListModelChanged(contactListModel_);
            break;
        case Header::signin|Header::error:
        case Header::signup|Header::error:
            emit setSignInUpInformSignal(QString(data.data1.c_str()));
            break;
        case Header::find|Header::ack:
            contactName = data.data1.c_str();
            emit findContactSuccessSignal();
            break;
        case Header::find|Header::error:
            emit findContactFailureSignal(QString(std::string(data.data1+" not found").c_str()));
            break;
        default:
            break;
    }
}

void GuiBridge::processMessage(Stream data)
{
//    QListWidgetItem* user = getUser(QString(data.sender.c_str()));
//    if(!user)
//        user = makeContact(data.sender.c_str());

    //    emit contactsListWidget->itemChanged(user);
}

void GuiBridge::currentUser(int index)
{
    emit resetMessageModel(contactListModel_->getContact(index));
}

void GuiBridge::insertContact(QString name)
{
    if(name != contactName)
        return;
    client.insertContact(name.toStdString());
    emit resetContactModel();
}

void GuiBridge::setContactListModel(ContactListModel *contactListModel)
{
    if (contactListModel_ == contactListModel)
        return;

    contactListModel_ = contactListModel;
    contactListModel_->client_ = &client;
    QObject::connect(this,&GuiBridge::resetContactModel, contactListModel_, &ContactListModel::resetModel,Qt::QueuedConnection);
    emit contactListModelChanged(contactListModel_);
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
