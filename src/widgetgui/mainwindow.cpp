//TODO: clear contactListWidget when user is changed
//TODO: allow multiple users
#ifndef NDEBUG
#include <iostream>
#endif
#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    center(this),
    contactsListWidget(&center),
    message(&center),
    msgIn(&center),
    sendButt("Send", &center)
{
    setCentralWidget(&center);
    createMenuBar();
    decorate();
#if (QT_VERSION_MAJOR < 6)
    player.setMedia(QUrl("qrc:/resources/assets/Done_for_You/done-for-you-612.ogg"));
#else
    player.setSource(QUrl("qrc:/resources/assets/Done_for_You/done-for-you-612.ogg"));
#endif
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
    QAction *connAct = new QAction(connIcon, tr("&Connect"), this);
    connect(connAct, &QAction::triggered, this, &MainWindow::initConnect);
    connMenu->addAction(connAct);
    
    QAction *disConnAct = new QAction(connIcon, tr("&Disconnect"), this);
    connect(disConnAct, &QAction::triggered, this, &MainWindow::disConnect);
    connMenu->addAction(disConnAct);

    QMenu* userMenu = menuBar()->addMenu(tr("&User"));

    QAction *signInAct = new QAction(connIcon, tr("Sign &In"), this);
    connect(signInAct, &QAction::triggered, this, &MainWindow::initSignIn);
    userMenu->addAction(signInAct);

    QAction *signUpAct = new QAction(connIcon, tr("Sign &Up"), this);
    connect(signUpAct, &QAction::triggered, this, &MainWindow::initSignUp);
    userMenu->addAction(signUpAct);
    
    QMenu* msgMenu = menuBar()->addMenu(tr("&Message"));
    
    QAction *newContact = new QAction(connIcon, tr("&New Contact"), this);
    connect(newContact, &QAction::triggered, this, &MainWindow::newContact);
    msgMenu->addAction(newContact);
}

void MainWindow::decorate()
{
    QGridLayout* mainLayout = new QGridLayout(&center);

    message.setReadOnly(true);
    
    mainLayout->addWidget(&contactsListWidget, 0, 0, 2, 1);
    mainLayout->addWidget(&message, 0, 1, 1, 2);
    mainLayout->addWidget(&msgIn, 1, 1);
    mainLayout->addWidget(&sendButt, 1, 2);
    
    
    connect(&contactsListWidget, &QListWidget::itemChanged, this, &MainWindow::displayMessage);
    connect(&contactsListWidget, &QListWidget::itemClicked, this, &MainWindow::displayMessage);
    connect(&sendButt, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(this, &MainWindow::insertText, &message, &QTextEdit::insertPlainText);
    
    center.setLayout(mainLayout);
}

void MainWindow::setContactList()
{
    std::vector<std::pair<std::string, int>> list;

    if(!client.getContactList(list))
        qInfo()<<client.getDBError().c_str();

    //Experimental:
    contactsListWidget.clear();

    //TODO: possible memory leak
    for(auto& a : list)
        new QListWidgetItem(a.first.c_str(), &contactsListWidget);
    if(!list.empty())
        emit contactsListWidget.itemClicked(contactsListWidget.item(0));
}

QListWidgetItem* MainWindow::makeContact(const QString& text)
{
    return new QListWidgetItem(text, &contactsListWidget);
}

void MainWindow::processMessage(Stream data)
{
    QListWidgetItem* user = getUser(QString(data.sender.c_str()));
    if(!user)
        user = makeContact(data.sender.c_str());

    emit contactsListWidget.itemChanged(user);
}

void MainWindow::initUserAuth(bool flag)
{
    authDialog = new ConnDialog(this);      // this dialog will delete itself after closing
    if(flag)
        authDialog->setWindowTitle("Sign Up");
    else
        authDialog->setWindowTitle("Sign In");
    authDialog->userAuthBox(flag);

    connect(authDialog, &ConnDialog::doUserAuth, this, &MainWindow::doUserAuth);

    authDialog->show();
}

QListWidgetItem* MainWindow::getUser(QString user)
{
    auto list = contactsListWidget.findItems(user, Qt::MatchExactly);
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

void MainWindow::processData(Stream data)
{
    switch(data.head)
    {
        case Header::message:
            processMessage(data);
            player.play();
            break;
        case Header::signin|Header::ack:
            authDialog->setCancelButtonText("Close");
            authDialog->setInform("Signed In Successfully");
            setContactList();
            break;
        case Header::signup|Header::ack:
            authDialog->setCancelButtonText("Close");
            authDialog->setInform("Signed Up Successfully");
            setContactList();
            break;
        case Header::signin|Header::error:
        case Header::signup|Header::error:
            authDialog->setInform(data.data1.c_str());
            break;
        case Header::find_contact|Header::ack:
            if(newContactDialog->text().toStdString() == data.data1){
                newContactDialog->setInform("User found");
                newContactDialog->setCreateEnable(true);
            }
            break;
        case Header::find_contact|Header::error:
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
    connect(connDialog, &ConnDialog::doConnect, this, &MainWindow::doConnect);
    
#ifndef NDEBUG
    //values below are for testing purpose
    connDialog->setHostInputText("localhost");
    connDialog->setPortInputText("1098");
#endif
    connDialog->show();
}

void MainWindow::doConnect(const QString host, const QString port)
{
    if(host.isEmpty() || port.isEmpty())
    {
        connDialog->setInform("All fields are necessary");
        return;
    }

    client.connect(host.toStdString(), port.toStdString(), [this](asio::error_code error)
    {
        if(error)
        {
            connDialog->setInform(std::string("Connection Error: "+ error.message()).c_str());
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
    else if(!client.isConnectedToServer())
    {
        if(ioThread.joinable())
        {
            ioThread.join();
            isThreadRunning = false;
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
    message.clear();
    contactsListWidget.setCurrentItem(item);
    
    std::vector<std::pair<std::string,std::string>> messages;

    if(!client.getMessages(item->text().toStdString(), messages))
        qInfo()<<client.getDBError().c_str();

    for(auto& a : messages)
    {
        QString msg {QString(a.first.c_str())+QString(":\n")+QString(a.second.c_str())+QString("\n\n")};
        emit insertText(msg);
    }

    message.ensureCursorVisible();
}

void MainWindow::sendMessage()
{
    if(msgIn.text().isEmpty() || !(contactsListWidget.currentItem()) || client.name().empty())
    {
        return;
    }

    Stream data;
    data.head = Header::message;
    data.sender = client.name();
    data.receiver = contactsListWidget.currentItem()->text().toStdString();
    data.data1 = msgIn.text().toStdString();
    QListWidgetItem* user = contactsListWidget.currentItem();
    client.queueMessage(data);
#ifndef NDEBUG
    std::cerr<<"queued:   "<<data.getSerialized()<<std::endl;
#endif
    emit contactsListWidget.itemChanged(user);             //emit to display message using main thread of execution else it will fail in runtime with 'unable to create child' error
    msgIn.clear();
}

void MainWindow::newContact()
{
    newContactDialog = new NewContactDialog(this);      // this dialog will automatically delete itself after closing
    connect(newContactDialog, &NewContactDialog::createContact, this, &MainWindow::createContact);
    connect(newContactDialog, &NewContactDialog::findContact, this, &MainWindow::findContact);

    newContactDialog->show();
}

void MainWindow::createContact(const QString& text)
{
    QListWidgetItem* user = getUser(text);
    if(!user)
    {
        client.insertContact(text.toStdString(), Client::ContactType::individual);
        user = makeContact(text);
    }

    newContactDialog->close();      
    emit contactsListWidget.itemChanged(user);             //emit to display message using main thread of execution
}

void MainWindow::findContact(const QString &text)
{
    if(!client.getSocket()->is_open() || client.name().empty())
    {
        newContactDialog->setInform("Client is not connected");
        return;
    }
    Stream data;
    data.head = Header::find_contact;
    data.sender = client.name();
    data.receiver = "server";
    data.data1 = text.toStdString();
    client.queueMessage(data);
#ifndef NDEBUG
    std::cerr<<"queued:   "<<data.getSerialized()<<std::endl;
#endif    
}

void MainWindow::initSignIn()
{
    initUserAuth(false);
}

void MainWindow::initSignUp()
{
    initUserAuth(true);
}

void MainWindow::doUserAuth(const QString userName, const QString passWD, const bool flag)
{
    if(userName.isEmpty() || passWD.isEmpty())
    {
        authDialog->setInform("All fields are necessary");
        return;
    }
    if(!client.getSocket()->is_open())
    {
        authDialog->setInform("Client is not connected");
        return;
    }
    if(flag)
        initialize(userName, passWD, Header::signup);
    else
        initialize(userName, passWD, Header::signin);
}
