///-----------------------------------------------------------------------------
#include "segmentation.h"
///-----------------------------------------------------------------------------
using namespace SEGMENTATION_SPACE;
///-----------------------------------------------------------------------------
SegmentationClass::SegmentationClass()
{
    _segment_cell = new QVector<SEGMENT_POINT>;

    _segment = new QVector<QVector<SEGMENT_POINT>>;

    _segment_cell->clear();
}
///-----------------------------------------------------------------------------
SegmentationClass::~SegmentationClass()
{
    delete[] _segment_cell;
    delete[] _segment;
}
///-----------------------------------------------------------------------------
SEGMENT_STATE SegmentationClass::clear()
{
    _segment_cell->clear();
    _segment->clear();
    return(SEGMENT_OK);
}
///-----------------------------------------------------------------------------
SEGMENT_STATE SegmentationClass::appendPoint(SEGMENT_POINT& point)
{
    _segment_cell->append(point);
    return(SEGMENT_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief SegmentationClass::appendSegmentCell
 * @note append data from cell segment to segment
 */
SEGMENT_STATE SegmentationClass::appendSegmentCell()
{
    _segment->append(*_segment_cell);
    _segment_cell->clear();
    return(SEGMENT_OK);
}
///-----------------------------------------------------------------------------
int SegmentationClass::size()
{
    return(_segment->size());
}
///-----------------------------------------------------------------------------
SEGMENT_STATE SegmentationClass::getPoint(int segment,int position,SEGMENT_POINT* point)
{
    if(segment >= _segment->length())
        return(SEGMENT_FAIL);
    if(position >= _segment_cell->length())
        return(SEGMENT_FAIL);
    point->_x     = _segment->at(segment).at(position)._x;
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
    return(SEGMENT_OK);
}
///-----------------------------------------------------------------------------
