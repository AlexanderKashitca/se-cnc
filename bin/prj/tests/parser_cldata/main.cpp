///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include <QDebug>
///-----------------------------------------------------------------------------
#include "parser.h"
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ParserClDataClass parser;

    QString path = "/home/evil/Programming/Qt/se-cnc/bin/prj/parser_cldata";
    QString name = "cldata.apt";

    if(parser.open(path,name))
    {
        parser.parse();
        parser.close();
    }
    else
    {
        qDebug() << "Error opened cldata.apt file" << endl;
    }

    return a.exec();
}
///-----------------------------------------------------------------------------
