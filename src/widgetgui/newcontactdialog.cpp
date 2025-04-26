#include "newcontactdialog.hpp"

NewContactDialog::NewContactDialog(QWidget* parent) : QDialog(parent),
    gridMsg(this),
    conLabel("Name: ", this),
    conInput(this),
    createCon("Create", this),
    findCon("Find", this),
    informLabel(this)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setLayout(&gridMsg);
    gridMsg.addWidget(&conLabel, 0, 0, Qt::AlignRight);
    gridMsg.addWidget(&conInput, 0, 1);
    gridMsg.addWidget(&findCon, 1, 1);
    gridMsg.addWidget(&createCon, 1, 0);
    gridMsg.addWidget(&informLabel, 2, 0, 2, 0, Qt::AlignCenter);

    createCon.setEnabled(false);
    
    connect(&createCon, &QPushButton::clicked, this, &NewContactDialog::createPressed);
    connect(&findCon, &QPushButton::clicked, this, &NewContactDialog::findPressed);
}

void NewContactDialog::setInform(QString text)
{
    informLabel.setText(text);
}

QString NewContactDialog::text()
{
    return conInput.text();
}

void NewContactDialog::setCreateEnable(bool flag)
{
    createCon.setEnabled(flag);
}

void NewContactDialog::createPressed()
{
    emit createContact(conInput.text());
}

void NewContactDialog::findPressed()
{
    emit findContact(conInput.text());
}
