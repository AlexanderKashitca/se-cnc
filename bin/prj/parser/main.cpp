///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include <QDebug>
///-----------------------------------------------------------------------------
#include "../../../src/parser/parser.h"
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);



    PARSER_SPACE::ParserDataClass parser;

    QString path = "/home/evil/Programming/Qt/se-cnc/bin/prj/parser";
    QString name = "rs274ngc.out";




    if(parser.execut(path,name) == PARSER_SPACE::PARESER_STATE::PARESER_OK)
    {
        qDebug() << "Parser executing file - " << name << " successful";
    }
    else
    {
        qDebug() << "Error opened " << name << " file" << endl;
    }

    return app.exec();
}
///-----------------------------------------------------------------------------
