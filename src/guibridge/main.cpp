#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <guibridge.hpp>
#include <contactlistmodel.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<ContactListModel>("pkChat.ContactListModel",1,0,"ContactListModel");
    qmlRegisterType<MessageListModel>("pkChat.MessageListModel",1,0,"MessageListModel");
    qmlRegisterType<GuiBridge>("pkChat.GuiBridge",1,0,"PKGui");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
