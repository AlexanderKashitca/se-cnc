///-----------------------------------------------------------------------------
#include "hirestimer.h"
///-----------------------------------------------------------------------------
HiResTimerClass::HiResTimerClass()
{
    nSplit = 0;
    MessageDisplayed = false;
}
///-----------------------------------------------------------------------------
HiResTimerClass::~HiResTimerClass()
{
}
///-----------------------------------------------------------------------------
void HiResTimerClass::Start()
{
    Timer.start();
    Times[0] = Timer.nsecsElapsed();
    nSplit = 1;
}
///-----------------------------------------------------------------------------
double HiResTimerClass::Diff_us(int t1, int t0)
{
    double result;
    qint64 T0;
    qint64 T1;

    T0 = Times[t0];
    T1 = Times[t1];
    result  = (T1 - T0);
    result *= 1e6;
    return result;
}
///-----------------------------------------------------------------------------
double HiResTimerClass::DiffSecs(int t1,int t0)
{
    double result;
    qint64 T0;
    qint64 T1;

    T0 = Times[t0];
    T1 = Times[t1];
    result  = (T1 - T0);
    result *= 1e9;
	return result;
}
///-----------------------------------------------------------------------------
void HiResTimerClass::Split()
{
    if(nSplit >= MAX_TIMES)
    {
        return;
    }
    Times[nSplit] = Timer.nsecsElapsed();
    Desc[nSplit] = "";
	nSplit++;
}
///-----------------------------------------------------------------------------
void HiResTimerClass::Split(QString s)
{
    if(nSplit >= MAX_TIMES)
    {
        return;
    }
    Times[nSplit] = Timer.nsecsElapsed();
    Desc[nSplit] = s;
	nSplit++;
}
///-----------------------------------------------------------------------------
double HiResTimerClass::Elapsed_Seconds()
{
    double result;
    qint64 T0;
    qint64 T1;

    T0 = Times[0];
    T1 = Timer.nsecsElapsed();
    result = static_cast<double>(T1 - T0);
	return result;
}
///-----------------------------------------------------------------------------
void HiResTimerClass::DisplayDiff(int t1, int t0)
{
    QString s;

    s = QString("From sample %d to %d was %f us")
        .arg(t1)
        .arg(t0)
        .arg(Diff_us(t1,t0));
    if(MessageDisplayed) return;
    MessageDisplayed = true;
    ///MessageBox(NULL,s,"Timer",MB_OK|MB_SYSTEMMODAL);
    MessageDisplayed = false;
}
///-----------------------------------------------------------------------------
void HiResTimerClass::DisplaySplit()
{
    QString s;
    QString s2;

    for(int i=0;i < nSplit - 1;i++)
	{
        s = QString("From sample %d to %d was %10.2f us %s")
            .arg(i+1)
            .arg(i)
            .arg(Diff_us(i+1,i))
            .arg(Desc[i+1]);
        s2 = s2 + s + "\r";
	}
    if(MessageDisplayed) return;
	MessageDisplayed=true;
///	MessageBox(NULL,s2,"Timer",MB_OK|MB_SYSTEMMODAL);
	MessageDisplayed=false;
}
///-----------------------------------------------------------------------------
