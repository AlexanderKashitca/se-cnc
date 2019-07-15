///-----------------------------------------------------------------------------
#include "widget.h"
#include "ui_widget.h"
///-----------------------------------------------------------------------------
#include <QDebug>
///-----------------------------------------------------------------------------
Widget::Widget(QWidget *parent) : QWidget(parent),ui(new Ui::Widget)
{
    ui->setupUi(this);

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


}
///-----------------------------------------------------------------------------
Widget::~Widget()
{
    delete ui;
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
void Widget::setVectorPoint(double x,double y,double z)
{
 ///   scene->
}

