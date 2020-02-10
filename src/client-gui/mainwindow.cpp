#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"
#include "contactlistitem.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setCentralWidget(center);
    createMenuBar();
    decorate();
    setContactList();
}

MainWindow::~MainWindow()
{
    if(isThreadRunning)
    {
        disConnect();
        work.reset();
        ioThread.join();
    }
}


//GUI functions
void MainWindow::createMenuBar()
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

void MainWindow::decorate()
{
    QGridLayout* mainLayout = new QGridLayout(center);

    message->setReadOnly(true);
    
    mainLayout->addWidget(contactsListWidget,0,0,2,1);
    mainLayout->addWidget(message,0,1,1,2);
    mainLayout->addWidget(msgIn,1,1);
    mainLayout->addWidget(sendButt,1,2);
    
    connect(contactsListWidget,&QListWidget::itemChanged,this,&MainWindow::displayMessage);
    connect(sendButt,&QPushButton::clicked,this,&MainWindow::sendMessage);
    connect(this,&MainWindow::insertText,message,&QTextEdit::insertPlainText);
    
    center->setLayout(mainLayout);
}

void MainWindow::setContactList()
{
    for(auto& a : contactsList)
        contactsListWidget->addItem(&a);
    
    connect(contactsListWidget,&QListWidget::itemClicked,this,&MainWindow::displayMessage);
}

ContactListItem* MainWindow::makeContact(const QString& text)
{
    return new ContactListItem(text,contactsListWidget);
}

void MainWindow::processMessage(Stream data)
{
    ContactListItem* user;
    user=getUser(QString(data.sender.c_str()));
    if(!user)
    {
        user = makeContact(data.sender.c_str());
    }
    user->pushMsg(MessageItem(data.sender,data.data1));
    emit contactsListWidget->itemChanged(user);
}

ContactListItem* MainWindow::getUser(QString anon)
{
    auto list = contactsListWidget->findItems(anon,0);
    if(!list.isEmpty())
        return static_cast<ContactListItem*>(*(list.begin()));
    return nullptr;
}

//client core
void MainWindow::initialize()
{
    Stream initPack;
    initPack.head = Header::INIT;
    initPack.sender = name.toStdString();
    net.send(initPack,[this](boost::system::error_code error, std::size_t sent)
    {
        if(error)
        {
            if(error != boost::asio::error::operation_aborted)
                initialize();
        }
        else
        {
            net.receive([this](Stream initAck, boost::system::error_code error, std::size_t read)
            {
                if(error)
                {
                    if(error != boost::asio::error::operation_aborted)
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

void MainWindow::reader()
{
    net.receive([this](Stream data, boost::system::error_code error, std::size_t read)
    {
        if(error)
        {
            if(error != boost::asio::error::operation_aborted)
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

//slots
void MainWindow::initConnect()
{
    connDialog = new ConnDialog(this);
    connDialog->setWindowTitle("Connect");

    connect(connDialog,&ConnDialog::doConnect, this, &MainWindow::doConnect);

    connDialog->setHostInputText("localhost");
    connDialog->setPortInputText("1098");

    connDialog->show();
}

void MainWindow::doConnect(const QString userName, const QString host, const QString portNum)
{
    name = userName;
    hostname = host;
    port = portNum;

    if(name.isEmpty() || hostname.isEmpty() || port.isEmpty())
    {
        connDialog->setInform("All fields are necessary");
        return;
    }

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
            connDialog->setCancelButtonText("Close");
            connDialog->setInform("Connected Successfully");
            initialize();
        }
    }
    );
    if(!isThreadRunning)
    {
        ioThread = std::thread([this]()
        {
            isThreadRunning = true;
            io.run();
        });
    }
}

void MainWindow::disConnect()
{
    net.disconnect();
}

void MainWindow::displayMessage(QListWidgetItem* item)
{
    message->clear();
    ContactListItem* item2 = static_cast<ContactListItem*>(item);
    contactsListWidget->setCurrentItem(item2);

    for(auto& a : (*item2).msg)
    {
        QString msg {QString(a.from.c_str())+QString(":\n")+QString(a.message.c_str())+QString("\n\n")};
        emit insertText(msg);
    }
    message->ensureCursorVisible();
}

void MainWindow::sendMessage()
{
    if(msgIn->text().isEmpty() || !(contactsListWidget->currentItem()))
    {
        return;
    }

    Stream data;
    data.head = Header::MESSAGE;
    data.sender = name.toStdString();
    data.receiver = contactsListWidget->currentItem()->text().toStdString();
    data.data1 = msgIn->text().toStdString();
    ContactListItem* user = static_cast<ContactListItem*>(contactsListWidget->currentItem());
    user->pushMsg(MessageItem(data.sender,data.data1));
    emit contactsListWidget->itemChanged(user);             //emit to display message using main thread of execution
    msgIn->clear();
    queueMessage(data);
}

void MainWindow::newContact()
{
    newContactDialog = new NewContactDialog(this);
    connect(newContactDialog,&NewContactDialog::createContact,this,&MainWindow::createContact);

    newContactDialog->show();
}

void MainWindow::createContact(const QString& text)
{
    newContactDialog->close();
    ContactListItem* user;
    user=getUser(text);
    if(!user)
    {
        user = makeContact(text);
    }
    emit contactsListWidget->itemChanged(user);             //emit to display message using main thread of execution
}
