#include <QCoreApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "pong.h"
#include "common.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QObject obj;
    Pong *pong = new Pong(&obj);
    QObject::connect(&app, &QCoreApplication::aboutToQuit, pong, &Pong::aboutToQuit);
    pong->setProperty("value", "initial value");
    QDBusConnection::sessionBus().registerObject("/", &obj);

    if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));
        exit(1);
    }

    app.exec();
    return 0;
}
