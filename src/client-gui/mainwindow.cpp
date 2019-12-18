#include <QtWidgets>
#include <QDebug>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    createActions();
    connectNetFace();
}

void MainWindow::createActions()
{
    QMenu *connMenu = menuBar()->addMenu(tr("&Connect"));

    const QIcon connIcon = QIcon::fromTheme("network-wireless");
    QAction *connAct = new QAction(connIcon,tr("&Connect"),this);
    connect(connAct,&QAction::triggered,this,&MainWindow::initConnect);
    connMenu->addAction(connAct);
}

void MainWindow::connectNetFace()
{
    connect(&net,&NetFace::connected,this,&MainWindow::connected,Qt::QueuedConnection);
    connect(&net,&NetFace::sentData,this,&MainWindow::sentData,Qt::QueuedConnection);
    connect(&net,&NetFace::receivedData,this,&MainWindow::receivedData,Qt::QueuedConnection);
    connect(&net,&NetFace::sendError,this,&MainWindow::sendError,Qt::QueuedConnection);
    connect(&net,&NetFace::receiveError,this,&MainWindow::receiveError,Qt::QueuedConnection);
}

void MainWindow::startReceive()
{
}


void MainWindow::initConnect()
{
    hostname = "localhost";
    port = "1098";
    net.connect(hostname.toStdString(),port.toStdString());
}

void MainWindow::connected(const boost::system::error_code result)
{
    if (result){
 
    }
    else
    { 
        startReceive();
    }
}

void MainWindow::sentData(std::size_t sent)
{
    
}

void MainWindow::receivedData(Stream stream)
{
    
}

void MainWindow::sendError(boost::system::error_code error, std::size_t size)
{
    
}

void MainWindow::receiveError(boost::system::error_code error, std::size_t size)
{
    
}
