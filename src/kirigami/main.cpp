#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <guibridge.hpp>
#include <contactlistmodel.hpp>
#ifdef Q_OS_ANDROID
#include "../external/kirigami/src/kirigamiplugin.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<GuiBridge>("pkChat.GuiBridge",1,0,"PKGui");
    qmlRegisterType<ContactListModel>("pkChat.ContactListModel",1,0,"PKContact");
    qmlRegisterType<MessageListModel>("pkChat.MessageListModel",1,0,"PKMessageList");

    QQmlApplicationEngine engine;
#ifdef Q_OS_ANDROID
//KirigamiPlugin::getInstance().registerTypes();
#endif
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
