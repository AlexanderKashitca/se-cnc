///-----------------------------------------------------------------------------
#include "planner.h"

#include <QDebug>
#include <QtMath>
///-----------------------------------------------------------------------------
using namespace PLANNER_SPACE;
///-----------------------------------------------------------------------------
PlannerClass::PlannerClass()
{
    _current_x = 0.0;
    _current_y = 0.0;
    _current_z = 0.0;
    _current_a = 0.0;
    _current_b = 0.0;
    _current_c = 0.0;
}
///-----------------------------------------------------------------------------
PlannerClass::~PlannerClass()
{

}
///-----------------------------------------------------------------------------
void PlannerClass::setDebug(bool enable)
{
    _debug = enable;
}
///-----------------------------------------------------------------------------
bool PlannerClass::getDebug()
{
    return(_debug);
}
///-----------------------------------------------------------------------------
/**
 * @brief moveStraightTraverse
 * @param x - end coordinate axis x
 * @param y - end coordinate axis y
 * @param z - end coordinate axis z
 * @param a - end coordinate axis a
 * @param b - end coordinate axis b
 * @param c - end coordinate axis c
 * @param feed - feed to new point
 * @note Now realizate three axis
 * @details liner moving from current position to new setting
 */
PLANNER_STATE PlannerClass::moveStraightTraverse(double x,double y,double z,double a,double b,double c,double feed)
{
    double _i;
    double _j;
    double _k;
    double _end_x(0);
    double _end_y(0);
    double _end_z(0);
    double _begint_x(5);
    double _begint_y(10);
    double _begint_z(15);
    double _x;
    double _y;
    double _z;
    double _length;

    /// set ortogonale vectors
    if(qAbs(_begint_x) < qAbs(_end_x))
        _i = 1.0;
    else
        _i = -1.0;
    if(qAbs(_begint_y) < qAbs(_end_y))
        _j = 1.0;
    else
        _j = -1.0;
    if(qAbs(_begint_z) < qAbs(_end_z))
        _k = 1.0;
    else
        _k = -1.0;
    /// cal delta axis
    _x = fabs(_end_x) - fabs(_begint_x);
    _y = fabs(_end_y) - fabs(_begint_y);
    _z = fabs(_end_z) - fabs(_begint_z);
    _x = _x * _i;
    _y = _y * _j;
    _z = _z * _k;
    /// calc length the vector
    _length = sqrt((pow(_x,2)+pow(_y,2)+pow(_z,2)));


    qDebug() << "_x = " << _x;
    qDebug() << "_y = " << _y;
    qDebug() << "_z = " << _z;
    qDebug() << "_i = " << _i;
    qDebug() << "_j = " << _j;
    qDebug() << "_k = " << _k;
    qDebug() << "_begint_x = " << _begint_x;
    qDebug() << "_begint_y = " << _begint_y;
    qDebug() << "_begint_z = " << _begint_z;
    qDebug() << "_end_x = " << _end_x;
    qDebug() << "_end_y = " << _end_y;
    qDebug() << "_end_z = " << _end_z;
    qDebug() << "_length = " << _length;

    //vector.push_back();


    //point._x = ;
    //point._y = ;
    //point._z = ;


    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------



