#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"
#include "contactlistitem.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setCentralWidget(center);
    createMenuBar();
    decorate();
    initDB();
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
    std::string query {"select name from contacts;"};
    
    if(!db.queryExec(query,[this](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName {columnName[0]};
            if(colName=="name")
            {
                new ContactListItem(columnData[0],contactsListWidget);
            }
            return 0;
        }))
        qInfo()<<db.getError().c_str();
    
    connect(contactsListWidget,&QListWidget::itemClicked,this,&MainWindow::displayMessage);
}

ContactListItem* MainWindow::makeContact(const QString& text)
{
    std::string query{"insert into contacts values (\""+text.toStdString()+"\");"};
    if(!db.queryExec(query))
        qInfo()<<db.getError().c_str();
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
    std::string query {"insert into messages (name,sender,message,time) values (\""+data.sender+"\", \""+data.sender+"\", \""+data.data1+"\",datetime(\"now\"));"};
    if(!db.queryExec(query))
        qInfo()<<db.getError().c_str();
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
    
    std::string query = "select sender,message from messages where name=\""+item2->text().toStdString()+"\";";
    
    auto fun = [this](int numOfColumns,char **columnData, char **columnName)->int
    {
        QString msg {QString(columnData[0])+QString(":\n")+QString(columnData[1])+QString("\n\n")};
        emit insertText(msg);
        
        return 0;
    };
    
    if(!db.queryExec(query,fun))
        qInfo()<<db.getError().c_str();
    
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
    
    std::string query {"insert into messages (name,sender,message,time) values (\""+data.receiver+"\", \""+data.sender+"\", \""+data.data1+"\",datetime(\"now\"));"};
    if(!db.queryExec(query))
        qInfo()<<db.getError().c_str();
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

void MainWindow::initDB()
{
    std::string query {"select count(name) from sqlite_master where name=\"user\";"};
    if(!db.queryExec(query,[this](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName{columnName[0]};
            std::string colData{columnData[0]};
            if(colName == "count(name)" && colData == "0")
            {
                if(!db.queryExec("create table user (name text primary key);"))
                    qInfo()<<db.getError().c_str();
            }
               return 0;
        }))
        qInfo()<<db.getError().c_str();
    
    
    
    query ="select count(name) from sqlite_master where name=\"contacts\";";
    if(!db.queryExec(query,[this](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName{columnName[0]};
            std::string colData{columnData[0]};
            if(colName == "count(name)" && colData == "0")
            {
                if(!db.queryExec("create table contacts (name text primary key);"))
                    qInfo()<<db.getError().c_str();
            }
                return 0;
        }))
        qInfo()<<db.getError().c_str();
    
    query ="select count(name) from sqlite_master where name=\"messages\";";
    if(!db.queryExec(query,[this](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName{columnName[0]};
            std::string colData{columnData[0]};
            if(colName == "count(name)" && colData == "0")
            {
                if(!db.queryExec("create table messages (id INTEGER PRIMARY KEY AUTOINCREMENT, name text, sender text, message text, time text);"))
                    qInfo()<<db.getError().c_str();
            }
                return 0;
        }))
        qInfo()<<db.getError().c_str();
}
