#ifndef NEWCONTACTDIALOG_H
#define NEWCONTACTDIALOG_H

#include <QtWidgets>

class NewContactDialog : public QDialog
{
    Q_OBJECT
    
    QGridLayout* gridMsg = new QGridLayout(this);

    QLabel* conLabel = new QLabel("Name: ", this);
    QLineEdit* conInput = new QLineEdit(this);
    QPushButton* createCon = new QPushButton("Create",this);
    
public:
    NewContactDialog(QWidget *parent = nullptr);

signals:
    void createContact(const QString &text);
    
public slots:
    void createPressed();
};

#endif // NEWCONTACTDIALOG_H
