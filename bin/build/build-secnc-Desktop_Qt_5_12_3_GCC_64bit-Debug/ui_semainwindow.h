/********************************************************************************
** Form generated from reading UI file 'semainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEMAINWINDOW_H
#define UI_SEMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SeMainWindow
{
public:
    QAction *actionOpen;
    QAction *actionClose;
    QAction *actionMaintenance;
    QAction *actionDiagnosticIO;
    QAction *actionTEST;
    QAction *actionManualControl;
    QWidget *centralWidget;
    QWidget *programGridWidget;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit;
    QPushButton *mdiPushButton;
    QTextEdit *programCodeEdit;
    QWidget *diagnosticIIOGridWidget;
    QGridLayout *gridLayout_2;
    QWidget *manualGridWidget;
    QGridLayout *manualGridLayout;
    QWidget *maintenanceGridWidget;
    QGridLayout *maintenanceGridLayout;
    QToolButton *settingsPathToolButton;
    QLabel *variablePathLabel;
    QToolButton *variablePathToolButton;
    QSpacerItem *verticalSpacer;
    QComboBox *axisConfigurationComboBox;
    QToolButton *toolTablePathToolButton;
    QLabel *axisConfigurationLabel;
    QLineEdit *settingsPathLineEdit;
    QLabel *machineTypeLabel;
    QLineEdit *toolTablePathLineEdit;
    QComboBox *machineTypeComboBox;
    QLineEdit *variablePathLineEdit;
    QLabel *settingsPathLabel;
    QLabel *toolTablePathLabel;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuMachine;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *SeMainWindow)
    {
        if (SeMainWindow->objectName().isEmpty())
            SeMainWindow->setObjectName(QString::fromUtf8("SeMainWindow"));
        SeMainWindow->resize(1537, 1097);
        SeMainWindow->setMinimumSize(QSize(1024, 768));
        SeMainWindow->setMaximumSize(QSize(10240, 7680));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(239, 239, 239, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(247, 247, 247, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(119, 119, 119, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(159, 159, 159, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush3);
        QBrush brush6(QColor(255, 255, 220, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        QBrush brush7(QColor(0, 0, 0, 128));
        brush7.setStyle(Qt::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush7);
#endif
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush7);
#endif
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush7);
#endif
        SeMainWindow->setPalette(palette);
        SeMainWindow->setCursor(QCursor(Qt::ArrowCursor));
        SeMainWindow->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        actionOpen = new QAction(SeMainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionClose = new QAction(SeMainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionMaintenance = new QAction(SeMainWindow);
        actionMaintenance->setObjectName(QString::fromUtf8("actionMaintenance"));
        actionDiagnosticIO = new QAction(SeMainWindow);
        actionDiagnosticIO->setObjectName(QString::fromUtf8("actionDiagnosticIO"));
        actionDiagnosticIO->setCheckable(false);
        actionTEST = new QAction(SeMainWindow);
        actionTEST->setObjectName(QString::fromUtf8("actionTEST"));
        actionManualControl = new QAction(SeMainWindow);
        actionManualControl->setObjectName(QString::fromUtf8("actionManualControl"));
        centralWidget = new QWidget(SeMainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        programGridWidget = new QWidget(centralWidget);
        programGridWidget->setObjectName(QString::fromUtf8("programGridWidget"));
        programGridWidget->setEnabled(true);
        programGridWidget->setGeometry(QRect(0, 0, 680, 240));
        programGridWidget->setMinimumSize(QSize(680, 240));
        programGridWidget->setMaximumSize(QSize(680, 240));
        gridLayout = new QGridLayout(programGridWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lineEdit = new QLineEdit(programGridWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        gridLayout->addWidget(lineEdit, 1, 0, 1, 1);

        mdiPushButton = new QPushButton(programGridWidget);
        mdiPushButton->setObjectName(QString::fromUtf8("mdiPushButton"));

        gridLayout->addWidget(mdiPushButton, 1, 1, 1, 1);

        programCodeEdit = new QTextEdit(programGridWidget);
        programCodeEdit->setObjectName(QString::fromUtf8("programCodeEdit"));
        QFont font;
        font.setPointSize(15);
        programCodeEdit->setFont(font);
        programCodeEdit->setLineWrapMode(QTextEdit::NoWrap);
        programCodeEdit->setReadOnly(true);

        gridLayout->addWidget(programCodeEdit, 0, 0, 1, 2);

        diagnosticIIOGridWidget = new QWidget(centralWidget);
        diagnosticIIOGridWidget->setObjectName(QString::fromUtf8("diagnosticIIOGridWidget"));
        diagnosticIIOGridWidget->setGeometry(QRect(0, 0, 680, 240));
        diagnosticIIOGridWidget->setMinimumSize(QSize(680, 240));
        diagnosticIIOGridWidget->setMaximumSize(QSize(680, 240));
        gridLayout_2 = new QGridLayout(diagnosticIIOGridWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        manualGridWidget = new QWidget(centralWidget);
        manualGridWidget->setObjectName(QString::fromUtf8("manualGridWidget"));
        manualGridWidget->setGeometry(QRect(730, 10, 680, 240));
        manualGridWidget->setMinimumSize(QSize(680, 240));
        manualGridWidget->setMaximumSize(QSize(680, 240));
        manualGridLayout = new QGridLayout(manualGridWidget);
        manualGridLayout->setSpacing(6);
        manualGridLayout->setContentsMargins(11, 11, 11, 11);
        manualGridLayout->setObjectName(QString::fromUtf8("manualGridLayout"));
        maintenanceGridWidget = new QWidget(centralWidget);
        maintenanceGridWidget->setObjectName(QString::fromUtf8("maintenanceGridWidget"));
        maintenanceGridWidget->setGeometry(QRect(0, 250, 680, 240));
        maintenanceGridWidget->setMinimumSize(QSize(680, 240));
        maintenanceGridWidget->setMaximumSize(QSize(680, 240));
        maintenanceGridLayout = new QGridLayout(maintenanceGridWidget);
        maintenanceGridLayout->setSpacing(6);
        maintenanceGridLayout->setContentsMargins(11, 11, 11, 11);
        maintenanceGridLayout->setObjectName(QString::fromUtf8("maintenanceGridLayout"));
        settingsPathToolButton = new QToolButton(maintenanceGridWidget);
        settingsPathToolButton->setObjectName(QString::fromUtf8("settingsPathToolButton"));

        maintenanceGridLayout->addWidget(settingsPathToolButton, 3, 1, 1, 1);

        variablePathLabel = new QLabel(maintenanceGridWidget);
        variablePathLabel->setObjectName(QString::fromUtf8("variablePathLabel"));

        maintenanceGridLayout->addWidget(variablePathLabel, 2, 2, 1, 1);

        variablePathToolButton = new QToolButton(maintenanceGridWidget);
        variablePathToolButton->setObjectName(QString::fromUtf8("variablePathToolButton"));

        maintenanceGridLayout->addWidget(variablePathToolButton, 2, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        maintenanceGridLayout->addItem(verticalSpacer, 5, 0, 1, 3);

        axisConfigurationComboBox = new QComboBox(maintenanceGridWidget);
        axisConfigurationComboBox->addItem(QString());
        axisConfigurationComboBox->addItem(QString());
        axisConfigurationComboBox->addItem(QString());
        axisConfigurationComboBox->addItem(QString());
        axisConfigurationComboBox->setObjectName(QString::fromUtf8("axisConfigurationComboBox"));

        maintenanceGridLayout->addWidget(axisConfigurationComboBox, 0, 0, 1, 2);

        toolTablePathToolButton = new QToolButton(maintenanceGridWidget);
        toolTablePathToolButton->setObjectName(QString::fromUtf8("toolTablePathToolButton"));

        maintenanceGridLayout->addWidget(toolTablePathToolButton, 4, 1, 1, 1);

        axisConfigurationLabel = new QLabel(maintenanceGridWidget);
        axisConfigurationLabel->setObjectName(QString::fromUtf8("axisConfigurationLabel"));

        maintenanceGridLayout->addWidget(axisConfigurationLabel, 0, 2, 1, 1);

        settingsPathLineEdit = new QLineEdit(maintenanceGridWidget);
        settingsPathLineEdit->setObjectName(QString::fromUtf8("settingsPathLineEdit"));
        settingsPathLineEdit->setReadOnly(true);

        maintenanceGridLayout->addWidget(settingsPathLineEdit, 3, 0, 1, 1);

        machineTypeLabel = new QLabel(maintenanceGridWidget);
        machineTypeLabel->setObjectName(QString::fromUtf8("machineTypeLabel"));

        maintenanceGridLayout->addWidget(machineTypeLabel, 1, 2, 1, 1);

        toolTablePathLineEdit = new QLineEdit(maintenanceGridWidget);
        toolTablePathLineEdit->setObjectName(QString::fromUtf8("toolTablePathLineEdit"));
        toolTablePathLineEdit->setReadOnly(true);

        maintenanceGridLayout->addWidget(toolTablePathLineEdit, 4, 0, 1, 1);

        machineTypeComboBox = new QComboBox(maintenanceGridWidget);
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->addItem(QString());
        machineTypeComboBox->setObjectName(QString::fromUtf8("machineTypeComboBox"));

        maintenanceGridLayout->addWidget(machineTypeComboBox, 1, 0, 1, 2);

        variablePathLineEdit = new QLineEdit(maintenanceGridWidget);
        variablePathLineEdit->setObjectName(QString::fromUtf8("variablePathLineEdit"));
        variablePathLineEdit->setReadOnly(true);

        maintenanceGridLayout->addWidget(variablePathLineEdit, 2, 0, 1, 1);

        settingsPathLabel = new QLabel(maintenanceGridWidget);
        settingsPathLabel->setObjectName(QString::fromUtf8("settingsPathLabel"));

        maintenanceGridLayout->addWidget(settingsPathLabel, 3, 2, 1, 1);

        toolTablePathLabel = new QLabel(maintenanceGridWidget);
        toolTablePathLabel->setObjectName(QString::fromUtf8("toolTablePathLabel"));

        maintenanceGridLayout->addWidget(toolTablePathLabel, 4, 2, 1, 1);

        SeMainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(SeMainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1537, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuMachine = new QMenu(menuBar);
        menuMachine->setObjectName(QString::fromUtf8("menuMachine"));
        SeMainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SeMainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
        SeMainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(SeMainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        SeMainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuMachine->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addSeparator();
        menuFile->addAction(actionMaintenance);
        menuMachine->addAction(actionDiagnosticIO);
        menuMachine->addSeparator();
        menuMachine->addAction(actionManualControl);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionTEST);

        retranslateUi(SeMainWindow);

        QMetaObject::connectSlotsByName(SeMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *SeMainWindow)
    {
        SeMainWindow->setWindowTitle(QApplication::translate("SeMainWindow", "SeMainWindow", nullptr));
        actionOpen->setText(QApplication::translate("SeMainWindow", "Open", nullptr));
        actionClose->setText(QApplication::translate("SeMainWindow", "Close", nullptr));
        actionMaintenance->setText(QApplication::translate("SeMainWindow", "Maintenance", nullptr));
        actionDiagnosticIO->setText(QApplication::translate("SeMainWindow", "Diagnostic I/O", nullptr));
        actionTEST->setText(QApplication::translate("SeMainWindow", "TEST", nullptr));
#ifndef QT_NO_SHORTCUT
        actionTEST->setShortcut(QApplication::translate("SeMainWindow", "Ctrl+D", nullptr));
#endif // QT_NO_SHORTCUT
        actionManualControl->setText(QApplication::translate("SeMainWindow", "Manual Control", nullptr));
        mdiPushButton->setText(QApplication::translate("SeMainWindow", "MDI", nullptr));
        settingsPathToolButton->setText(QApplication::translate("SeMainWindow", "...", nullptr));
        variablePathLabel->setText(QApplication::translate("SeMainWindow", "Variable File", nullptr));
        variablePathToolButton->setText(QApplication::translate("SeMainWindow", "...", nullptr));
        axisConfigurationComboBox->setItemText(0, QApplication::translate("SeMainWindow", "2 Axis", nullptr));
        axisConfigurationComboBox->setItemText(1, QApplication::translate("SeMainWindow", "3 Axis", nullptr));
        axisConfigurationComboBox->setItemText(2, QApplication::translate("SeMainWindow", "4 Axis", nullptr));
        axisConfigurationComboBox->setItemText(3, QApplication::translate("SeMainWindow", "5 Axis", nullptr));

        toolTablePathToolButton->setText(QApplication::translate("SeMainWindow", "...", nullptr));
        axisConfigurationLabel->setText(QApplication::translate("SeMainWindow", "Axis Number", nullptr));
        machineTypeLabel->setText(QApplication::translate("SeMainWindow", "Machine type", nullptr));
        machineTypeComboBox->setItemText(0, QApplication::translate("SeMainWindow", "Die  EDM", nullptr));
        machineTypeComboBox->setItemText(1, QApplication::translate("SeMainWindow", "Flame", nullptr));
        machineTypeComboBox->setItemText(2, QApplication::translate("SeMainWindow", "Grinder", nullptr));
        machineTypeComboBox->setItemText(3, QApplication::translate("SeMainWindow", "Laser", nullptr));
        machineTypeComboBox->setItemText(4, QApplication::translate("SeMainWindow", "Milling", nullptr));
        machineTypeComboBox->setItemText(5, QApplication::translate("SeMainWindow", "Punch", nullptr));
        machineTypeComboBox->setItemText(6, QApplication::translate("SeMainWindow", "Turning", nullptr));
        machineTypeComboBox->setItemText(7, QApplication::translate("SeMainWindow", "Wire EDM", nullptr));
        machineTypeComboBox->setItemText(8, QApplication::translate("SeMainWindow", "Probe", nullptr));
        machineTypeComboBox->setItemText(9, QApplication::translate("SeMainWindow", "Plotting", nullptr));

        settingsPathLabel->setText(QApplication::translate("SeMainWindow", "Settings File", nullptr));
        toolTablePathLabel->setText(QApplication::translate("SeMainWindow", "Tool Table", nullptr));
        menuFile->setTitle(QApplication::translate("SeMainWindow", "File", nullptr));
        menuMachine->setTitle(QApplication::translate("SeMainWindow", "Machine", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SeMainWindow: public Ui_SeMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEMAINWINDOW_H
