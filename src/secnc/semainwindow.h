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
///-----------------------------------------------------------------------------
class SeMainWindow : public QMainWindow
{
    Q_OBJECT
    public:
        explicit SeMainWindow(QWidget *parent = nullptr);
        ~SeMainWindow();
        QHBoxLayout* pbxLayout;
    private:
        Ui::SeMainWindow *ui;

};
///-----------------------------------------------------------------------------
#endif /// SEMAINWINDOW_H
///-----------------------------------------------------------------------------
