#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.hpp"

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
    void doConnect(const QString userName, const QString passWD, const QString host, const QString portNum);
    void disConnect();
    void displayMessage(QListWidgetItem* item);
    void sendMessage();
    void newContact();
    void createContact(const QString& text);

private:
    //networking scaffold
    Client client{};
    std::thread ioThread;
    bool isThreadRunning{false};
    
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
    void initialize(QString userName, QString passwd);
    void reader();
    void processData(Stream data);
    void processMessage(Stream data);
};

#endif // MAINWINDOW_H
