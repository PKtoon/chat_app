//TODO: clear contactListWidget when user is changed
//TODO: allow multiple users
#include <QDebug>
#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setCentralWidget(center);
    createMenuBar();
    decorate();
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

    QMenu* userMenu = menuBar()->addMenu(tr("&User"));

    QAction *signInAct = new QAction(connIcon,tr("Sign &In"),this);
    connect(signInAct,&QAction::triggered,this,&MainWindow::initSignIn);
    userMenu->addAction(signInAct);

    QAction *signUpAct = new QAction(connIcon,tr("Sign &Up"),this);
    connect(signUpAct,&QAction::triggered,this,&MainWindow::initSignUp);
    userMenu->addAction(signUpAct);
    
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

    //Experimental:
    contactsListWidget->clear();

    //TODO: possible memory leak
    for(auto& a : list)
        new QListWidgetItem(a.c_str(),contactsListWidget);
    if(!list.empty())
        emit contactsListWidget->itemClicked(contactsListWidget->item(0));
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
void MainWindow::initialize(QString userName, QString passwd, Header head)
{
    client.userAuthInit(userName.toStdString(), passwd.toStdString(), head);
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
        case Header::signin|Header::ack:
            connDialog->setCancelButtonText("Close");
            connDialog->setInform("Signed In Successfully");
            client.initDB();
            setContactList();
            break;
        case Header::signup|Header::ack:
            connDialog->setCancelButtonText("Close");
            connDialog->setInform("Signed Up Successfully");
            client.initDB();
            setContactList();
            break;
        case Header::signin|Header::error:
        case Header::signup|Header::error:
            connDialog->setInform(data.data1.c_str());
            break;
        case Header::find|Header::ack:
            if(newContactDialog->text().toStdString() == data.data1){
                newContactDialog->setInform("User found");
                newContactDialog->setCreateEnable(true);
            }
            break;
        case Header::find|Header::error:
            newContactDialog->setInform(std::string(data.data1+" not found").c_str());
        default:
            break;
    }
}

//slots
void MainWindow::initConnect()
{
    connDialog = new ConnDialog(this);
    connDialog->setWindowTitle("Connect");
    connDialog->connectBox();
    connect(connDialog,&ConnDialog::doConnect, this, &MainWindow::doConnect);
    
    //values below are for testing purpose
    connDialog->setHostInputText("localhost");
    connDialog->setPortInputText("1098");

    connDialog->show();
}

void MainWindow::doConnect(const QString userName, const QString passwd, const QString host, const QString port)
{
    if(host.isEmpty() || port.isEmpty())
    {
        connDialog->setInform("All fields are necessary");
        return;
    }

//     TODO: make it so that, there is no call to client.getSocket(), as everytime using newSocket is better and make newSocket to disconnect old socket and do not initialize NetFace at start, it is unnecessary as we are using new socket so it will initialized again anyways
    if(!client.getSocket()->is_open())
    {
        client.newSocket();

        client.connect(host.toStdString(),port.toStdString(),[this,userName,passwd](asio::error_code error)
        {
            if(error)
            {
                if(error != asio::error::operation_aborted)
                {
                    connDialog->setInform(std::string("Connection Error: "+ error.message()).c_str());
                }
            }
            else
            {
                reader();
                connDialog->setCancelButtonText("Close");
                connDialog->setInform("Connected Successfully");
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
    if(msgIn->text().isEmpty() || !(contactsListWidget->currentItem()) || client.name().empty())
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
    connect(newContactDialog,&NewContactDialog::findContact,this,&MainWindow::findContact);

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

void MainWindow::findContact(const QString &text)
{
    if(!client.getSocket()->is_open() || client.name().empty())
    {
        newContactDialog->setInform("Client is not connected");
        return;
    }
    Stream data;
    data.head = Header::find;
    data.sender = client.name();
    data.receiver = "server";
    data.data1 = text.toStdString();
    client.queueMessage(data);
}

void MainWindow::initSignIn()
{
    connDialog = new ConnDialog(this);
    connDialog->setWindowTitle("Sign In");

    connDialog->userAuthBox(false);

    connect(connDialog,&ConnDialog::doUserAuth, this, &MainWindow::doUserAuth);

    connDialog->show();
}

void MainWindow::initSignUp()
{
    connDialog = new ConnDialog(this);
    connDialog->setWindowTitle("Sign Up");

    connDialog->userAuthBox(true);

    connect(connDialog,&ConnDialog::doUserAuth, this, &MainWindow::doUserAuth);

    connDialog->show();
}

void MainWindow::doUserAuth(const QString userName, const QString passWD, const bool flag)
{
    if(userName.isEmpty() || passWD.isEmpty())
    {
        connDialog->setInform("All fields are necessary");
        return;
    }
    if(!client.getSocket()->is_open())
    {
        connDialog->setInform("Client is not connected");
        return;
    }
    if(flag)
        initialize(userName,passWD,Header::signup);
    else
        initialize(userName,passWD,Header::signin);
}
