#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <deque>

#include <QMainWindow>
#include <QListWidget>

#include "conndialog.h"
#include "newcontactdialog.h"
#include "src/client-gui/client.hpp"

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
    void doConnect(const QString userName, const QString passWD, const QString host, const QString portNum);
    void disConnect();
    void displayMessage(QListWidgetItem* item);
    void sendMessage();
    void newContact();
    void createContact(const QString& text);

private:
    //client parameters
    QString name;
    QString passwd;
    QString hostname;
    QString port;
    
    //networking scaffold
    asio::io_context io;
    Client client;
    std::thread ioThread;
    bool isThreadRunning{false};
    asio::executor_work_guard<asio::io_context::executor_type> work = asio::make_work_guard(io);
    
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
    QListWidgetItem* makeContact(const QString& text);
    QListWidgetItem* getUser(QString name);
    
    //client.h
    void initialize();
    void reader();
    void processData(Stream data);
    void processMessage(Stream data);
};

#endif // MAINWINDOW_H
