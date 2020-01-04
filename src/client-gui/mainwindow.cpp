#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"
#include "contactlistitem.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setCentralWidget(center);
    createActions();
    decorate();
    setContactList();
    connect(this,&MainWindow::insertText,message,&QTextEdit::insertPlainText);
    ioThread = std::thread([this]()
    { 
        work = new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>(boost::asio::make_work_guard(io));
        io.run();
    });
}

void MainWindow::createActions()
{
    QMenu *connMenu = menuBar()->addMenu(tr("&Connect"));

    const QIcon connIcon = QIcon::fromTheme("network-wireless");
    QAction *connAct = new QAction(connIcon,tr("&Connect"),this);
    connect(connAct,&QAction::triggered,this,&MainWindow::initConnect);
    connMenu->addAction(connAct);
    
    QAction *disConnAct = new QAction(connIcon,tr("&Disconnect"),this);
    connect(disConnAct,&QAction::triggered,this,&MainWindow::disConnect);
    connMenu->addAction(disConnAct);
    
    QMenu* msgMenu = menuBar()->addMenu(tr("&Message"));
    
    QAction *newContact = new QAction(connIcon,tr("&New Contact"),this);
    connect(newContact,&QAction::triggered,this,&MainWindow::newContact);
    msgMenu->addAction(newContact);
}

void MainWindow::newContact()
{
    newContactDialog = new NewContactDialog(this);

    connect(newContactDialog,&NewContactDialog::createContact,this,&MainWindow::createCon);
    
    newContactDialog->show();
}

ContactListItem* MainWindow::createContact(const QString& text)
{
    ContactListItem* user;
    if(!(user = getUser(text.toStdString())))
        user = new ContactListItem(text,contactsList);
    return user;
}

void MainWindow::createCon(const QString& text)
{
    newContactDialog->close();
    createContact(text);
}


void MainWindow::disConnect()
{
    net.disconnect();
}

void MainWindow::initConnect()
{
    connDialog = new ConnDialog(this);
    connDialog->setWindowTitle("Connect");
    
    connect(connDialog,&ConnDialog::doConnect, this, &MainWindow::doConnect);
    
    connDialog->hostInput->setText("localhost");
    connDialog->portInput->setText("1098");
    
    connDialog->show();
}

void MainWindow::doConnect(const QString userName, const QString host, const QString portNum)
{
    name = userName;
    hostname = host;
    port = portNum;
    
    qInfo()<<name;
    qInfo()<<hostname;
    qInfo()<<port;
    net.connect(hostname.toStdString(),port.toStdString(),[this](boost::system::error_code error)
    {
        if(error)
        {
            if(error != boost::asio::error::operation_aborted)
            {
                connDialog->setInform("Connection Error");
            }
        }
        else
        {
            connDialog->cancel->setText("Close");
            connDialog->setInform("Connected Successfully");
            initialize();
        }
    }
    );
}

void MainWindow::initialize()
{
    Stream initPack;
    initPack.head = Header::INIT;
    initPack.sender = name.toStdString();
    net.send(initPack,[this](boost::system::error_code error, std::size_t sent)
    {
        if(error)
        {
            if(error !=boost::asio::error::operation_aborted)
                initialize();
        }
        else
        {
            net.receive([this](Stream initAck, boost::system::error_code error, std::size_t read)
            {
                if(error)
                {
                    if(error !=boost::asio::error::operation_aborted)
                        initialize();
                }
                else
                {
                    if(initAck.head ==(Header) (Header::INIT | Header::ACK))
                        reader();
                    else
                        initialize();
                }
            }
            );
        }
    }
    );
}

void MainWindow::processData(Stream data)
{
    switch(data.head)
    {
        case Header::PING:
            ping();
            break;
        case Header::MESSAGE:
            processMessage(data);
            break;
        default:
            break;
    }
}

void MainWindow::queueMessage(Stream data)
{
    writeQueue.push_back(data);
    if(!isWriting)
        writer();
}

void MainWindow::ping()
{
    Stream ping;
    ping.head = Header::PING;
    queueMessage(ping);
}

void MainWindow::reader()
{
    net.receive([this](Stream data, boost::system::error_code error, std::size_t read)
    {
        if(error)
        {
            if(error !=boost::asio::error::operation_aborted)
                reader();
        }
        else
        {
            processData(data);
            reader();
        }
    }
    );
}

void MainWindow::writer()
{
    isWriting = true;
    if(!writeQueue.empty())
    {
        net.send(*writeQueue.begin(),[this](boost::system::error_code error, std::size_t sent)
        {
            if(error)
            {
                if(error != boost::asio::error::operation_aborted)
                    writer();
            }
            else
            {
                writeQueue.pop_front();
                writer();
            }
        });
    }
    else
        isWriting = false;
}

void MainWindow::decorate()
{
    QGridLayout* mainLayout = new QGridLayout(center);
    
    contactsList = new QListWidget(this);
    message->setReadOnly(true);
    mainLayout->addWidget(contactsList,0,0,2,1);
    mainLayout->addWidget(message,0,1,1,2);
    mainLayout->addWidget(msgIn,1,1);
    mainLayout->addWidget(sendButt,1,2);
    
    connect(contactsList,&QListWidget::itemChanged,this,&MainWindow::displayMessage);
    connect(sendButt,&QPushButton::clicked,this,&MainWindow::sendMsg);
    
    center->setLayout(mainLayout);
}

void MainWindow::setContactList()
{
    for(auto& a : contacts)
        contactsList->addItem(&a);
    
    connect(contactsList,&QListWidget::itemClicked,this,&MainWindow::displayMessage);
}

void MainWindow::displayMessage(QListWidgetItem* item)
{
    message->clear();
    ContactListItem* item2 = static_cast<ContactListItem*>(item);
    contactsList->setCurrentItem(item2);
    
    for(auto& a : (*item2).msg)
    {
        emit insertText(QString(a.from.c_str()));
        emit insertText(QString(':'));
        emit insertText(QString('\n'));
        emit insertText(QString(a.message.c_str()));
        emit insertText(QString('\n'));
        emit insertText(QString('\n'));
    }
    message->ensureCursorVisible();
}

void MainWindow::processMessage(Stream data)
{
    ContactListItem* user;
    user=getUser(data.getSender());
    if(!user)
    {
        user = createContact(data.getSender().c_str());
    }
    user->pushMsg(MessageItem(data.getSender(),data.getData1()));
    emit contactsList->itemChanged(user);
}

ContactListItem* MainWindow::getUser(std::string anon)
{
    auto list = contactsList->findItems(anon.c_str(),0);
    if(!list.isEmpty())
        return static_cast<ContactListItem*>(*(list.begin()));
    return nullptr;
}

void MainWindow::sendMsg()
{
    Stream data;
    data.head = Header::MESSAGE;
    data.sender = name.toStdString();
    data.receiver = contactsList->currentItem()->text().toStdString();
    data.data1 = msgIn->text().toStdString();
    ContactListItem* user = static_cast<ContactListItem*>(contactsList->currentItem());
    user->pushMsg(MessageItem(data.sender,data.data1));
    emit contactsList->itemChanged(user);
    msgIn->clear();
    queueMessage(data);
}

