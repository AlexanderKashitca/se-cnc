///-----------------------------------------------------------------------------
#include "client.h"
///-----------------------------------------------------------------------------
bool DBUS_SPACE::dbusClientClass::initialization()
{
    if(!QDBusConnection::sessionBus().isConnected())
    {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return(false);
    }
    /// setting watcher
    _serviceWatcher.addWatchedService(SERVICE_NAME);
    _serviceWatcher.setConnection(QDBusConnection::sessionBus());
    _serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForRegistration);

    return(true);
}
///-----------------------------------------------------------------------------
bool DBUS_SPACE::dbusClientClass::connect(const QString &name)
{
//    QObject::connect(&_serviceWatcher,
//                     &QDBusServiceWatcher::serviceRegistered,
//                     this,
//                     &dbusClientClass::sendQuery);
    if(name != SERVICE_NAME)
        return(false);

    /// open stdin for reading
    qstdin.open(stdin,QIODevice::ReadOnly);
    /// find our remote
    iface = new QDBusInterface(SERVICE_NAME,"/",SERVICE_INTERFACE_NAME,QDBusConnection::sessionBus(), this);
    if(!iface->isValid())
    {
        fprintf(stderr,"%s\n",qPrintable(QDBusConnection::sessionBus().lastError().message()));
        QCoreApplication::instance()->quit();
    }
    QObject::connect(iface,SIGNAL(aboutToQuit()),QCoreApplication::instance(),SLOT(quit()));
    return(true);
}
///-----------------------------------------------------------------------------
/**
 * @brief dbusClientClass::sendQuery
 * @param  query  - query to server
 * @return answer - answer from server
 */
QString DBUS_SPACE::dbusClientClass::sendQuery(const QString& query,QList<QVariant>& args)
{
    QString answer = "NOT RESPONSE";
    QDBusReply<QDBusVariant> reply;



    reply = iface->call(QDBus::CallMode::AutoDetect,"getIO",args);

    if(reply.isValid())
    {
        answer = qPrintable(reply.value().variant().toString());
    }
    /// temp debuging
    qDebug() << "Server replay was - " << answer;
    return(answer);
}
///-----------------------------------------------------------------------------
