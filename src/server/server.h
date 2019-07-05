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
#define SE_MOTION_SERVER_INFO        "org.example.QtDBus.SeMotionService.Server"
///-----------------------------------------------------------------------------
class ServerClass: public QDBusAbstractAdaptor
{
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface",SE_MOTION_SERVER_INFO)
        Q_PROPERTY(QString value READ value WRITE setValue)
    public:
        QString m_value;
        QString value() const;
        void setValue(const QString &newValue);
        ServerClass(QObject *obj) : QDBusAbstractAdaptor(obj)
        { }
    signals:
        void aboutToQuit();
    public slots:
        QDBusVariant query(const QString &query);
        QDBusVariant query(const int &query);
        Q_NOREPLY void quit();
};
///-----------------------------------------------------------------------------
#endif /// SERVER_H
///-----------------------------------------------------------------------------
