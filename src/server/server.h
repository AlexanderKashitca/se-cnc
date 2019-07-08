///-----------------------------------------------------------------------------
#ifndef SERVER_H
#define SERVER_H
///-----------------------------------------------------------------------------
#include <QtCore/QObject>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusVariant>
///-----------------------------------------------------------------------------
#include "server_common.h"
#include "../direct/direct.h"
#include "../motion/motion_service.h"

///-----------------------------------------------------------------------------
#define SE_MOTION_DBUS_BUFSIZE  4096
///-----------------------------------------------------------------------------
class ServerClass : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface",SE_MOTION_SERVICE_INTERFACE_NAME)
    Q_PROPERTY(QString value READ value WRITE setValue)
    private :
        char _replyBuffer[SE_MOTION_DBUS_BUFSIZE];
        char _requestBuffer[SE_MOTION_DBUS_BUFSIZE];
        unsigned int _replyBytes;
        unsigned int _requestBytes;
        MotionDirectClass _motionDirect;
    public :
        QString m_value;
        QString value() const{return m_value;}
        void setValue(const QString &newValue){m_value = newValue;}


        ServerClass(QObject *obj) : QDBusAbstractAdaptor(obj){}
        ServerClass(const ServerClass&) = delete; /// disable copy constructor
        ~ServerClass();
        void GetQueryToAnswer();
    signals :
        void aboutToQuit();
    public slots :
        QDBusVariant query(const QByteArray &query);
        QDBusVariant query(const QString &query);
        Q_NOREPLY void quit();
};
///-----------------------------------------------------------------------------
#endif /// SERVER_H
///-----------------------------------------------------------------------------

