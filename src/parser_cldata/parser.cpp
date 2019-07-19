///-----------------------------------------------------------------------------
#include "parser.h"
///-----------------------------------------------------------------------------
#include <QDebug>

ParserClDataClass::ParserClDataClass()
{
    _file_in  = new QFile;
}
///-----------------------------------------------------------------------------
ParserClDataClass::~ParserClDataClass()
{
    delete _file_in;
}
///-----------------------------------------------------------------------------
void ParserClDataClass::close()
{
    _file_in->close();
}
///-----------------------------------------------------------------------------
bool ParserClDataClass::open(QString file_path,QString file_name)
{
    QString file_name_in;

    _filename = file_name;
    file_name_in  = file_path + "/" + file_name;

    if(!QFile::exists(file_name_in))
    {
        return(false);
    }
    _file_in->setFileName(file_name_in);
    if(!_file_in->open(QIODevice::ReadOnly))
    {
        return(false);
    }
    return(true);
}
///-----------------------------------------------------------------------------
bool ParserClDataClass::parse()
{
    QString line;
    QString command;
    QTextStream in(_file_in);



    while(!in.atEnd())
    {
        line = in.readLine();
        qDebug() << line << endl;

        for(int i = 0;i < command_general->size();i++)
        {
            command = command_general[i];
            if(line.contains(command))
            {
                commandGeneral(i);
            }

        }




    }

    return(true);
}
///-----------------------------------------------------------------------------

