///-----------------------------------------------------------------------------
#ifndef WIDGET_H
#define WIDGET_H
///-----------------------------------------------------------------------------
#include <QWidget>
#include <QGraphicsScene>
#include <QtDataVisualization>
///-----------------------------------------------------------------------------
namespace Ui
{
    class Widget;
}
///-----------------------------------------------------------------------------
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void addData();

    ///void setVectorPoint(double x,double y,double z);

private slots:

    void on_testPushButton_clicked();

private:
    Ui::Widget *ui;
    QtDataVisualization::Q3DScatter *_graph;
    QGraphicsScene  *_scene;
    QGraphicsItem   *_item;
    QGraphicsView   *_view;
    QtDataVisualization::QScatterDataProxy *_proxy;
    QtDataVisualization::QScatter3DSeries *_series;

    QtDataVisualization::QAbstract3DSeries::Mesh _style;
    int     _fontSize;
    bool    _smooth;
    int     _itemCount;
    float   _curveDivider;
    QFont   _font;
};
///-----------------------------------------------------------------------------
#endif // WIDGET_H
///-----------------------------------------------------------------------------
