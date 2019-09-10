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

    PlannerClass planner;

    planner._segment_point._x = 1;
    planner._segment_point._y = 2;
    planner._segment_point._z = 3;
    planner._segment->appendPoint(planner._segment_point);
    planner._segment_point._x = 10;
    planner._segment_point._y = 20;
    planner._segment_point._z = 30;
    planner._segment->appendPoint(planner._segment_point);
    planner._segment->appendSegmentCell();
    planner._segment_point._x = 210;
    planner._segment_point._y = 220;
    planner._segment_point._z = 230;
    planner._segment->appendPoint(planner._segment_point);
    planner._segment->appendSegmentCell();

    qDebug() << "_segment size - " << planner._segment->size();

//    form.plotAddData(vector);
//    form.plotShow(true);

    return app.exec();
}
///-----------------------------------------------------------------------------
