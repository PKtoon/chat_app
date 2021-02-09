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
    QPushButton* findCon = new QPushButton("Find",this);
    QLabel* informLabel = new QLabel(this);
    
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
