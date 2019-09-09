#include <QApplication>
#include "mainform.h"
#include <QDebug>


#include "../../../../src/planner/planner.h"
using namespace PLANNER_SPACE;
using namespace SEGMENTATION_SPACE;
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mainForm form;
    form.show();

//    form.plotAddData(vector);
//    form.plotShow(true);

    return app.exec();
}
///-----------------------------------------------------------------------------
