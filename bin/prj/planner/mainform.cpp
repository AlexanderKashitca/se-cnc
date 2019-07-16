///-----------------------------------------------------------------------------
#include "mainform.h"
#include "ui_mainform.h"

///-----------------------------------------------------------------------------
mainForm::mainForm(QWidget *parent) : QWidget(parent),ui(new Ui::mainForm)
{
    ui->setupUi(this);
    _dataArray = new QtDataVisualization::QScatterDataArray;
    _scene = new QGraphicsScene;

    _fontSize = 20.0f;
    _style = QtDataVisualization::QAbstract3DSeries::MeshSphere;
    _smooth = true;
    _itemCount = 900;
    _curveDivider = 0.75f;

    ///-------------------------------------------------------------------------
    _scene->setSceneRect(-250,-250,500,500);
    _scene->addLine(-250,0,250,0,QPen(Qt::blue));
    _scene->addLine(0,-250,0,250,QPen(Qt::blue));

    ui->mainForm::graphicsView->setGeometry(
                0,
                0,
                static_cast<int>(_scene->width()),
                static_cast<int>(_scene->height()));
    ui->mainForm::graphicsView->setScene(_scene);
    ///-------------------------------------------------------------------------
    plotCreate();
}
///-----------------------------------------------------------------------------
mainForm::~mainForm()
{
    delete ui;
    delete _graph;
}
///-----------------------------------------------------------------------------
void mainForm::plotCreate()
{
    _graph = new QtDataVisualization::Q3DScatter();
    ///-------------------------------------------------------------------------
    _font = _graph->activeTheme()->font();
    _font.setPointSize(_fontSize);
    ///-------------------------------------------------------------------------
    _graph->activeTheme()->setType(QtDataVisualization::Q3DTheme::ThemeEbony);
    _graph->activeTheme()->setFont(_font);
    _graph->activeTheme()->setGridEnabled(true);
    _graph->activeTheme()->setLightColor(Qt::white);
    _graph->activeTheme()->setWindowColor(Qt::white);
    _graph->activeTheme()->setGridLineColor(Qt::green);
    _graph->activeTheme()->setBackgroundColor(Qt::white);
    _graph->setShadowQuality(QtDataVisualization::QAbstract3DGraph::ShadowQualitySoftLow);
    _graph->scene()->activeCamera()->setCameraPreset(QtDataVisualization::Q3DCamera::CameraPresetFront);
    ///-------------------------------------------------------------------------
    _proxy = new QtDataVisualization::QScatterDataProxy;
    _series = new QtDataVisualization::QScatter3DSeries(_proxy);
    _series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    _series->setMeshSmooth(true);
    _graph->addSeries(_series);
    ///-------------------------------------------------------------------------
    _graph->setGeometry(0,0,1024,768);
    ///-------------------------------------------------------------------------
    _graph->axisX()->setTitle("X");
    _graph->axisY()->setTitle("Y");
    _graph->axisZ()->setTitle("Z");
}
///-----------------------------------------------------------------------------
void mainForm::plotShow(bool show)
{
    if(show)
        _graph->show();
    else
        _graph->close();
}
///-----------------------------------------------------------------------------
void mainForm::plotClear()
{
    _dataArray->resize(0);
    _graph->seriesList().at(0)->dataProxy()->resetArray(_dataArray);
}
///-----------------------------------------------------------------------------
void mainForm::plotAddPoint(QVector3D point3d)
{
    QtDataVisualization::QScatterDataItem *ptrTo_dataArray = &_dataArray->last();
    _itemCount = _dataArray->size();
    _itemCount++;
    _dataArray->resize(_itemCount);

    ptrTo_dataArray->setPosition(point3d);
    _graph->seriesList().at(0)->dataProxy()->resetArray(_dataArray);
}
///-----------------------------------------------------------------------------
void mainForm::plotAddData(QVector<QVector3D> vector3d)
{
    _itemCount = vector3d.size();

    _dataArray->resize(_itemCount);
    QtDataVisualization::QScatterDataItem *ptrTo_dataArray = &_dataArray->first();

    for(int i = 0;i < _itemCount;i++)
    {
        ptrTo_dataArray->setPosition(vector3d.at(i));
        ptrTo_dataArray++;
    }
    _graph->seriesList().at(0)->dataProxy()->resetArray(_dataArray);
}
///-----------------------------------------------------------------------------
void mainForm::plotAddData()
{
    _dataArray->resize(_itemCount);
    QtDataVisualization::QScatterDataItem *ptrTo_dataArray = &_dataArray->first();

#ifdef RANDOM_SCATTER
    for (int i = 0; i < _itemCount; i++) {
        QVector3D vector(
                    (float)(QRandomGenerator::global()->bounded(100)) / 2.0f   - (float)(QRandomGenerator::global()->bounded(100)) / 2.0f,
                    (float)(QRandomGenerator::global()->bounded(100)) / 100.0f - (float)(QRandomGenerator::global()->bounded(100)) / 100.0f,
                    (float)(QRandomGenerator::global()->bounded(100)) / 2.0f -   (float)(QRandomGenerator::global()->bounded(100)) / 2.0f);
        ptrTo_dataArray->setPosition(vector);
        ptrTo_dataArray++;
    }
#else
    float limit = qSqrt(_itemCount) / 2.0f;
    for(float i = -limit; i < limit; i++)
    {
        for(float j = -limit;j < limit;j++)
        {
            ptrTo_dataArray->setPosition(QVector3D(i + 0.5f,qCos(qDegreesToRadians((i * j) / _curveDivider)),j + 0.5f));
            ptrTo_dataArray++;
        }
    }
#endif
    _graph->seriesList().at(0)->dataProxy()->resetArray(_dataArray);
}
///-----------------------------------------------------------------------------
void mainForm::on_clearPlaner3D_clicked()
{
    plotClear();
}
///-----------------------------------------------------------------------------
void mainForm::on_showPlanner3D_clicked()
{
    plotAddData();
    plotShow(true);
}
///-----------------------------------------------------------------------------
void mainForm::on_closePlanner3D_clicked()
{
    plotShow(false);
}
///-----------------------------------------------------------------------------
