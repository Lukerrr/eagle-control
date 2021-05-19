/********************************************************************************
** Form generated from reading UI file 'planner_widgetvn6783.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PLANNER_WIDGETVN6783_H
#define PLANNER_WIDGETVN6783_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlannerWidget
{
public:
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *PlannerWidget)
    {
        if (PlannerWidget->objectName().isEmpty())
            PlannerWidget->setObjectName(QStringLiteral("PlannerWidget"));
        PlannerWidget->resize(512, 512);
        PlannerWidget->setCursor(QCursor(Qt::CrossCursor));
        PlannerWidget->setMouseTracking(true);
        PlannerWidget->setFocusPolicy(Qt::StrongFocus);
        PlannerWidget->setAcceptDrops(true);
        horizontalLayout = new QHBoxLayout(PlannerWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);

        retranslateUi(PlannerWidget);

        QMetaObject::connectSlotsByName(PlannerWidget);
    } // setupUi

    void retranslateUi(QWidget *PlannerWidget)
    {
        PlannerWidget->setWindowTitle(QApplication::translate("PlannerWidget", "EagleControl - Planner", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PlannerWidget: public Ui_PlannerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PLANNER_WIDGETVN6783_H
