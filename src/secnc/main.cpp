///-----------------------------------------------------------------------------
#include "semainwindow.h"
#include <QApplication>
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    SeMainWindow mainForm;



    mainForm.setWindowTitle("MAIN WINDOWS SE CNC");
    //mainForm.cursor().setPos(500,500);

    //mainForm.setAutoFillBackground(true);
    mainForm.resize(300,300);

    mainForm.show();

    return app.exec();
}
///-----------------------------------------------------------------------------
