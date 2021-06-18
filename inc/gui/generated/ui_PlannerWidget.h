/********************************************************************************
** Form generated from reading UI file 'planner_widgetpKYnJG.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PLANNER_WIDGETPKYNJG_H
#define PLANNER_WIDGETPKYNJG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlannerWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *noGpsLabel;

    void setupUi(QWidget *PlannerWidget)
    {
        if (PlannerWidget->objectName().isEmpty())
            PlannerWidget->setObjectName(QString::fromUtf8("PlannerWidget"));
        PlannerWidget->resize(512, 512);
        PlannerWidget->setCursor(QCursor(Qt::CrossCursor));
        PlannerWidget->setMouseTracking(true);
        PlannerWidget->setFocusPolicy(Qt::StrongFocus);
        PlannerWidget->setAcceptDrops(true);
        horizontalLayout = new QHBoxLayout(PlannerWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        noGpsLabel = new QLabel(PlannerWidget);
        noGpsLabel->setObjectName(QString::fromUtf8("noGpsLabel"));
        QFont font;
        font.setPointSize(11);
        font.setBold(false);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(50);
        font.setStrikeOut(false);
        font.setKerning(false);
        noGpsLabel->setFont(font);
        noGpsLabel->setAutoFillBackground(false);
        noGpsLabel->setStyleSheet(QString::fromUtf8("color: rgb(204, 0, 0);"));
        noGpsLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        noGpsLabel->setWordWrap(false);
        noGpsLabel->setMargin(0);

        horizontalLayout->addWidget(noGpsLabel, 0, Qt::AlignLeft|Qt::AlignTop);


        retranslateUi(PlannerWidget);

        QMetaObject::connectSlotsByName(PlannerWidget);
    } // setupUi

    void retranslateUi(QWidget *PlannerWidget)
    {
        PlannerWidget->setWindowTitle(QApplication::translate("PlannerWidget", "EagleControl - Planner", nullptr));
        noGpsLabel->setText(QApplication::translate("PlannerWidget", "No GPS data", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlannerWidget: public Ui_PlannerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PLANNER_WIDGETPKYNJG_H
