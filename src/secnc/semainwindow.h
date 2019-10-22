///-----------------------------------------------------------------------------
#ifndef SEMAINWINDOW_H
#define SEMAINWINDOW_H
///-----------------------------------------------------------------------------
#include <QMainWindow>
#include <QtWidgets>
///-----------------------------------------------------------------------------
namespace Ui
{
    class SeMainWindow;
}

typedef enum
{
    WIDGET_HIDE          = 0,
    WIDGET_PROGRAM_CODE  = 1,
    WIDGET_DIAGNOSTIC_IO = 2
}WIDGET_TYPE;
///-----------------------------------------------------------------------------
class SeMainWindow : public QMainWindow
{
    Q_OBJECT
    public:
        explicit SeMainWindow(QWidget *parent = nullptr);
        ~SeMainWindow();


    public slots :
        void menuActionOpen();
        void menuActionClose();
        void menuActionMaintenance();
        void menuActionDiagnisticIO();
private slots:

private:
        Ui::SeMainWindow *ui;
        void widgetShow(WIDGET_TYPE type);

};
///-----------------------------------------------------------------------------
#endif /// SEMAINWINDOW_H
///-----------------------------------------------------------------------------
