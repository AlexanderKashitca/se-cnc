///-----------------------------------------------------------------------------
#ifndef PARSER_G_CODE_H
#define PARSER_G_CODE_H
///-----------------------------------------------------------------------------
#include <QFile>
#include <QTextStream>
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
#endif /// PARSER_G_CODE_H
///-----------------------------------------------------------------------------
