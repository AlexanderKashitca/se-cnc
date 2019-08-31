///-----------------------------------------------------------------------------
#ifndef SEGMENTATION_H
#define SEGMENTATION_H
///-----------------------------------------------------------------------------
#include <QPair>
#include <QVector>
///-----------------------------------------------------------------------------
namespace SEGMENTATION_SPACE
{


    /**
     *  @brief segment type
     */
    typedef enum
    {
        SEG_UNDEFINED = 0,
        SEG_LINEAR,
        SEG_ARC,
        SEG_RAPID,
        SEG_DWELL
    }SEGMENT_TYPE;
    /**
     *  @brief point
     */
    class SegmentPoint
    {
        public :
            double _x;
            double _y;
            double _z;
            double _a;
            double _b;
            double _c;
    };
    /**
     * @brief The SegmentationClass class
     */
    class SegmentationClass
    {
        private :
            QVector<SegmentPoint>* _segment_cell;
        public :
            SegmentationClass();
            ~SegmentationClass();

            SEGMENT_TYPE _type;
            void clear();
            void appendPoint(SegmentPoint& point);
            int  getPoint(int position,SegmentPoint* point);
            int  size();
    };
}
///-----------------------------------------------------------------------------
#endif /// SEGMENTATION_H
///-----------------------------------------------------------------------------
