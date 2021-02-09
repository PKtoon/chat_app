#include "conndialog.hpp"

ConnDialog::ConnDialog(QWidget* parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    decorate();
}

void ConnDialog::decorate()
{   

    connect(cancel,&QPushButton::clicked,this, &QWidget::close);
    
    grid->addWidget(name,0,0);
    grid->addWidget(nameInput,1,0);
    grid->addWidget(passwd,0,1);
    grid->addWidget(passwdInput,1,1);
    grid->addWidget(hostLabel,2,0);
    grid->addWidget(hostInput,3,0);
    grid->addWidget(portLabel,2,1);
    grid->addWidget(portInput,3,1);
    grid->addWidget(connButton,4,0);
    grid->addWidget(inform,5,0,1,2,Qt::AlignCenter);
    grid->addWidget(cancel,4,1);
}

void ConnDialog::connInit()
{
    emit doConnect(hostInput->text(),portInput->text());
}

void ConnDialog::signInInit()
{
    emit doUserAuth(nameInput->text(),passwdInput->text(), false);
}

void ConnDialog::signUpInit()
{
    emit doUserAuth(nameInput->text(),passwdInput->text(), true);
}

void ConnDialog::setInform(QString text)
{
    inform->setText(text);
}

void ConnDialog::setHostInputText(QString text)
{
    hostInput->setText(text);
}

void ConnDialog::setPortInputText(QString text)
{
    portInput->setText(text);
}

void ConnDialog::setCancelButtonText(QString text)
{
    cancel->setText(text);
}

void ConnDialog::userAuthBox(bool flag)
{
    hostLabel->hide();
    portLabel->hide();
    hostInput->hide();
    portInput->hide();
    name->show();
    nameInput->show();
    passwd->show();
    passwdInput->show();
    if(flag)
        connect(connButton,&QPushButton::clicked, this,&ConnDialog::signUpInit);
    else
        connect(connButton,&QPushButton::clicked,this,&ConnDialog::signInInit);
}

void ConnDialog::connectBox()
{
    hostLabel->show();
    portLabel->show();
    hostInput->show();
    portInput->show();
    name->hide();
    nameInput->hide();
    passwd->hide();
    passwdInput->hide();
    connect(connButton,&QPushButton::clicked, this,&ConnDialog::connInit);
}

