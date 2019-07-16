///-----------------------------------------------------------------------------
#include "widget.h"
#include "ui_widget.h"
///-----------------------------------------------------------------------------
#include <QDebug>
///-----------------------------------------------------------------------------
Widget::Widget(QWidget *parent) : QWidget(parent),ui(new Ui::Widget)
{
    ///-------------------------------------------------------------------------
    ui->setupUi(this);
    _graph = new QtDataVisualization::Q3DScatter();
    ///-------------------------------------------------------------------------
    _font = _graph->activeTheme()->font();
    _font.setPointSize(_fontSize);
    ///-------------------------------------------------------------------------
    _graph->activeTheme()->setType(QtDataVisualization::Q3DTheme::ThemeEbony);
    _graph->activeTheme()->setFont(_font);
    _graph->setShadowQuality(QtDataVisualization::QAbstract3DGraph::ShadowQualitySoftLow);
    _graph->scene()->activeCamera()->setCameraPreset(QtDataVisualization::Q3DCamera::CameraPresetFront);
    ///-------------------------------------------------------------------------
    _proxy = new QtDataVisualization::QScatterDataProxy;
    _series = new QtDataVisualization::QScatter3DSeries(_proxy);
    _series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    _series->setMeshSmooth(true);
    _graph->addSeries(_series);
    ///-------------------------------------------------------------------------
    addData();


    ///-------------------------------------------------------------------------
    _scene = new QGraphicsScene;

///    ui->mainWidget = QWidget::createWindowContainer(_graph);

    ui->plot->


    //    ui->Widget::plannerGraphicsView->setScene(_scene);  // Устанавливаем графическую сцену в plannerGraphicsView
//    ui->Widget::plannerGraphicsView->setRenderHint(QPainter::Antialiasing);    // Устанавливаем сглаживание
//    ui->Widget::plannerGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем скроллбар по вертикали
//    ui->Widget::plannerGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем скроллбар по горизонтали

    _scene->setSceneRect(-250,-250,500,500);       /// Устанавливаем область графической сцены
    _scene->addLine(-250,0,250,0,QPen(Qt::black)); /// Добавляем горизонтальную линию через центр
    _scene->addLine(0,-250,0,250,QPen(Qt::black)); /// Добавляем вертикальную линию через центр
    _scene->addText("addText",_font);

#if 0

    this->resize(600,600);          // Задаем размеры виджета, то есть окна
    this->setFixedSize(600,600);    // Фиксируем размеры виджета

    scene = new QGraphicsScene(ui->Widget::plannerGraphicsView);   // Инициализируем графическую сцену
///    triangle = new Triangle();      // Инициализируем треугольник


    ui->Widget::plannerGraphicsView->setScene(scene);  // Устанавливаем графическую сцену в plannerGraphicsView
    ui->Widget::plannerGraphicsView->setRenderHint(QPainter::Antialiasing);    // Устанавливаем сглаживание
    ui->Widget::plannerGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем скроллбар по вертикали
    ui->Widget::plannerGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем скроллбар по горизонтали

    scene->setSceneRect(-250,-250,500,500); // Устанавливаем область графической сцены

    scene->addLine(-250,0,250,0,QPen(Qt::black));   // Добавляем горизонтальную линию через центр
    scene->addLine(0,-250,0,250,QPen(Qt::black));   // Добавляем вертикальную линию через центр

    //scene->addItem(triangle);   // Добавляем на сцену треугольник
    //triangle->setPos(0,0);      // Устанавливаем треугольник в центр сцены
#endif

    ui->mainWidget->show();

}
///-----------------------------------------------------------------------------
Widget::~Widget()
{
    delete ui;
    delete _graph;
    delete _view;


}
///-----------------------------------------------------------------------------
void Widget::addData()
{
    // Configure the axes according to the data
    //! [4]
    _graph->axisX()->setTitle("X");
    _graph->axisY()->setTitle("Y");
    _graph->axisZ()->setTitle("Z");
    //! [4]

    //! [5]
    QtDataVisualization::QScatterDataArray *dataArray = new QtDataVisualization::QScatterDataArray;
    dataArray->resize(_itemCount);
    QtDataVisualization::QScatterDataItem *ptrToDataArray = &dataArray->first();
    //! [5]

#ifdef RANDOM_SCATTER
    for (int i = 0; i < m_itemCount; i++) {
        ptrToDataArray->setPosition(randVector());
        ptrToDataArray++;
    }
#else
    //! [6]
    float limit = qSqrt(_itemCount) / 2.0f;
    for (float i = -limit; i < limit; i++) {
        for (float j = -limit; j < limit; j++) {
            ptrToDataArray->setPosition(QVector3D(i + 0.5f,
                                                  qCos(qDegreesToRadians((i * j) / _curveDivider)),
                                                  j + 0.5f));
            ptrToDataArray++;
        }
    }
    //! [6]
#endif

    //! [7]
    _graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
    //! [7]
}
///-----------------------------------------------------------------------------
void Widget::on_testPushButton_clicked()
{
    qDebug() << "test push button clicked" << endl;

    if(ui->Widget::testCheckBox->checkState() == Qt::CheckState::Checked)
    {
        ui->Widget::testCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        ui->Widget::testCheckBox->setCheckState(Qt::CheckState::Checked);
    }

}
///-----------------------------------------------------------------------------


