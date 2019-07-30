///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include <QDebug>
///-----------------------------------------------------------------------------
#include "../../../src/parser/parser.h"
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);


    ParserDataClass parser;

    QString path = "/home/evil/Programming/Qt/se-cnc/bin/prj/parser";
    QString name = "rs274ngc.out";

    if(parser.open(path,name))
    {
        parser.parse();
        parser.close();
    }
    else
    {
        qDebug() << "Error opened " << name << " file" << endl;
    }

    return app.exec();
}
///-----------------------------------------------------------------------------
