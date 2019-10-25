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
QString dbusServerClass::value() const
{
    return m_value;
}
///-----------------------------------------------------------------------------
void dbusServerClass::setValue(const QString &newValue)
{
    m_value = newValue;
}
///-----------------------------------------------------------------------------
void dbusServerClass::quit()
{
    QTimer::singleShot(0, QCoreApplication::instance(), &QCoreApplication::quit);
}
///-----------------------------------------------------------------------------
QDBusVariant dbusServerClass::query(const QString &query)
{
    QString q = query.toLower();

    fprintf(stdout,"server getting  query.\n");

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
///-----------------------------------------------------------------------------
