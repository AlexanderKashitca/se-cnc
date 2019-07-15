/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *testPushButton;
    QCheckBox *testCheckBox;
    QGraphicsView *plannerGraphicsView;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(1004, 614);
        testPushButton = new QPushButton(Widget);
        testPushButton->setObjectName(QString::fromUtf8("testPushButton"));
        testPushButton->setGeometry(QRect(100, 10, 80, 25));
        testCheckBox = new QCheckBox(Widget);
        testCheckBox->setObjectName(QString::fromUtf8("testCheckBox"));
        testCheckBox->setGeometry(QRect(20, 10, 82, 23));
        plannerGraphicsView = new QGraphicsView(Widget);
        plannerGraphicsView->setObjectName(QString::fromUtf8("plannerGraphicsView"));
        plannerGraphicsView->setGeometry(QRect(10, 40, 481, 281));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", nullptr));
        testPushButton->setText(QApplication::translate("Widget", "PushButton", nullptr));
        testCheckBox->setText(QApplication::translate("Widget", "CheckBox", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
