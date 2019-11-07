///-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
///-----------------------------------------------------------------------------
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>
///-----------------------------------------------------------------------------
#include "server.h"
///-----------------------------------------------------------------------------
/// the property
QString DBUS_SPACE::dbusServerClass::value() const
{
    return m_value;
}
///-----------------------------------------------------------------------------
void DBUS_SPACE::dbusServerClass::setValue(const QString &newValue)
{
    m_value = newValue;
}
///-----------------------------------------------------------------------------
void DBUS_SPACE::dbusServerClass::quit()
{
    QTimer::singleShot(0,QCoreApplication::instance(),&QCoreApplication::quit);
}
///-----------------------------------------------------------------------------
/*
QDBusVariant DBUS_SPACE::dbusServerClass::query(const QString &query)
{
    QString q = query.toLower();
    QByteArray ba = q.toLocal8Bit();
    const char* q_str = ba.data();

    fprintf(stdout,"server getting  query.\n");
    fprintf(stdout,"input query - %s\n",q_str);

    if(q == "hello")
        return QDBusVariant("World");
    if(q == "ping")
        return QDBusVariant("dbusServerClass");
    if(q.indexOf("the answer to life, the universe and everything") != -1)
        return QDBusVariant(42);
    if(q.indexOf("unladen swallow") != -1) {
        if(q.indexOf("european") != -1)
            return QDBusVariant(11.0);
        return QDBusVariant(QByteArray("african or european?"));
    }

    return QDBusVariant("Sorry, I don't know the answer");
}
*/
///-----------------------------------------------------------------------------
QDBusVariant DBUS_SPACE::dbusServerClass::getIO(const QList<QVariant>& io)
{
    fprintf(stdout,"server getting  IO with arguments.\n");

    return QDBusVariant("server getting  IO answer");
}
///-----------------------------------------------------------------------------
QDBusVariant DBUS_SPACE::dbusServerClass::query(const QString &query,const QList<QVariant>& args)
{
    QString q = query.toLower();
    QByteArray ba = q.toLocal8Bit();
    const char* q_str = ba.data();

    fprintf(stdout,"server getting  query with arguments.\n");
    fprintf(stdout,"input query - %s, arguments - \n",q_str);

    return QDBusVariant("Sorry, I don't know the answer");
}
