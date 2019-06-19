///-----------------------------------------------------------------------------
#ifndef HI_RES_TIMER_H
#define HI_RES_TIMER_H
///-----------------------------------------------------------------------------
#include <QString>
#include <QElapsedTimer>
///-----------------------------------------------------------------------------
#define MAX_TIMES 100
///-----------------------------------------------------------------------------
//typedef union _LARGE_INTEGER
//{
//  struct {
//    uint32_t LowPart;
//    int32_t  HighPart;
//  } p;
//  struct {
//    uint32_t LowPart;
//    int32_t  HighPart;
//  } u;
//  int64_t QuadPart;
//} LARGE_INTEGER, *PLARGE_INTEGER;

///-----------------------------------------------------------------------------
class CHiResTimer  
{
    public:
        CHiResTimer();
        virtual ~CHiResTimer();
        QElapsedTimer   Timer;

        void    Start();                    /// start timer
        double  DiffSecs(int t1,int t0);    /// difference t1 and t2 at secconds
        double  Diff_us(int t1,int t0);     /// difference t1 and t2 at microsecconds

        void    DisplayDiff(int t1,int t0); ///
        double  Elapsed_Seconds();          /// elapsed time from start in seconds

        void    Split();                    /// grab a split time measurement
        void    Split(QString s);           /// grab a split time measurement
        void    DisplaySplit();
        qint64  Times[MAX_TIMES];           /// nanoseconds from startig timer
        QString Desc[MAX_TIMES];
        int     nSplit;
        bool    MessageDisplayed;
};
///-----------------------------------------------------------------------------
#endif /// HI_RES_TIMER_H
///-----------------------------------------------------------------------------
