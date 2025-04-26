#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../client/client.hpp"

#include <QMainWindow>

#include "conndialog.hpp"
#include "newcontactdialog.hpp"

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
    void doConnect(const QString host, const QString portNum);
    void disConnect();
    void displayMessage(QListWidgetItem* item);
    void sendMessage();
    void newContact();
    void createContact(const QString& text);
    void findContact(const QString& text);
    void initSignIn();
    void initSignUp();
    void doUserAuth(const QString userName, const QString passWD, const bool flag);

private:
    //networking scaffold
    Client client{};
    std::thread ioThread;
    bool isThreadRunning{false};

    // GUI components
    ConnDialog* connDialog;
    ConnDialog* authDialog;
    NewContactDialog* newContactDialog;

    QWidget center;
    QListWidget contactsListWidget;
    QTextEdit message;
    QLineEdit msgIn;
    QPushButton sendButt;
    
    void createMenuBar();
    void decorate();
    void setContactList();
    QListWidgetItem* makeContact(const QString& text);
    QListWidgetItem* getUser(QString name);
    
    //client.h
    void initialize(QString userName, QString passwd, Header head);
    void reader();
    void processData(Stream data);
    void processMessage(Stream data);
    void initUserAuth(bool flag);
};

#endif // MAINWINDOW_H
