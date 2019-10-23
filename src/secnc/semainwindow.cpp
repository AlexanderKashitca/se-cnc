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
    connect(ui->actionOpen,          SIGNAL(triggered()), this, SLOT(menuActionOpen()));
    connect(ui->actionClose,         SIGNAL(triggered()), this, SLOT(menuActionClose()));
    connect(ui->actionMaintenance,   SIGNAL(triggered()), this, SLOT(menuActionMaintenance()));
    connect(ui->actionDiagnosticIO,  SIGNAL(triggered()), this, SLOT(menuActionDiagnisticIO()));
    connect(ui->actionManualControl, SIGNAL(triggered()), this, SLOT(menuActionManualControl()));

    connect(ui->variablePathToolButton,  SIGNAL(pressed()), this, SLOT(variablePathToolButtonAction()));
    connect(ui->settingsPathToolButton,  SIGNAL(pressed()), this, SLOT(settingsPathToolButtonAction()));
    connect(ui->toolTablePathToolButton, SIGNAL(pressed()), this, SLOT(toolTablePathToolButtonAction()));

    /// hide all widgets
    widgetShow(WIDGET_HIDE);

    _filedlg = new QFileDialog();
}
///-----------------------------------------------------------------------------
SeMainWindow::~SeMainWindow()
{
    delete _filedlg;
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
            ui->manualGridWidget->hide();
            ui->maintenanceGridWidget->hide();
            break;
        case WIDGET_PROGRAM_CODE :
            ui->diagnosticIIOGridWidget->hide();
            ui->manualGridWidget->hide();
            ui->maintenanceGridWidget->hide();
            ui->programGridWidget->show();
            break;
        case WIDGET_DIAGNOSTIC_IO :
            ui->programGridWidget->hide();
            ui->manualGridWidget->hide();
            ui->maintenanceGridWidget->hide();
            ui->diagnosticIIOGridWidget->show();
            break;
        case WIDGET_MANUAL_CONTROL :
            ui->programGridWidget->hide();
            ui->diagnosticIIOGridWidget->hide();
            ui->maintenanceGridWidget->hide();
            ui->manualGridWidget->show();
            break;
        case WIDGET_MAINTENANCE :
            ui->programGridWidget->hide();
            ui->diagnosticIIOGridWidget->hide();
            ui->manualGridWidget->hide();
            ui->maintenanceGridWidget->show();
            break;

    }
}
///-----------------------------------------------------------------------------
void SeMainWindow::menuActionOpen()
{

    qDebug() << "action Open";
    /// clear  all old data
    ui->programCodeEdit->clear();
    /// loading program  file
    _filetype = "nc program file(*.nc)";
    _filename = _filedlg->getOpenFileName(this,tr("Open File"),"",_filetype);

    _file.setFileName(_filename);
    _file.open(QFile::ReadOnly | QFile::Text);

    QTextStream ReadFile(&_file);
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
    /// show maintenance widget and hide other
    widgetShow(WIDGET_MAINTENANCE);
}
///-----------------------------------------------------------------------------
void SeMainWindow::menuActionDiagnisticIO()
{
    qDebug() << "action Diagnostioc IO";
    /// show diagnostic widget and hide other
    widgetShow(WIDGET_DIAGNOSTIC_IO);
}
///-----------------------------------------------------------------------------
void SeMainWindow::menuActionManualControl()
{
    qDebug() << "action Manual Control";
    /// show manual control widget and hide other
    widgetShow(WIDGET_MANUAL_CONTROL);
}
///-----------------------------------------------------------------------------
void SeMainWindow::variablePathToolButtonAction()
{
    qDebug() << "action variable Path Tool Button";
    _filetype = "variable file(*.var)";
    _filename = _filedlg->getOpenFileName(this,tr("Open File"),"",_filetype);
    ui->variablePathLineEdit->setText(_filename);
}
///-----------------------------------------------------------------------------
void SeMainWindow::settingsPathToolButtonAction()
{
    qDebug() << "action settings Path Tool Button";
    _filetype = "settings file(*.set)";
    _filename = _filedlg->getOpenFileName(this,tr("Open File"),"",_filetype);
    ui->settingsPathLineEdit->setText(_filename);
}
///-----------------------------------------------------------------------------
void SeMainWindow::toolTablePathToolButtonAction()
{
    qDebug() << "action tool Table Path Tool  Button";
    _filetype = "tool table file(*.tbl)";
    _filename = _filedlg->getOpenFileName(this,tr("Open File"),"",_filetype);
    ui->toolTablePathLineEdit->setText(_filename);
}
///-----------------------------------------------------------------------------

