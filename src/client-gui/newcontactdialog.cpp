#include "newcontactdialog.h"

NewContactDialog::NewContactDialog(QWidget* parent) : QDialog(parent)
{
    setLayout(gridMsg);
    gridMsg->addWidget(conLabel,0,0,Qt::AlignRight);
    gridMsg->addWidget(conInput,0,1);
    gridMsg->addWidget(createCon,1,0,2,0);
    
    connect(createCon,&QPushButton::clicked,this,&NewContactDialog::createPressed);
}

void NewContactDialog::createPressed()
{
    emit createContact(conInput->text());
}
