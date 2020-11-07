#include <QDebug>

#include "mainwindow.h"

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
    connect(contactsListWidget,&QListWidget::itemClicked,this,&MainWindow::displayMessage);
    connect(sendButt,&QPushButton::clicked,this,&MainWindow::sendMessage);
    connect(this,&MainWindow::insertText,message,&QTextEdit::insertPlainText);
    
    center->setLayout(mainLayout);
}

void MainWindow::setContactList()
{
    std::vector<std::string> list;

    if(!client.getContactList(list))
        qInfo()<<client.getDBError().c_str();
    for(auto& a : list)
        new QListWidgetItem(a.c_str(),contactsListWidget);
}

QListWidgetItem* MainWindow::makeContact(const QString& text)
{
    return new QListWidgetItem(text,contactsListWidget);
}

void MainWindow::processMessage(Stream data)
{
    QListWidgetItem* user = getUser(QString(data.sender.c_str()));
    if(!user)
        user = makeContact(data.sender.c_str());

    emit contactsListWidget->itemChanged(user);
}

QListWidgetItem* MainWindow::getUser(QString user)
{
    auto list = contactsListWidget->findItems(user,Qt::MatchExactly);
    if(!list.isEmpty())
        return (*(list.begin()));
    return nullptr;
}

//client core
void MainWindow::initialize(QString userName, QString passwd)
{
    client.init(userName.toStdString(),passwd.toStdString(),[this,userName,passwd](asio::error_code error, Stream initAck)
    {
        if(error)
            initialize(userName,passwd);
        else
            if(initAck.head == static_cast<Header>(Header::init | Header::ack))
            {
                reader();
            }
            else
                initialize(userName,passwd);
    });
}

void MainWindow::reader()
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

void MainWindow::processData(Stream data)
{
    switch(data.head)
    {
        case Header::message:
            processMessage(data);
            break;
        default:
            break;
    }
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

void MainWindow::doConnect(const QString userName, const QString passwd, const QString host, const QString port)
{
    if(userName.isEmpty() || passwd.isEmpty() || host.isEmpty() || port.isEmpty())
    {
        connDialog->setInform("All fields are necessary");
        return;
    }

//     TODO: make it so that, there is no call to client.getSocket(), as everytime using newSocket is better and make newSocket to disconnect old socket and do not initialize NetFace at start, it is unnecessary as we are using new socket so it will initialized again anyways
//    if(!client.getSocket())
    client.newSocket();

    client.connect(host.toStdString(),port.toStdString(),[this,userName,passwd](asio::error_code error)
    {
        if(error)
        {
            if(error != asio::error::operation_aborted)
            {
                connDialog->setInform("Connection Error");
            }
        }
        else
        {
            connDialog->setCancelButtonText("Close");
            connDialog->setInform("Connected Successfully");
            initialize(userName,passwd);
        }
    }
    );
    if(!isThreadRunning)
    {
        ioThread = std::thread([this]()
        {
            isThreadRunning = true;
            client.runIOContext();
//             isThreadRunning = false;
        });
    }
}

void MainWindow::disConnect()
{
    client.disconnect();
    if(isThreadRunning)
    {
        ioThread.join();
        isThreadRunning = false;
    }
}

void MainWindow::displayMessage(QListWidgetItem* item)
{
    message->clear();
    contactsListWidget->setCurrentItem(item);
    
    std::vector<std::pair<std::string,std::string>> messages;

    if(!client.getMessages(item->text().toStdString(),messages))
        qInfo()<<client.getDBError().c_str();

    for(auto& a : messages)
    {
        QString msg {QString(a.first.c_str())+QString(":\n")+QString(a.second.c_str())+QString("\n\n")};
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
    data.head = Header::message;
    data.sender = client.name();
    data.receiver = contactsListWidget->currentItem()->text().toStdString();
    data.data1 = msgIn->text().toStdString();
    QListWidgetItem* user = contactsListWidget->currentItem();
    
    if(!client.insertMessage(data.receiver,data.sender,data.data1))
        qInfo()<<client.getDBError().c_str();
    emit contactsListWidget->itemChanged(user);             //emit to display message using main thread of execution else it will fail in runtime with 'unable to create child' error
    msgIn->clear();
    client.queueMessage(data);
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
        client.insertContact(text.toStdString());
        user = makeContact(text);
    }

    newContactDialog->close();      
    emit contactsListWidget->itemChanged(user);             //emit to display message using main thread of execution
}
