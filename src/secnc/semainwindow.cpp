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
    connect(ui->actionOpen,          SIGNAL(triggered()), this, SLOT(menuOpenAction()));
    connect(ui->actionClose,         SIGNAL(triggered()), this, SLOT(menuCloseAction()));
    connect(ui->actionMaintenance,   SIGNAL(triggered()), this, SLOT(menuMaintenanceAction()));
    connect(ui->actionDiagnosticIO,  SIGNAL(triggered()), this, SLOT(menuDiagnisticIOAction()));
    connect(ui->actionManualControl, SIGNAL(triggered()), this, SLOT(menuManualControlAction()));

    connect(ui->variablePathToolButton,  SIGNAL(pressed()), this, SLOT(variablePathToolButtonAction()));
    connect(ui->settingsPathToolButton,  SIGNAL(pressed()), this, SLOT(settingsPathToolButtonAction()));
    connect(ui->toolTablePathToolButton, SIGNAL(pressed()), this, SLOT(toolTablePathToolButtonAction()));

    connect(ui->axisConfigurationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(axisConfigurationComboBoxCurrentIndexChanged(int)));
    connect(ui->machineTypeComboBox,       SIGNAL(currentIndexChanged(int)), this, SLOT(machineTypeComboBoxCurrentIndexChanged(int)));



    /// hide all widgets
    widgetShow(WIDGET_HIDE);
    /// system dialog object
    _filedlg = new QFileDialog();
    /// ini QSettings
    _settingsFile   = "settings.conf";
    _configSettings = new QSettings(_settingsFile, QSettings::IniFormat);
    /// load all  settings
    loadKeySettings();
}
///-----------------------------------------------------------------------------
SeMainWindow::~SeMainWindow()
{
    delete _filedlg;
    delete ui;
}
///-----------------------------------------------------------------------------
/**
 * @brief SeMainWindow::loadKeySettings
 * @note Load all settings from INI file
 */
void inline SeMainWindow::loadKeySettings()
{
    int index;
    QVariant variant;
    _configSettings->beginGroup("MAINTENANCE");
    ui->variablePathLineEdit->setText(_configSettings->value("variablePath").toString());
    ui->settingsPathLineEdit->setText(_configSettings->value("settingsPath").toString());
    ui->toolTablePathLineEdit->setText(_configSettings->value("toolTablePath").toString());
    variant = _configSettings->value("axisConfiguration");
    index = variant.toInt();
    ui->axisConfigurationComboBox->setCurrentIndex(index);
    variant = _configSettings->value("machineType");
    index = variant.toInt();
    ui->machineTypeComboBox->setCurrentIndex(index);
    _configSettings->endGroup();
}
///-----------------------------------------------------------------------------
void inline SeMainWindow::saveKeySettings(const QString group,const QString name,QVariant& value)
{
    _configSettings->beginGroup(group);
    _configSettings->setValue(name,value);
    _configSettings->endGroup();
}
///-----------------------------------------------------------------------------
void inline SeMainWindow::saveKeySettings(const QString group,const QString name,QString& value)
{
    _configSettings->beginGroup(group);
    _configSettings->setValue(name,value);
    _configSettings->endGroup();
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
void SeMainWindow::menuOpenAction()
{
    qDebug() << "action Open";
    /// clear  all old data
    ui->programCodeEdit->clear();
    /// loading program  file
    _filetype = "nc program file(*.nc)";
    _filename = _filedlg->getOpenFileName(this,tr("Open File"),"",_filetype);
    _file.setFileName(_filename);
    if(_file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream ReadFile(&_file);
        ui->programCodeEdit->setText(ReadFile.readAll());
        /// show programm widget and hide other
        widgetShow(WIDGET_PROGRAM_CODE);
    }
}
///-----------------------------------------------------------------------------
void SeMainWindow::menuCloseAction()
{
    qDebug() << "action Close";
}
///-----------------------------------------------------------------------------
void SeMainWindow::menuMaintenanceAction()
{
    qDebug() << "action Maintenance";
    /// show maintenance widget and hide other
    widgetShow(WIDGET_MAINTENANCE);
}
///-----------------------------------------------------------------------------
void SeMainWindow::menuDiagnisticIOAction()
{
    qDebug() << "action Diagnostioc IO";
    /// show diagnostic widget and hide other
    widgetShow(WIDGET_DIAGNOSTIC_IO);
}
///-----------------------------------------------------------------------------
void SeMainWindow::menuManualControlAction()
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
    _filename = _filedlg->getOpenFileName(this,tr("Open Variable File"),"",_filetype);
    if(!_filename.isNull())
    {
        ui->variablePathLineEdit->setText(_filename);
        saveKeySettings("MAINTENANCE","variablePath",_filename);
    }
}
///-----------------------------------------------------------------------------
void SeMainWindow::settingsPathToolButtonAction()
{
    qDebug() << "action settings Path Tool Button";
    _filetype = "settings file(*.set)";
    _filename = _filedlg->getOpenFileName(this,tr("Open Setting File"),"",_filetype);
    if(!_filename.isNull())
    {
        ui->settingsPathLineEdit->setText(_filename);
        saveKeySettings("MAINTENANCE","settingsPath",_filename);
    }
}
///-----------------------------------------------------------------------------
void SeMainWindow::toolTablePathToolButtonAction()
{
    qDebug() << "action tool Table Path Tool  Button";
    _filetype = "tool table file(*.tbl)";
    _filename = _filedlg->getOpenFileName(this,tr("Open Tool Table File"),"",_filetype);
    if(!_filename.isNull())
    {
        ui->toolTablePathLineEdit->setText(_filename);
        saveKeySettings("MAINTENANCE","toolTablePath",_filename);
    }
}
///-----------------------------------------------------------------------------
void SeMainWindow::axisConfigurationComboBoxCurrentIndexChanged(int index)
{
    QVariant value(QVariant::Type::Int);
    qDebug() << "axis configuration index changer to - " << index;
    value = index;
    saveKeySettings("MAINTENANCE","axisConfiguration",value);
}
///-----------------------------------------------------------------------------
void SeMainWindow::machineTypeComboBoxCurrentIndexChanged(int index)
{
    QVariant value(QVariant::Type::Int);
    qDebug() << "machine  type index changer to - " << index;
    value = index;
    saveKeySettings("MAINTENANCE","machineType",value);
}
///-----------------------------------------------------------------------------
