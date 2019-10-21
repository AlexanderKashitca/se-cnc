///-----------------------------------------------------------------------------
#include "semainwindow.h"
#include "ui_semainwindow.h"
///-----------------------------------------------------------------------------
SeMainWindow::SeMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SeMainWindow)
{
    ui->setupUi(this);
    pbxLayout = new QHBoxLayout;


    pbxLayout->addWidget(ui->pushButton);
    pbxLayout->addStretch(1);
    pbxLayout->addWidget(ui->pushButton_2);
    pbxLayout->addStretch(1);
    pbxLayout->addWidget(ui->pushButton_3);
    pbxLayout->addStretch(1);
    pbxLayout->addWidget(ui->pushButton_4);
    pbxLayout->addStretch(1);
    pbxLayout->addWidget(ui->pushButton_5);

    setLayout(pbxLayout);


}
///-----------------------------------------------------------------------------
SeMainWindow::~SeMainWindow()
{
    delete ui;
}
///-----------------------------------------------------------------------------
