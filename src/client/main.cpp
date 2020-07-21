#ifdef __WIN32__
#include <winsock2.h>
#endif

#include "mainwindow.h"

#include <QApplication>

//Q_DECLARE_METATYPE(asio::error_code)

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PK2n");
    QCoreApplication::setApplicationName("Chat app client");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    
//    qRegisterMetaType<asio::error_code>();

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
