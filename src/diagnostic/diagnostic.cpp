///-----------------------------------------------------------------------------
#include "diagnostic.h"
///-----------------------------------------------------------------------------
DIAGNOSTIC_SPACE::diagnosticClass::diagnosticClass()
{
    _dbus  = new dbusClientClass;
}
///-----------------------------------------------------------------------------
DIAGNOSTIC_SPACE::diagnosticClass::~diagnosticClass()
{
    delete[] _dbus;
}
///-----------------------------------------------------------------------------
bool DIAGNOSTIC_SPACE::diagnosticClass::initialization()
{
    _input_state.clear();
    _output_state.clear();
    if(!_dbus->initialization())
        return(false);
    if(!_dbus->connect(SERVICE_NAME))
        return(false);
    return(true);
}
///-----------------------------------------------------------------------------
bool DIAGNOSTIC_SPACE::diagnosticClass::getIO()
{
    QString query = "pingfdghd";
    QList<QVariant> args;
    args.clear();
    args.append("ping");


    _dbus->sendQuery(query,args);


    return(true);
}
///-----------------------------------------------------------------------------
