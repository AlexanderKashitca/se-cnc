///-----------------------------------------------------------------------------
#ifndef LOGGER_H
#define LOGGER_H
///-----------------------------------------------------------------------------
#include <QString>
///-----------------------------------------------------------------------------
/// STREAM_STDOUT - standard stdout port
/// STREAM_COM    - serial com port
/// STREAM_FILE   - log file
typedef enum
{
    STREAM_STDOUT = 0,
    STREAM_COM    = 1,
    STREAM_FILE   = 2
}STREAM_TYPE;
///
typedef struct
{

}STREAM_CONFIG;

///-----------------------------------------------------------------------------
class LoggerClass
{
    private :
        QString _serviceName;
        quint8  _streamType;

        void send();
    public:
        LoggerClass();
        ~LoggerClass();
        quint8 setConfiguration(STREAM_TYPE type,STREAM_CONFIG config);

        void operator<<(QString data);
        void operator<<(qint8   data);
        void operator<<(qint16  data);
        void operator<<(qint32  data);
        void operator<<(qint64  data);
        void operator<<(quint8  data);
        void operator<<(quint16 data);
        void operator<<(quint32 data);
        void operator<<(quint64 data);

        QString temp;
        int ddata;
};
///-----------------------------------------------------------------------------
#endif /// LOGGER_H
///-----------------------------------------------------------------------------
