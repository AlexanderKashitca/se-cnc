///-----------------------------------------------------------------------------
#ifndef HI_RES_TIMER_H
#define HI_RES_TIMER_H
///-----------------------------------------------------------------------------
#include <QElapsedTimer>
#include <QString>
///-----------------------------------------------------------------------------
#define MAX_TIMES 100
///-----------------------------------------------------------------------------
class HiResTimerClass
{
    public:
        HiResTimerClass();
        virtual ~HiResTimerClass();
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
