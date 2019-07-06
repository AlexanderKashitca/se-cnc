///-----------------------------------------------------------------------------
#ifndef SERVER_H
#define SERVER_H
///-----------------------------------------------------------------------------
#include <QtCore/QObject>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusVariant>
///-----------------------------------------------------------------------------
#include "server_common.h"
#include "../motion/motion_service.h"
///-----------------------------------------------------------------------------
#define SE_MOTION_SERVER_INFO   "org.example.QtDBus.SeMotionService.Server"
#define SE_MOTION_DBUS_BUFSIZE  4096
///-----------------------------------------------------------------------------
class ServerClass: public QDBusAbstractAdaptor
{
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface",SE_MOTION_SERVER_INFO)
        Q_PROPERTY(QString value READ value WRITE setValue)
    private:
        char _replyBuffer[SE_MOTION_DBUS_BUFSIZE];
        char _requestBuffer[SE_MOTION_DBUS_BUFSIZE];
        unsigned int _replyBytes;
        unsigned int _requestBytes;
    public:
        QString m_value;
        QString value() const;
        void setValue(const QString &newValue);

        ServerClass(QObject *obj) : QDBusAbstractAdaptor(obj){}
        void GetQueryToAnswer();
    signals:
        void aboutToQuit();
    public slots:
        QDBusVariant query(const QString &query);
        QDBusVariant query(const QByteArray &query);
        Q_NOREPLY void quit();
};
///-----------------------------------------------------------------------------
#endif /// SERVER_H
///-----------------------------------------------------------------------------
