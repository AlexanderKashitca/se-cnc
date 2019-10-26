///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include "../../../src/diagnostic/diagnostic.h"
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DIAGNOSTIC_SPACE::diagnosticClass* diagnistic;

    diagnistic  = new DIAGNOSTIC_SPACE::diagnosticClass;
    diagnistic->initialization();
    diagnistic->connect(SERVICE_NAME);

    const QString query = "ping";
    QByteArray answer = "";
    QByteArray data = "";
   // diagnistic->sendQuery(query,answer,data);

    return a.exec();

}
///-----------------------------------------------------------------------------
