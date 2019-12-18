#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "connectionmanager.h"
#include "netface.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:
    void initConnect();
    void connected(const boost::system::error_code result);
    void sentData(std::size_t sent);
    void receivedData(Stream stream);
    void sendError(boost::system::error_code error, std::size_t size);
    void receiveError(boost::system::error_code error, std::size_t size);

private:
    QString hostname;
    QString port;
    NetFace net;

    void createActions();
    void connectNetFace();
    void startReceive();
    void readSettings();
    void writeSettings();
};

#endif // MAINWINDOW_H
