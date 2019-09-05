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
    _debug = true;
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
PLANNER_STATE PlannerClass::calcStraightVelocity(double velocity)
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
    if(_debug)
    {
        qDebug() << "_velocity_x = " << _velocity_x;
        qDebug() << "_velocity_y = " << _velocity_y;
        qDebug() << "_velocity_z = " << _velocity_z;
    }
    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------
PLANNER_STATE PlannerClass::calcStraightLength(double x_begin,double y_begin,double z_begin,double x_end,double y_end,double z_end)
{
    /// cal delta axis
    _x_delta = fabs(fabs(x_end) - fabs(x_begin));
    _y_delta = fabs(fabs(y_end) - fabs(y_begin));
    _z_delta = fabs(fabs(z_end) - fabs(z_begin));
    /// calc length the vector
    _length = sqrt((pow(_x_delta,2)+pow(_y_delta,2)+pow(_z_delta,2)));
    if(_debug)
    {
        qDebug() << "_length = " << _length;
    }
    if(_length == 0.0)
        return(PLANNER_FAIL);
    /// calc cosinus algle each axis
    _cos_alpha = _x_delta / _length;
    _cos_beta  = _y_delta / _length;
    _cos_gamma = _z_delta / _length;
    if(_debug)
    {
        qDebug() << "_cos_alpha = " << _cos_alpha;
        qDebug() << "_cos_beta  = " << _cos_beta;
        qDebug() << "_cos_gamma = " << _cos_gamma;
    }
    /// set ortogonale vectors
    if(qAbs(x_begin) < qAbs(x_end))
        _current_i = 1.0;
    else
        _current_i = -1.0;
    if(qAbs(y_begin) < qAbs(y_end))
        _current_j = 1.0;
    else
        _current_j = -1.0;
    if(qAbs(z_begin) < qAbs(z_end))
        _current_k = 1.0;
    else
        _current_k = -1.0;

    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief moveStraightTraverse
 * @param x - end coordinate axis x
 * @param y - end coordinate axis y
 * @param z - end coordinate axis z
 * @param feed - feed to new point
 * @note Now realizate three axis
 * @details liner moving from current position to new setting
 */
PLANNER_STATE PlannerClass::moveStraight(double x,double y,double z,double feed)
{
    double _length_n;
    double _x_n;
    double _y_n;
    double _z_n;

    if(calcStraightLength(_current_x,_current_y,_current_z,x,y,z) != PLANNER_OK)
        return(PLANNER_FAIL);
    if(calcStraightVelocity(feed) != PLANNER_OK)
        return(PLANNER_FAIL);

    _length_n = _length_delta;
    while(_length_n <= _length)
    {
        _x_n = _length_n * _cos_alpha;
        _y_n = _length_n * _cos_beta;
        _z_n = _length_n * _cos_gamma;
        /// push coordinate each axis to vector
        _coordinate.setX(static_cast<float>(_x_n));
        _coordinate.setY(static_cast<float>(_y_n));
        _coordinate.setZ(static_cast<float>(_z_n));
        _coord_vector.push_back(_coordinate);
        if(_debug)
        {
            qDebug() << "_length_n = " << _length_n << "_x_n = " << _x_n << " : _y_n = " << _y_n << " : _z_n = " << _z_n;
        }
        _length_n += _length_delta;
    }
    _length_n = _length;
    _x_n = _length_n * _cos_alpha;
    _y_n = _length_n * _cos_beta;
    _z_n = _length_n * _cos_gamma;
    /// push coordinate each axis to vector
    _coordinate.setX(static_cast<float>(_x_n));
    _coordinate.setY(static_cast<float>(_y_n));
    _coordinate.setZ(static_cast<float>(_z_n));
    _coord_vector.push_back(_coordinate);
    if(_debug)
        qDebug() << "_length_n = " << _length_n << "_x_n = " << _x_n << " : _y_n = " << _y_n << " : _z_n = " << _z_n;

    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------
PLANNER_STATE PlannerClass::moveArc(INTERPRETER_SPACE::CANON_PLANE plane,double x,double y,double z,double i,double j,double k,double feed)
{
    double _vertical_begin;
    double _ortogonal_begin;
    double _horizontal_begin;

    double _vertical_end;
    double _ortogonal_end;
    double _horizontal_end;

    double _vertical_centr;
    double _horizontal_centr;

    double _coordinate_vertical_centr;
    double _coordinate_horizontal_centr;

    double _vertical_delta;
    double _horizontal_delta;

    double _radius_vector_length;


    switch(plane)
    {
        case INTERPRETER_SPACE::CANON_PLANE::CANON_PLANE_XY :
            _ortogonal_end    = z;
            _ortogonal_begin  = _current_z;
            _horizontal_begin = _current_x;
            _horizontal_end   = x;
            _vertical_begin   = _current_y;
            _vertical_end     = y;
            _horizontal_centr = i;
            _vertical_centr   = j;
            break;
        case INTERPRETER_SPACE::CANON_PLANE::CANON_PLANE_YZ :
            _ortogonal_end    = x;
            _ortogonal_begin  = _current_x;
            _horizontal_begin = _current_z;
            _horizontal_end   = z;
            _vertical_begin   = _current_y;
            _vertical_end     = y;
            _horizontal_centr = k;
            _vertical_centr   = j;
            break;
        case INTERPRETER_SPACE::CANON_PLANE::CANON_PLANE_XZ :
            _ortogonal_end    = y;
            _ortogonal_begin  = _current_y;
            _horizontal_begin = _current_x;
            _horizontal_end   = x;
            _vertical_begin   = _current_z;
            _vertical_end     = z;
            _horizontal_centr = i;
            _vertical_centr   = k;
            break;
    }
    /// validation variables
    if(qFuzzyCompare(_horizontal_begin,_horizontal_end))
        return(PLANNER_INVALID_PARAMETER);
    if(qFuzzyCompare(_vertical_begin,_vertical_end))
        return(PLANNER_INVALID_PARAMETER);
    /// calculation delta
    _vertical_delta   = fabs(_vertical_centr);
    _horizontal_delta = fabs(_horizontal_centr);
    /// calc radius length the plane vector
    _radius_vector_length = sqrt((pow(_vertical_delta,2)+pow(_horizontal_delta,2)));
    /// calc center coordinate
    _coordinate_vertical_centr   = _vertical_begin + _vertical_centr;
    _coordinate_horizontal_centr = _horizontal_begin + _horizontal_centr;
    if(_debug)
    {
        qDebug() << "_vertical_delta              - " << _vertical_delta;
        qDebug() << "_horizontal_delta            - " << _horizontal_delta;
        qDebug() << "_radius_vector_length        - " << _radius_vector_length;
        qDebug() << "_coordinate_vertical_centr   - " << _coordinate_vertical_centr;
        qDebug() << "_coordinate_horizontal_centr - " << _coordinate_horizontal_centr;
    }

    /// comparing ortogonal moving
    if(qFuzzyCompare(_ortogonal_begin,_ortogonal_end))
    { /// no ortogonale moving. Only in the plane moving
        if(_horizontal_begin > _horizontal_end)
        { /// II and III
            if(_vertical_begin > _vertical_end)
            { /// III
                /// find begin and end angle

            }
            else
            { /// II
                /// find begin and end angle

            }
        }
        else
        { /// I and IV
            if(_vertical_begin > _vertical_end)
            { /// IV
                /// find begin and end angle

            }
            else
            { /// I
                /// find begin and end angle

            }
        }
    }

    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------
