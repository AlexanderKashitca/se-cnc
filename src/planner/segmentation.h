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
     *  @brief segment point
     */
    typedef struct
    {
         double _x;
         double _y;
         double _z;
         double _a;
         double _b;
         double _c;
         double _vel_x;
         double _vel_y;
         double _vel_z;
         double _vel_a;
         double _vel_b;
         double _vel_c;
    }SEGMENT_POINT;
    /**
     *  @brief segment state
     */
    typedef enum
    {
        SEGMENT_OK   = 0,
        SEGMENT_FAIL = 1
    }SEGMENT_STATE;

    /**
     * @brief The SegmentationClass class
     */
    class SegmentationClass
    {
        private :
            QVector<SEGMENT_POINT>* _segment_cell;
            QVector<QVector<SEGMENT_POINT>>* _segment;
        public :
            SegmentationClass();
            ~SegmentationClass();

            SEGMENT_TYPE _type;
            SEGMENT_STATE clear();
            SEGMENT_STATE appendPoint(SEGMENT_POINT& point);
            SEGMENT_STATE appendSegmentCell();
            SEGMENT_STATE getPoint(int segment,int position,SEGMENT_POINT* point);
            int size();
    };
}
///-----------------------------------------------------------------------------
#endif /// SEGMENTATION_H
///-----------------------------------------------------------------------------
