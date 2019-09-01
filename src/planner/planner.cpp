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
PLANNER_STATE PlannerClass::calcVelocity(double velocity)
{
    if(_length == 0.0)
    {
        return(PLANNER_FAIL);
    }
    _velocity = velocity;
    /// calculation velocity each axis
    _velocity_x = _velocity * _x_delta / _length;
    _velocity_y = _velocity * _y_delta / _length;
    _velocity_z = _velocity * _z_delta / _length;
    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------
PLANNER_STATE PlannerClass::calcLength(double x_begin,double y_begin,double z_begin,double x_end,double y_end,double z_end)
{
    /// cal delta axis
    _x_delta = fabs(x_end - x_begin);
    _y_delta = fabs(y_end - y_begin);
    _z_delta = fabs(z_end - z_begin);
    /// calc length the vector
    _length = sqrt((pow(_x_delta,2)+pow(_y_delta,2)+pow(_z_delta,2)));
    if(_length == 0.0)
        return(PLANNER_FAIL);
    /// calc cosinus algle each axis
    _cos_alpha = _x_delta / _length;
    _cos_beta  = _y_delta / _length;
    _cos_gamma = _z_delta / _length;
    return(PLANNER_OK);
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
PLANNER_STATE PlannerClass::moveStraightTraverse(double x,double y,double z,double feed)
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
    double _delta_x;
    double _delta_y;
    double _delta_z;
    double _length_move;
    double const _length_delta = 0.1;
    double _velocity(10.0);
    double _velocity_x;
    double _velocity_y;
    double _velocity_z;


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
    _delta_x = fabs(_end_x - _begint_x);
    _delta_y = fabs(_end_y - _begint_y);
    _delta_z = fabs(_end_z - _begint_z);
    /// calc length the vector
    _length_move = sqrt((pow(_delta_x,2)+pow(_delta_y,2)+pow(_delta_z,2)));
    /// calculation velocity each axis
    _velocity_x = _velocity * _delta_x / _length_move;
    _velocity_y = _velocity * _delta_y / _length_move;
    _velocity_z = _velocity * _delta_z / _length_move;


    _delta_x = _delta_x * _i;
    _delta_y = _delta_y * _j;
    _delta_z = _delta_z * _k;

    qDebug() << "_velocity = " << _velocity;
    qDebug() << "_velocity_x = " << _velocity_x;
    qDebug() << "_velocity_y = " << _velocity_y;
    qDebug() << "_velocity_z = " << _velocity_z;

    qDebug() << "_delta_x = " << _delta_x;
    qDebug() << "_delta_y = " << _delta_y;
    qDebug() << "_delta_z = " << _delta_z;
    qDebug() << "_i = " << _i;
    qDebug() << "_j = " << _j;
    qDebug() << "_k = " << _k;
    qDebug() << "_begint_x = " << _begint_x;
    qDebug() << "_begint_y = " << _begint_y;
    qDebug() << "_begint_z = " << _begint_z;
    qDebug() << "_end_x = " << _end_x;
    qDebug() << "_end_y = " << _end_y;
    qDebug() << "_end_z = " << _end_z;
    qDebug() << "_length = " << _length_move;

    //vector.push_back();


    //point._x = ;
    //point._y = ;
    //point._z = ;


    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------



