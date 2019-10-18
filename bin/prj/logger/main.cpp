///-----------------------------------------------------------------------------
#include <QCoreApplication>
///-----------------------------------------------------------------------------
#include "../../../src/logger/logger.h"
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    LoggerClass logger;
    QString str = "TestDAta String";

    logger << str;
    logger << str;


    return app.exec();
}
///-----------------------------------------------------------------------------
