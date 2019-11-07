///-----------------------------------------------------------------------------
#ifndef CLIENT_DBUS_H
#define CLIENT_DBUS_H
///-----------------------------------------------------------------------------
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusInterface>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QProcess>

#include <stdio.h>
///-----------------------------------------------------------------------------
#include "../common.h"
///-----------------------------------------------------------------------------
namespace DBUS_SPACE
{
    class dbusClientClass : public QObject
    {
            Q_OBJECT
        private :
            QDBusServiceWatcher _serviceWatcher;
        public slots:
            bool connect(const QString& name);


        public :
            bool initialization();
            QString  sendQuery(const QString& query,QList<QVariant>& args);
        public:
            QFile qstdin;
            QDBusInterface *iface;
    };
}
///-----------------------------------------------------------------------------
#endif /// CLIENT_DBUS_H
///-----------------------------------------------------------------------------
