/********************************************************************************
** Form generated from reading UI file 'mainform.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFORM_H
#define UI_MAINFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainForm
{
public:
    QGraphicsView *graphicsView;
    QPushButton *showPlanner3D;
    QPushButton *closePlanner3D;
    QPushButton *clearPlaner3D;

    void setupUi(QWidget *mainForm)
    {
        if (mainForm->objectName().isEmpty())
            mainForm->setObjectName(QString::fromUtf8("mainForm"));
        mainForm->resize(803, 662);
        graphicsView = new QGraphicsView(mainForm);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(180, 200, 256, 192));
        showPlanner3D = new QPushButton(mainForm);
        showPlanner3D->setObjectName(QString::fromUtf8("showPlanner3D"));
        showPlanner3D->setGeometry(QRect(480, 490, 80, 25));
        closePlanner3D = new QPushButton(mainForm);
        closePlanner3D->setObjectName(QString::fromUtf8("closePlanner3D"));
        closePlanner3D->setGeometry(QRect(590, 490, 80, 25));
        clearPlaner3D = new QPushButton(mainForm);
        clearPlaner3D->setObjectName(QString::fromUtf8("clearPlaner3D"));
        clearPlaner3D->setGeometry(QRect(360, 560, 80, 25));

        retranslateUi(mainForm);

        QMetaObject::connectSlotsByName(mainForm);
    } // setupUi

    void retranslateUi(QWidget *mainForm)
    {
        mainForm->setWindowTitle(QApplication::translate("mainForm", "Form", nullptr));
        showPlanner3D->setText(QApplication::translate("mainForm", "show plot", nullptr));
        closePlanner3D->setText(QApplication::translate("mainForm", "close plot", nullptr));
        clearPlaner3D->setText(QApplication::translate("mainForm", "clear plot", nullptr));
    } // retranslateUi

};

namespace Ui {
    class mainForm: public Ui_mainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H
