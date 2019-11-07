///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include "../../../src/diagnostic/diagnostic.h"
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DIAGNOSTIC_SPACE::diagnosticClass* diagnistic;

    diagnistic  = new DIAGNOSTIC_SPACE::diagnosticClass;

    if(diagnistic->initialization())
    {
        qDebug() << "init successful";
        diagnistic->getIO();
    }



    return a.exec();

}
///-----------------------------------------------------------------------------
