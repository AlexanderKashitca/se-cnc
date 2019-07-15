#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void setVectorPoint(double x,double y,double z);

private slots:

    void on_testPushButton_clicked();

private:
    Ui::Widget *ui;

    QGraphicsScene  *scene;     // Объявляем графическую сцену


};

#endif // WIDGET_H
