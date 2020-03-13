#ifndef CONNDIALOG_H
#define CONNDIALOG_H

#include <QtWidgets>

class ConnDialog : public QDialog
{
    Q_OBJECT
    
    QGridLayout* grid = new QGridLayout(this);
    QLabel* name = new QLabel("Name:",this);
    QLineEdit* nameInput = new QLineEdit(this);
    QLabel* hostLabel = new QLabel("Host:",this);
    QLineEdit* hostInput = new QLineEdit(this);
    QLabel* portLabel = new QLabel("Port:",this);
    QLineEdit* portInput = new QLineEdit(this);
    QPushButton* connButton = new QPushButton("Connect",this);
    QPushButton* cancel = new QPushButton("Cancel",this);
    QLabel* inform = new QLabel(this);
    
    void decorate();
    
public:
    explicit ConnDialog(QWidget *parent = nullptr);

    void setInform(QString text);
    void setHostInputText(QString text);
    void setPortInputText(QString text);
    void setCancelButtonText(QString text);

signals:
    void doConnect(const QString name, const QString host, const QString port);

public slots:
    void connInit();
};

#endif // CONNDIALOG_H
