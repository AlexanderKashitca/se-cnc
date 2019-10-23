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
    WIDGET_HIDE           = 0,
    WIDGET_PROGRAM_CODE   = 1,
    WIDGET_DIAGNOSTIC_IO  = 2,
    WIDGET_MANUAL_CONTROL = 3,
    WIDGET_MAINTENANCE    = 4
}WIDGET_TYPE;
///-----------------------------------------------------------------------------
class SeMainWindow : public QMainWindow
{
    Q_OBJECT
    public:
        explicit SeMainWindow(QWidget *parent = nullptr);
        ~SeMainWindow();


    private slots :
        void menuActionOpen();
        void menuActionClose();
        void menuActionMaintenance();
        void menuActionDiagnisticIO();
        void menuActionManualControl();

        void variablePathToolButtonAction();
        void settingsPathToolButtonAction();
        void toolTablePathToolButtonAction();

private:
        Ui::SeMainWindow *ui;
        void widgetShow(WIDGET_TYPE type);
        QFile   _file;
        QString _filename;
        QString _filetype;
        QFileDialog* _filedlg;

};
///-----------------------------------------------------------------------------
#endif /// SEMAINWINDOW_H
///-----------------------------------------------------------------------------
