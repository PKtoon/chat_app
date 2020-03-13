#include <QDebug>

#include "mainwindow.h"

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
    std::string query {"SELECT name FROM contacts;"};
    
    auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName {columnName[0]};            //need to store in a string first as columnName[0]=="name" always fails
            if(colName=="name")
                new QListWidgetItem(columnData[0],contactsListWidget);
    
            return 0;
        };
        
    if(!db.queryExec(query,func))
        qInfo()<<db.getError().c_str();
}

QListWidgetItem* MainWindow::makeContact(const QString& text)
{
    std::string query{"INSERT INTO contacts VALUES (\""+text.toStdString()+"\");"};
    if(!db.queryExec(query))
        qInfo()<<db.getError().c_str();
    return new QListWidgetItem(text,contactsListWidget);
}

void MainWindow::processMessage(Stream data)
{
    QListWidgetItem* user = getUser(QString(data.sender.c_str()));
    if(!user)
        user = makeContact(data.sender.c_str());

    std::string query {"INSERT INTO messages (name,sender,message,time) VALUES (\""+data.sender+"\", \""+data.sender+"\", \""+data.data1+"\",datetime(\"now\"));"};
    if(!db.queryExec(query))
        qInfo()<<db.getError().c_str();
    emit contactsListWidget->itemChanged(user);
}

QListWidgetItem* MainWindow::getUser(QString user)
{
    auto list = contactsListWidget->findItems(user,0);
    if(!list.isEmpty())
        return (*(list.begin()));
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
                    if(initAck.head == static_cast<Header>(Header::INIT | Header::ACK))
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

//same problem as described in server. one failing message will stall others. well here the recipient is only server, so if one message have network error all other messages will also.
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
    
    //values below are for testing purpose
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
    //todo:
    //better disconnect. send a socket close to server and then close the socket.
    //todo end
    net.disconnect();
}

void MainWindow::displayMessage(QListWidgetItem* item)
{
    message->clear();
    contactsListWidget->setCurrentItem(item);
    
    std::string query = "SELECT sender,message FROM messages WHERE name=\""+item->text().toStdString()+"\";";
    
    auto func = [this](int numOfColumns,char **columnData, char **columnName)->int
    {
        QString msg {QString(columnData[0])+QString(":\n")+QString(columnData[1])+QString("\n\n")};
        emit insertText(msg);
        return 0;
    };
    
    if(!db.queryExec(query,func))
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
    QListWidgetItem* user = contactsListWidget->currentItem();
    
    std::string query {"INSERT INTO messages (name,sender,message,time) VALUES (\""+data.receiver+"\", \""+data.sender+"\", \""+data.data1+"\",datetime(\"now\"));"};
    if(!db.queryExec(query))
        qInfo()<<db.getError().c_str();
    emit contactsListWidget->itemChanged(user);             //emit to display message using main thread of execution else it will fail in runtime with 'unable to create child' error
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
    QListWidgetItem* user = getUser(text);
    if(!user)
    {
        user = makeContact(text);
    }

    newContactDialog->close();      
    emit contactsListWidget->itemChanged(user);             //emit to display message using main thread of execution
}

void MainWindow::initDB()
{
    std::string query {"SELECT COUNT(name) FROM sqlite_master WHERE name=\"user\";"};
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
            {
                std::string colName{columnName[0]};
                std::string colData{columnData[0]};
                if(colName == "COUNT(name)" && colData == "0")
                {
                    if(!db.queryExec("CREATE TABLE user (name TEXT PRIMARY KEY);"))
                        qInfo()<<db.getError().c_str();
                }
                return 0;
            };
        if(!db.queryExec(query,func))
            qInfo()<<db.getError().c_str();
    }
    query ="SELECT COUNT(name) FROM sqlite_master WHERE name=\"contacts\";";
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName{columnName[0]};
            std::string colData{columnData[0]};
            if(colName == "COUNT(name)" && colData == "0")
            {
                if(!db.queryExec("CREATE TABLE contacts (name TEXT PRIMARY KEY);"))
                    qInfo()<<db.getError().c_str();
            }
                return 0;
        };
        if(!db.queryExec(query,func))
            qInfo()<<db.getError().c_str();
    }
    query ="SELECT COUNT(name) FROM sqlite_master WHERE name=\"messages\";";
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
            {
                std::string colName{columnName[0]};
                std::string colData{columnData[0]};
                if(colName == "COUNT(name)" && colData == "0")
                {
                    if(!db.queryExec("CREATE TABLE messages (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, sender TEXT, message TEXT, time TEXT);"))
                        qInfo()<<db.getError().c_str();
                }
                    return 0;
            };
        if(!db.queryExec(query,func))
            qInfo()<<db.getError().c_str();
    }
}
