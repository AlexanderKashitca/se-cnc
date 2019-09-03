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

#if 0

QVector<QVector3D> vector;
QVector<QVector3D> segment;
vector.clear();

QVector3D vector3d;
float _x;
float _y;
float _z;





///    for(int i = 0;i < 10;i++)
///    {
///        vector3d.setX(point);
///        vector3d.setY(point);
///        vector3d.setZ(point);
///        point = point + 0.1F;
///        vector.push_back(vector3d);
///    }







/// segment // vector 3d
/// vector  // vector 3d




    SEGMENT_TYPE _seg_type(SEG_LINEAR);


    double _liner_end_x;
    double _liner_end_y;
    double _liner_end_z;

    double _arc_radius_begin;
    double _arc_radius_end;
    double _arc_begin_x;
    double _arc_begin_y;
    double _arc_end_x;
    double _arc_end_y;
    double _arc_center_x;
    double _arc_center_y;
    double _arc_center_i;
    double _arc_center_j;





    _arc_begin_x =  142.0; /// X begin
    _arc_begin_y = -2.628; /// Y begin

    _arc_end_x =  140.0; /// X end
    _arc_end_y = -3.089; /// Y end

    _arc_center_i =  10.905; /// I
    _arc_center_j = -42.374; /// J

    _arc_center_x = _arc_begin_x + _arc_center_i; /// X center position
    _arc_center_y = _arc_begin_y + _arc_center_j; /// Y center position

    _arc_radius_begin = fabs(_arc_center_x - _arc_begin_x);
    _arc_radius_end   = fabs(_arc_center_x - _arc_end_x);

    qDebug() << endl;
    qDebug() << "==========================================";
    qDebug() << "_arc_begin_x = "      << _arc_begin_x;
    qDebug() << "_arc_begin_y = "      << _arc_begin_y;
    qDebug() << "_arc_end_x = "        << _arc_end_x;
    qDebug() << "_arc_end_y = "        << _arc_end_y;
    qDebug() << "_arc_center_i = "     << _arc_center_i;
    qDebug() << "_arc_center_j = "     << _arc_center_j;
    qDebug() << "_arc_center_x = "     << _arc_center_x;
    qDebug() << "_arc_center_y = "     << _arc_center_y;
    qDebug() << "_arc_radius_begin = " << _arc_radius_begin;
    qDebug() << "_arc_radius_end   = " << _arc_radius_end;
    qDebug() << "==========================================";


    QVector2D vector_2d;
    vector_2d.setX(static_cast<float>(_arc_end_x));
    vector_2d.setY(static_cast<float>(_arc_end_y));
    qDebug() << "2D length" << vector_2d.length();


    qDebug() << "2D distance - " << vector_2d.distanceToPoint(QVector2D(10,10));




    switch(_seg_type)
    {
        case SEG_ARC :
            break;
        default :
            break;
    }
#endif

    PlannerClass planner;
    PLANNER_STATE planner_state;
    SegmentPoint  point;
    planner_state = planner.moveStraight(10.219449,1.9843,2,250);
    qDebug() << "planner state - " << planner_state;
    if(planner_state == PLANNER_OK)
    {
        QVector<QVector3D> vector3d;
        form.plotAddData(planner._coord_vector);
        form.plotShow(true);
    }


//    form.plotAddData(vector);
//    form.plotShow(true);

    // ********************************************************************
    // Deleting the objects of the Reflexxes Motion Library end terminating
    // the process



    return app.exec();
}
