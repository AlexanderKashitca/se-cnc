///-----------------------------------------------------------------------------
#ifndef COMPLEXPONG_H
#define COMPLEXPONG_H
///-----------------------------------------------------------------------------
#include <QtCore/QObject>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusVariant>
///-----------------------------------------------------------------------------
#include "../common.h"
///-----------------------------------------------------------------------------
namespace DBUS_SPACE
{
    class dbusServerClass : public QDBusAbstractAdaptor
    {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface",SERVICE_INTERFACE_NAME)
            Q_PROPERTY(QString value READ value WRITE setValue)
        public:
            QString m_value;
            QString value() const;
            void setValue(const QString &newValue);

            dbusServerClass(QObject *obj) : QDBusAbstractAdaptor(obj){ }
        signals:
            void aboutToQuit();
        public slots:

            QDBusVariant query(const QString &query,const QList<QVariant>& args);

            QDBusVariant getIO(const QList<QVariant>& args);

            Q_NOREPLY void quit();
    };
} /// end namespace DBUS_SPACE


///-----------------------------------------------------------------------------
#endif
///-----------------------------------------------------------------------------
