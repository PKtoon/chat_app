#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <deque>

#include <QMainWindow>
#include <QListWidget>

#include <boost/asio.hpp>

#include "src/network-interface/netface.h"
#include "conndialog.h"
#include "newcontactdialog.h"

#include "external/SQLite/sqlite3_wrap.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void insertText(const QString& text);

public slots:
    void initConnect();
    void doConnect(const QString userName, const QString host, const QString portNum);
    void disConnect();
    void displayMessage(QListWidgetItem* item);
    void sendMessage();
    void newContact();
    void createContact(const QString& text);

private:
    //client parameters
    QString name;
    QString hostname;
    QString port;
    
    //networking scaffold
    boost::asio::io_context io;
    NetFace net{io};
    std::thread ioThread;
    bool isThreadRunning{false};
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work = boost::asio::make_work_guard(io);
    
    // I/O scaffold
    std::deque<Stream> writeQueue;
    bool isWriting{false};
    
    // GUI components
    ConnDialog* connDialog;
    NewContactDialog* newContactDialog;
    
    QWidget* center = new QWidget(this);
    QListWidget* contactsListWidget  = new QListWidget(center);
    QTextEdit* message = new QTextEdit(center);
    QLineEdit* msgIn = new QLineEdit(center);
    QPushButton* sendButt = new QPushButton("Send",center);
    
    void createMenuBar();
    void decorate();
    void setContactList();
    void processMessage(Stream data);
    QListWidgetItem* makeContact(const QString& text);
    QListWidgetItem* getUser(QString name);
    
    //client.h
    void initialize();
    void reader();
    void writer();
    void processData(Stream);
    void queueMessage(Stream);
    void ping();
    
    //SQLite3 db
    SQLite3DB db {"storage"};
    void initDB();
};

#endif // MAINWINDOW_H
