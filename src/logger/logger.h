///-----------------------------------------------------------------------------
#ifndef LOGGER_H
#define LOGGER_H
///-----------------------------------------------------------------------------
#include <QDateTime>
#include <QString>
#include <QFile>
///-----------------------------------------------------------------------------
#include <QDebug>
#include <iostream>
///-----------------------------------------------------------------------------
namespace LOGGER_SPACE
{
    /// STREAM_STDOUT - standard stdout port
    /// STREAM_CONSOL - serial com port
    /// STREAM_FILE   - log file
    /// STREAM_DEBUG  - qDebug
    typedef enum
    {
        STREAM_STDOUT = 0,
        STREAM_CONSOL = 1,
        STREAM_FILE   = 2,
        STREAM_DEBUG  = 3
    }STREAM_TYPE;
    ///
    typedef struct
    {
        QString name;
        QString file_path;
        QString file_name;
    }STREAM_CONFIG;
    ///
    typedef enum
    {
        LOGGER_OK = 0,
        LOGGER_FILE_NOT_OPENED
    }LOGGER_STATE;
    ///-----------------------------------------------------------------------------
    class LoggerClass
    {
        private :
            QFile       _file;
            QDateTime   _dateTime;
            QString     _serviceName;
            STREAM_TYPE _streamType;

            QString     _message;
            QString     _messageData;
            LOGGER_STATE inline send(QString);
        public:
            LoggerClass();
            ~LoggerClass();
            LOGGER_STATE setConfiguration(STREAM_TYPE& type,STREAM_CONFIG& config);
            LOGGER_STATE push(qint8 data);
            LOGGER_STATE push(qint16 data);
            LOGGER_STATE push(qint64 data);
            LOGGER_STATE push(qint32 data);
            LOGGER_STATE push(quint8 data);
            LOGGER_STATE push(quint16 data);
            LOGGER_STATE push(quint64 data);
            LOGGER_STATE push(quint32 data);
            LOGGER_STATE push(QString data);



    };
} /// end LOGGER_SPACE
///-----------------------------------------------------------------------------
#endif /// LOGGER_H
///-----------------------------------------------------------------------------
