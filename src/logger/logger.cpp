///-----------------------------------------------------------------------------
#include "logger.h"
using namespace LOGGER_SPACE;
///-----------------------------------------------------------------------------
LoggerClass::LoggerClass()
{
    _serviceName.clear();
    _messageData.clear();
    _streamType = STREAM_DEBUG;
    _message.clear();
}
///-----------------------------------------------------------------------------
LoggerClass::~LoggerClass()
{

}
///-----------------------------------------------------------------------------
LOGGER_STATE LoggerClass::send(QString data)
{
    _dateTime = QDateTime::currentDateTime();
    _message.clear();

    _message.append(_dateTime.date().toString());
    _message.append(" ");
    _message.append(_dateTime.time().toString());
    _message.append(" | ");
    _message.append(_serviceName);
    _message.append(" | ");
    _message.append(data);
    _message.append('\n');

    switch(_streamType)
    {
        case STREAM_STDOUT :
            std::cout << _message.begin();
            break;
        case STREAM_DEBUG :
            qDebug() << QString(_message);
            break;
        case STREAM_CONSOL :
            break;
        case STREAM_FILE :
            if(!_file.open(QIODevice::Append | QIODevice::Text))
                return(LOGGER_FILE_NOT_OPENED);
            QTextStream out(&_file);
            out << _message;
            out.flush();
            _file.close();
            break;
    }
    return(LOGGER_OK);
}
///-----------------------------------------------------------------------------
LOGGER_STATE LoggerClass::setConfiguration(STREAM_TYPE& type,STREAM_CONFIG& config)
{
    QString file_name;
    _streamType = type;
    _serviceName = config.name;

    if(_streamType == STREAM_FILE)
    {
        file_name.clear();
        file_name.append(config.file_path);
        file_name.append("/");
        file_name.append(config.file_name);
        _file.setFileName(file_name);
    }
    return(LOGGER_OK);
}
///-----------------------------------------------------------------------------
LOGGER_STATE LoggerClass::push(qint8 data)
{
    return(send(QString::number(data)));
}
///-----------------------------------------------------------------------------
LOGGER_STATE LoggerClass::push(QString data)
{
    return(send(data));
}
///-----------------------------------------------------------------------------
