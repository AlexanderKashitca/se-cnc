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
    point->_x = _segment_cell->at(position)._x;
    point->_y = _segment_cell->at(position)._y;
    point->_z = _segment_cell->at(position)._z;
    point->_a = _segment_cell->at(position)._a;
    point->_b = _segment_cell->at(position)._b;
    point->_c = _segment_cell->at(position)._c;
    return(0);
}
///-----------------------------------------------------------------------------
