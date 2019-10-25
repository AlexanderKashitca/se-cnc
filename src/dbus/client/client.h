///-----------------------------------------------------------------------------
#ifndef CLIENT_DBUS_H
#define CLIENT_DBUS_H
///-----------------------------------------------------------------------------
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtDBus/QDBusInterface>


//#include <QtCore/QFile>
//#include <QtCore/QDebug>
//#include <QtCore/QProcess>
#include <QtDBus/QtDBus>
///-----------------------------------------------------------------------------
class dbusClientClass : public QObject
{
        Q_OBJECT
    private :
        QDBusServiceWatcher serviceWatcher;
    public slots:
        void start(const QString &);

    private :
        int initialization();
    public:
        QFile qstdin;
        QDBusInterface *iface;



};
///-----------------------------------------------------------------------------
#endif /// CLIENT_DBUS_H
///-----------------------------------------------------------------------------
