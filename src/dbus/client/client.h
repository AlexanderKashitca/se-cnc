///-----------------------------------------------------------------------------
#ifndef CLIENT_DBUS_H
#define CLIENT_DBUS_H
///-----------------------------------------------------------------------------
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusInterface>

#include "../common.h"
///-----------------------------------------------------------------------------
class dbusClientClass : public QObject
{
        Q_OBJECT
    private :
        QDBusServiceWatcher _serviceWatcher;

    public slots:
        int connect(const QString& name);
        int sendQuery(const QString& query,QByteArray& answer,QByteArray& data);
    public :
        int initialization();

    public:
        QFile qstdin;
        QDBusInterface *iface;

        const QString _query;
        QByteArray    _answer;
        QByteArray    _data;

};
///-----------------------------------------------------------------------------
#endif /// CLIENT_DBUS_H
///-----------------------------------------------------------------------------
