#ifdef __WIN32__
#include <winsock2.h>
#endif

#include <asio.hpp>

#include <QApplication>

#include <thread>

#include "mainwindow.h"

Q_DECLARE_METATYPE(asio::error_code)
Q_DECLARE_METATYPE(std::size_t)
Q_DECLARE_METATYPE(Stream)

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PK2n");
    QCoreApplication::setApplicationName("Chat app client");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    
    qRegisterMetaType<asio::error_code>();
    qRegisterMetaType<std::size_t>();
    qRegisterMetaType<Stream>();

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
