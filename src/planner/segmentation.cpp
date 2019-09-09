///-----------------------------------------------------------------------------
#include "segmentation.h"
///-----------------------------------------------------------------------------
using namespace SEGMENTATION_SPACE;
///-----------------------------------------------------------------------------
SegmentationClass::SegmentationClass()
{
    _segment_cell = new QVector<SEGMENT_POINT>;
    _segment_cell->clear();
}
///-----------------------------------------------------------------------------
SegmentationClass::~SegmentationClass()
{
    delete[] _segment_cell;
}
///-----------------------------------------------------------------------------
void SegmentationClass::clear()
{
    _segment_cell->clear();
}
///-----------------------------------------------------------------------------
void SegmentationClass::appendPoint(SEGMENT_POINT& point)
{
    _segment_cell->append(point);
}
///-----------------------------------------------------------------------------
int SegmentationClass::size()
{
    return(_segment_cell->size());
}
///-----------------------------------------------------------------------------
int SegmentationClass::getPoint(int position,SEGMENT_POINT* point)
{
    if(position >= _segment_cell->length())
        return(1);
    point->_x     = _segment_cell->at(position)._x;
    point->_y     = _segment_cell->at(position)._y;
    point->_z     = _segment_cell->at(position)._z;
    point->_a     = _segment_cell->at(position)._a;
    point->_b     = _segment_cell->at(position)._b;
    point->_c     = _segment_cell->at(position)._c;
    point->_vel_x = _segment_cell->at(position)._vel_x;
    point->_vel_y = _segment_cell->at(position)._vel_y;
    point->_vel_z = _segment_cell->at(position)._vel_z;
    point->_vel_a = _segment_cell->at(position)._vel_a;
    point->_vel_b = _segment_cell->at(position)._vel_b;
    point->_vel_c = _segment_cell->at(position)._vel_c;
    return(0);
}
///-----------------------------------------------------------------------------
