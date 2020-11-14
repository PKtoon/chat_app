#include "conndialog.hpp"

ConnDialog::ConnDialog(QWidget* parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    decorate();
}

void ConnDialog::decorate()
{   
    connect(connButton,&QPushButton::clicked, this,&ConnDialog::connInit);
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
    emit doConnect(nameInput->text(),passwdInput->text(), hostInput->text(),portInput->text());
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

