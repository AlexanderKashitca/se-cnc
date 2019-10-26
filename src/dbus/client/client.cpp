///-----------------------------------------------------------------------------
#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtDBus/QtDBus>

#include "client.h"
///-----------------------------------------------------------------------------
int dbusClientClass::initialization()
{
    if(!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return(1);
    }
    /// setting watcher
    _serviceWatcher.addWatchedService(SERVICE_NAME);
    _serviceWatcher.setConnection(QDBusConnection::sessionBus());
    _serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForRegistration);

    return(0);
}
///-----------------------------------------------------------------------------
int dbusClientClass::connect(const QString &name)
{
    QObject::connect(&_serviceWatcher,
                     &QDBusServiceWatcher::serviceRegistered,
                     this,
                     &dbusClientClass::sendQuery(
                         _query,
                         _answer,
                         _data));
    if(name != SERVICE_NAME)
        return(1);

    /// open stdin for reading
    qstdin.open(stdin, QIODevice::ReadOnly);

    /// find our remote
    iface = new QDBusInterface(SERVICE_NAME,"/",SERVICE_INTERFACE_NAME,QDBusConnection::sessionBus(), this);
    if(!iface->isValid())
    {
        fprintf(stderr,"%s\n",qPrintable(QDBusConnection::sessionBus().lastError().message()));
        QCoreApplication::instance()->quit();
    }

    QObject::connect(iface,SIGNAL(aboutToQuit()),QCoreApplication::instance(),SLOT(quit()));

    return(0);
}
///-----------------------------------------------------------------------------
/**
 * @brief dbusClientClass::sendQuery
 * @param query  - query to server
 * @param answer - answer from server
 * @param data   - data the query if necessary
 * @return 0 - successful
 *         1 - failed
 */
int dbusClientClass::sendQuery(const QString& query,QByteArray& answer,QByteArray& data)
{




        printf("Ask your question: ");
        //QString line = QString::fromLocal8Bit(qstdin.readLine()).trimmed();
        //QByteArray ba = "";//"TEST SEND BBYTE ARRAY";

        QDBusReply<QDBusVariant> reply = iface->call("query", query);
        if(reply.isValid())
        {

  //          answer = qPrintable(reply.value().variant().toString());
        }
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

            return(0);
}
///-----------------------------------------------------------------------------
