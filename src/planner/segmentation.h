///-----------------------------------------------------------------------------
#ifndef SEGMENTATION_H
#define SEGMENTATION_H
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
     * @brief The SegmentationClass class
     */
    class SegmentationClass
    {
        public:
            SegmentationClass();

            SEGMENT_TYPE _type;
            void clear();
            int sappend();
            int getLast();
    };
}
///-----------------------------------------------------------------------------
#endif /// SEGMENTATION_H
///-----------------------------------------------------------------------------
