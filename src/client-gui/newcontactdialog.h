#ifndef NEWCONTACTDIALOG_H
#define NEWCONTACTDIALOG_H

#include <QtWidgets>
#include <QDialog>

class NewContactDialog : public QDialog
{
    Q_OBJECT

public:
    NewContactDialog(QWidget *parent = nullptr);
    void clear();

signals:
    void createContact(const QString &text);
    
public slots:
    void createPressed();
    
private:
    QGridLayout* gridMsg = new QGridLayout(this);
    
    QLabel* conLabel = new QLabel("Name: ", this);
    QLineEdit* conInput = new QLineEdit(this);
    QPushButton* createCon = new QPushButton("Create",this);
};

#endif // NEWCONTACTDIALOG_H
