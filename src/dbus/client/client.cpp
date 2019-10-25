///-----------------------------------------------------------------------------
#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtDBus/QtDBus>

#include "../common.h"
#include "client.h"
///-----------------------------------------------------------------------------
int dbusClientClass::initialization()
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return(1);
    }


    serviceWatcher.addWatchedService(SERVICE_NAME);
    serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForRegistration);

    //serviceWatcher.setWatchMode(DBusServiceWatcher::WatchModeFlag::WatchForRegistration);
    //        (SERVICE_NAME,
    //         QDBusConnection::sessionBus(),
    //         QDBusServiceWatcher::WatchForRegistration);

    //QDBusServiceWatcher(const QString &service,
    //                    const QDBusConnection &connection,
    //                    WatchMode watchMode = WatchForOwnerChange,
    //                    QObject *parent = nullptr);

    return(0);
}
///-----------------------------------------------------------------------------
void dbusClientClass::start(const QString &name)
{
        QObject::connect(&serviceWatcher,&QDBusServiceWatcher::serviceRegistered,this,&dbusClientClass::start);
    if(name != SERVICE_NAME)
        return;

    // open stdin for reading
    qstdin.open(stdin, QIODevice::ReadOnly);

    // find our remote
    iface = new QDBusInterface(SERVICE_NAME,"/",SERVICE_INTERFACE_NAME,QDBusConnection::sessionBus(), this);
    if(!iface->isValid())
    {
        fprintf(stderr,"%s\n",qPrintable(QDBusConnection::sessionBus().lastError().message()));
        QCoreApplication::instance()->quit();
    }

    connect(iface, SIGNAL(aboutToQuit()),QCoreApplication::instance(),SLOT(quit()));

    while(true)
    {
        printf("Ask your question: ");

        QString line = QString::fromLocal8Bit(qstdin.readLine()).trimmed();
        QByteArray ba = "";//"TEST SEND BBYTE ARRAY";

        QDBusReply<QDBusVariant> reply = iface->call("query", line);
        if(reply.isValid())
            printf("Reply was: %s\n", qPrintable(reply.value().variant().toString()));


        ///if (line.isEmpty()) {
        ///    iface->call("quit");
        ///    return;
        ///} else if (line == "value") {
        ///    QVariant reply = iface->property("value");
        ///    if (!reply.isNull())
        ///        printf("value = %s\n", qPrintable(reply.toString()));
        ///} else if (line.startsWith("value=")) {
        ///    iface->setProperty("value", line.mid(6));
        ///} else {
        ///    QDBusReply<QDBusVariant> reply = iface->call("query", line);
        ///    if (reply.isValid())
        ///        printf("Reply was: %s\n", qPrintable(reply.value().variant().toString()));
        ///}
        ///if (iface->lastError().isValid())
        ///    fprintf(stderr, "Call failed: %s\n", qPrintable(iface->lastError().message()));
    }
}
///-----------------------------------------------------------------------------
