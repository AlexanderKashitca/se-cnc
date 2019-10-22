///-----------------------------------------------------------------------------
#include "semainwindow.h"
#include "ui_semainwindow.h"
///-----------------------------------------------------------------------------
SeMainWindow::SeMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SeMainWindow)
{
    ui->setupUi(this);
    /// add action connection
    connect(ui->actionOpen,         SIGNAL(triggered()), this, SLOT(menuActionOpen()));
    connect(ui->actionClose,        SIGNAL(triggered()), this, SLOT(menuActionClose()));
    connect(ui->actionMaintenance,  SIGNAL(triggered()), this, SLOT(menuActionMaintenance()));
    connect(ui->actionDiagnosticIO, SIGNAL(triggered()), this, SLOT(menuActionDiagnisticIO()));

    /// hide all widgets
    widgetShow(WIDGET_HIDE);
}
///-----------------------------------------------------------------------------
SeMainWindow::~SeMainWindow()
{
    delete ui;
}
///-----------------------------------------------------------------------------
void SeMainWindow::widgetShow(WIDGET_TYPE type)
{
    switch(type)
    {
        case WIDGET_HIDE :
            ui->programGridWidget->hide();
            ui->diagnosticIIOGridWidget->hide();
            break;
        case WIDGET_PROGRAM_CODE :
            ui->diagnosticIIOGridWidget->hide();
            ui->programGridWidget->show();
            break;
        case WIDGET_DIAGNOSTIC_IO :
            ui->programGridWidget->hide();
            ui->diagnosticIIOGridWidget->show();
            break;
    }
}
///-----------------------------------------------------------------------------
void SeMainWindow::menuActionOpen()
{
    qDebug() << "action Open";
    /// clear  all old data
    ui->programCodeEdit->clear();
    /// create open dialog and loading program  file
    QFileDialog *filedlg;
    filedlg  = new QFileDialog();

    QString filename;
    filename = filedlg->getOpenFileName();
    QFile file(filename);

    file.open(QFile::ReadOnly | QFile::Text);

    QTextStream ReadFile(&file);
    ui->programCodeEdit->setText(ReadFile.readAll());
    /// show programm widget and hide other
    widgetShow(WIDGET_PROGRAM_CODE);

}
///-----------------------------------------------------------------------------
void SeMainWindow::menuActionClose()
{
    qDebug() << "action Close";

}
///-----------------------------------------------------------------------------
void SeMainWindow::menuActionMaintenance()
{
    qDebug() << "action Maintenance";

}
///-----------------------------------------------------------------------------
void SeMainWindow::menuActionDiagnisticIO()
{
    qDebug() << "action Diagnostioc IO";
    /// show diagnostic widget and hide other
    widgetShow(WIDGET_DIAGNOSTIC_IO);
}
///-----------------------------------------------------------------------------
