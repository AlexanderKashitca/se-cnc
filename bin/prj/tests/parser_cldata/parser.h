///-----------------------------------------------------------------------------
#ifndef PARSER_H
#define PARSER_H
///-----------------------------------------------------------------------------
#include <QFile>
#include <QTextStream>
///-----------------------------------------------------------------------------
#include "command.h"
///-----------------------------------------------------------------------------
class ParserClDataClass
{
    private :
        QFile*  _file_in;
        QString _filename;
    public :
        ParserClDataClass();
        ~ParserClDataClass();
        ParserClDataClass(const ParserClDataClass &);


        bool open(QString file_path,QString file_name);
        void close();
        bool parse();

};
///-----------------------------------------------------------------------------
#endif /// PARSERCLDATACLASS_H
///-----------------------------------------------------------------------------
