///-----------------------------------------------------------------------------
#ifndef MAINFORM_H
#define MAINFORM_H
///-----------------------------------------------------------------------------
#include <QWidget>
#include <QVector3D>
#include <QGraphicsScene>
#include <QtDataVisualization>
///-----------------------------------------------------------------------------
namespace Ui
{
    class mainForm;
}
///-----------------------------------------------------------------------------
class mainForm : public QWidget
{
        Q_OBJECT
    private:
        Ui::mainForm *ui;
        QGraphicsScene  *_scene;
        QtDataVisualization::Q3DScatter *_graph;
        QtDataVisualization::QAbstract3DSeries::Mesh _style;
        QtDataVisualization::QScatterDataProxy *_proxy;
        QtDataVisualization::QScatter3DSeries  *_series;
        QtDataVisualization::QScatterDataArray *_dataArray;
        QFont   _font;

        int     _fontSize;
        bool    _smooth;
        int     _itemCount;
        float   _curveDivider;

        void plotCreate();

    public:
        explicit mainForm(QWidget *parent = nullptr);
        ~mainForm();

        void plotShow(bool);
        void plotAddData();
        void plotAddData(QVector<QVector3D> vector3d);
        void plotAddPoint(QVector3D point3d);
        void plotClear();

private slots:
        void on_showPlanner3D_clicked();
        void on_closePlanner3D_clicked();
        void on_clearPlaner3D_clicked();
};
///-----------------------------------------------------------------------------
#endif /// MAINFORM_H
///-----------------------------------------------------------------------------
