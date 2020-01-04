#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <deque>

#include <QMainWindow>
#include <QListWidget>

#include <boost/asio.hpp>

#include "src/network-interface/netface.h"
#include "conndialog.h"
#include "newcontactdialog.h"
#include "contactlistitem.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:
    void insertText(const QString& text);

public slots:
    void initConnect();
    void doConnect(const QString userName, const QString host, const QString portNum);
    void disConnect();
    void displayMessage(QListWidgetItem* item);
    void sendMsg();
    void newContact();
    void createCon(const QString& text);

private:
    QString name = "PK";
    QString hostname;
    QString port;
    boost::asio::io_context io;
    NetFace net{io};
//     boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work = boost::asio::make_work_guard(io_context);
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>* work;
    std::thread ioThread;
    ConnDialog* connDialog;
    QListWidget* contactsList;
    std::deque<Stream> writeQueue;
    bool isWriting{false};
    QWidget* center = new QWidget(this);
    QTextEdit* message = new QTextEdit(center);
    QLineEdit* msgIn = new QLineEdit(center);
    QPushButton* sendButt = new QPushButton("Send",center);
    NewContactDialog* newContactDialog;
    std::vector<ContactListItem> contacts;
    
    void createActions();
    void decorate();
    void setContactList();
    void processMessage(Stream data);
    ContactListItem* createContact(const QString& text);
    ContactListItem* getUser(std::string name);
    
    //client.h
    void initialize();
    void reader();
    void writer();
    void processData(Stream);
    void queueMessage(Stream);
    void ping();
};

#endif // MAINWINDOW_H
