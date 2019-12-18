#include <boost/asio.hpp>

#include <QApplication>

#include <thread>

#include "mainwindow.h"

Q_DECLARE_METATYPE(boost::system::error_code)
Q_DECLARE_METATYPE(std::size_t)

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PK2n");
    QCoreApplication::setApplicationName("Chat app client");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    
    qRegisterMetaType<boost::system::error_code>();
    qRegisterMetaType<std::size_t>();

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
