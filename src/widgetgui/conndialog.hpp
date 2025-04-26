#ifndef CONNDIALOG_H
#define CONNDIALOG_H

#include <QtWidgets>
#ifndef NDEBUG
#include <iostream>
#endif

class ConnDialog : public QDialog
{
    Q_OBJECT
    
    QGridLayout grid;
    QLabel name;
    QLineEdit nameInput;
    QLabel passwd;
    QLineEdit passwdInput;
    QLabel hostLabel;
    QLineEdit hostInput;
    QLabel portLabel;
    QLineEdit portInput;
    QPushButton connButton;
    QPushButton cancel;
    QLabel inform;
    
    void decorate();
    
public:
    explicit ConnDialog(QWidget *parent = nullptr);

    void setInform(QString text);
    void setHostInputText(QString text);
    void setPortInputText(QString text);
    void setCancelButtonText(QString text);
    void userAuthBox(bool flag);
    void connectBox();

signals:
    void doConnect(const QString host, const QString port);
    void doUserAuth(const QString name, const QString passwd, const bool flag);

public slots:
    void connInit();
    void signInInit();
    void signUpInit();
};

#endif // CONNDIALOG_H
