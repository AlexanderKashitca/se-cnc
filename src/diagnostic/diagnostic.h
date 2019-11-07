///---------
/// --------------------------------------------------------------------
#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H
///-----------------------------------------------------------------------------
#include "../dbus/client/client.h"
///-----------------------------------------------------------------------------
namespace DIAGNOSTIC_SPACE
{
    using namespace DBUS_SPACE;

    class diagnosticClass
    {
        private :
            dbusClientClass* _dbus;
            QVector<bool> _input_state;
            QVector<bool> _output_state;
        public :
            diagnosticClass();
            ~diagnosticClass();

            bool initialization();
            bool getIO();
    };
} /// end namespace DIAGNOSTIC_SPACE
///-----------------------------------------------------------------------------
#endif /// DIAGNOSTIC_H
///-----------------------------------------------------------------------------
