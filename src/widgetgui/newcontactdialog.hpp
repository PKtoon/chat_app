#ifndef NEWCONTACTDIALOG_H
#define NEWCONTACTDIALOG_H

#include <QtWidgets>

class NewContactDialog : public QDialog
{
    Q_OBJECT
    
    QGridLayout gridMsg;

    QLabel conLabel;
    QLineEdit conInput;
    QPushButton createCon;
    QPushButton findCon;
    QLabel informLabel;
    
public:
    NewContactDialog(QWidget *parent = nullptr);
    void setInform(QString text);
    QString text();
    void setCreateEnable(bool flag);

signals:
    void createContact(const QString &text);
    void findContact(const QString &text);
    
public slots:
    void createPressed();
    void findPressed();
};

#endif // NEWCONTACTDIALOG_H
