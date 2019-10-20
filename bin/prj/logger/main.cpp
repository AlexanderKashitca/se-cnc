///-----------------------------------------------------------------------------
#include <QCoreApplication>
///-----------------------------------------------------------------------------
#include "../../../src/logger/logger.h"
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    LOGGER_SPACE::LoggerClass logger;
    LOGGER_SPACE::STREAM_TYPE type;
    LOGGER_SPACE::STREAM_CONFIG config;
    type = LOGGER_SPACE::STREAM_TYPE::STREAM_FILE;
    config.name = "CNC";
    config.file_path = "/home/evil/Programming/qt/se-cnc/bin/prj/logger";
    config.file_name = "logger.txt";
    logger.setConfiguration(type,config);
    logger.push("Test Data File String");
    logger.push("Next Line Test Data File String");


    return app.exec();
}
///-----------------------------------------------------------------------------
