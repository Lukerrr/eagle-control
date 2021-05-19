/********************************************************************************
** Form generated from reading UI file 'user_interfacent4368.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef USER_INTERFACENT4368_H
#define USER_INTERFACENT4368_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserInterface
{
public:
    QAction *actionaa;
    QAction *actionbb;
    QWidget *centralwidget;
    QGroupBox *droneStateBox;
    QLabel *droneStatePos;
    QLabel *droneStateRot;
    QLabel *droneStateHgt;
    QLabel *droneStateMode;
    QLabel *chargeLabel;
    QProgressBar *chargeBar;
    QPushButton *armBtn;
    QLabel *connStateLabel;
    QPushButton *startBtn;
    QPushButton *plannerBtn;
    QDoubleSpinBox *heightSpinBox;
    QLabel *heightLabel;
    QPushButton *sendPathBtn;
    QPushButton *adjustHeightBtn;
    QLabel *toleranceLabel;
    QDoubleSpinBox *toleranceSpinBox;
    QPushButton *adjustToleranceBtn;
    QLabel *pathSpacingLabel;
    QDoubleSpinBox *pathSpacingSpinBox;

    void setupUi(QMainWindow *UserInterface)
    {
        if (UserInterface->objectName().isEmpty())
            UserInterface->setObjectName(QStringLiteral("UserInterface"));
        UserInterface->setEnabled(true);
        UserInterface->resize(543, 188);
        UserInterface->setContextMenuPolicy(Qt::DefaultContextMenu);
        UserInterface->setDocumentMode(false);
        UserInterface->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        actionaa = new QAction(UserInterface);
        actionaa->setObjectName(QStringLiteral("actionaa"));
        actionbb = new QAction(UserInterface);
        actionbb->setObjectName(QStringLiteral("actionbb"));
        centralwidget = new QWidget(UserInterface);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        centralwidget->setEnabled(true);
        droneStateBox = new QGroupBox(centralwidget);
        droneStateBox->setObjectName(QStringLiteral("droneStateBox"));
        droneStateBox->setEnabled(true);
        droneStateBox->setGeometry(QRect(260, 10, 271, 151));
        droneStatePos = new QLabel(droneStateBox);
        droneStatePos->setObjectName(QStringLiteral("droneStatePos"));
        droneStatePos->setGeometry(QRect(10, 30, 251, 21));
        droneStateRot = new QLabel(droneStateBox);
        droneStateRot->setObjectName(QStringLiteral("droneStateRot"));
        droneStateRot->setGeometry(QRect(10, 50, 251, 21));
        droneStateHgt = new QLabel(droneStateBox);
        droneStateHgt->setObjectName(QStringLiteral("droneStateHgt"));
        droneStateHgt->setGeometry(QRect(10, 70, 251, 21));
        droneStateMode = new QLabel(droneStateBox);
        droneStateMode->setObjectName(QStringLiteral("droneStateMode"));
        droneStateMode->setGeometry(QRect(10, 90, 251, 21));
        chargeLabel = new QLabel(droneStateBox);
        chargeLabel->setObjectName(QStringLiteral("chargeLabel"));
        chargeLabel->setEnabled(true);
        chargeLabel->setGeometry(QRect(10, 123, 111, 17));
        chargeBar = new QProgressBar(droneStateBox);
        chargeBar->setObjectName(QStringLiteral("chargeBar"));
        chargeBar->setEnabled(true);
        chargeBar->setGeometry(QRect(120, 120, 141, 23));
        chargeBar->setValue(0);
        armBtn = new QPushButton(centralwidget);
        armBtn->setObjectName(QStringLiteral("armBtn"));
        armBtn->setEnabled(true);
        armBtn->setGeometry(QRect(10, 10, 89, 25));
        connStateLabel = new QLabel(centralwidget);
        connStateLabel->setObjectName(QStringLiteral("connStateLabel"));
        connStateLabel->setGeometry(QRect(430, 168, 101, 17));
        connStateLabel->setLayoutDirection(Qt::LeftToRight);
        connStateLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        startBtn = new QPushButton(centralwidget);
        startBtn->setObjectName(QStringLiteral("startBtn"));
        startBtn->setEnabled(false);
        startBtn->setGeometry(QRect(10, 40, 89, 25));
        plannerBtn = new QPushButton(centralwidget);
        plannerBtn->setObjectName(QStringLiteral("plannerBtn"));
        plannerBtn->setEnabled(true);
        plannerBtn->setGeometry(QRect(150, 10, 89, 25));
        heightSpinBox = new QDoubleSpinBox(centralwidget);
        heightSpinBox->setObjectName(QStringLiteral("heightSpinBox"));
        heightSpinBox->setGeometry(QRect(110, 80, 71, 21));
        heightSpinBox->setMinimum(1.8);
        heightSpinBox->setMaximum(8);
        heightSpinBox->setSingleStep(0.01);
        heightSpinBox->setValue(2);
        heightLabel = new QLabel(centralwidget);
        heightLabel->setObjectName(QStringLiteral("heightLabel"));
        heightLabel->setGeometry(QRect(10, 82, 91, 17));
        sendPathBtn = new QPushButton(centralwidget);
        sendPathBtn->setObjectName(QStringLiteral("sendPathBtn"));
        sendPathBtn->setEnabled(false);
        sendPathBtn->setGeometry(QRect(150, 40, 89, 25));
        adjustHeightBtn = new QPushButton(centralwidget);
        adjustHeightBtn->setObjectName(QStringLiteral("adjustHeightBtn"));
        adjustHeightBtn->setEnabled(true);
        adjustHeightBtn->setGeometry(QRect(10, 130, 101, 25));
        toleranceLabel = new QLabel(centralwidget);
        toleranceLabel->setObjectName(QStringLiteral("toleranceLabel"));
        toleranceLabel->setGeometry(QRect(10, 102, 91, 17));
        toleranceSpinBox = new QDoubleSpinBox(centralwidget);
        toleranceSpinBox->setObjectName(QStringLiteral("toleranceSpinBox"));
        toleranceSpinBox->setGeometry(QRect(110, 100, 71, 21));
        toleranceSpinBox->setMinimum(0.01);
        toleranceSpinBox->setMaximum(15);
        toleranceSpinBox->setSingleStep(0.01);
        toleranceSpinBox->setValue(1);
        adjustToleranceBtn = new QPushButton(centralwidget);
        adjustToleranceBtn->setObjectName(QStringLiteral("adjustToleranceBtn"));
        adjustToleranceBtn->setEnabled(true);
        adjustToleranceBtn->setGeometry(QRect(120, 130, 121, 25));
        pathSpacingLabel = new QLabel(centralwidget);
        pathSpacingLabel->setObjectName(QStringLiteral("pathSpacingLabel"));
        pathSpacingLabel->setGeometry(QRect(10, 162, 101, 17));
        pathSpacingSpinBox = new QDoubleSpinBox(centralwidget);
        pathSpacingSpinBox->setObjectName(QStringLiteral("pathSpacingSpinBox"));
        pathSpacingSpinBox->setGeometry(QRect(110, 160, 71, 21));
        pathSpacingSpinBox->setDecimals(2);
        pathSpacingSpinBox->setMinimum(1);
        pathSpacingSpinBox->setMaximum(1000);
        pathSpacingSpinBox->setSingleStep(0.01);
        pathSpacingSpinBox->setValue(20);
        UserInterface->setCentralWidget(centralwidget);

        retranslateUi(UserInterface);

        QMetaObject::connectSlotsByName(UserInterface);
    } // setupUi

    void retranslateUi(QMainWindow *UserInterface)
    {
        UserInterface->setWindowTitle(QApplication::translate("UserInterface", "EagleControl", Q_NULLPTR));
        actionaa->setText(QApplication::translate("UserInterface", "aa", Q_NULLPTR));
        actionbb->setText(QApplication::translate("UserInterface", "bb", Q_NULLPTR));
        droneStateBox->setTitle(QApplication::translate("UserInterface", "Vehicle data", Q_NULLPTR));
        droneStatePos->setText(QApplication::translate("UserInterface", "Position: [0.0, 0.0, 0.0]", Q_NULLPTR));
        droneStateRot->setText(QApplication::translate("UserInterface", "Rotation: [0.0, 0.0, 0.0]", Q_NULLPTR));
        droneStateHgt->setText(QApplication::translate("UserInterface", "Ground distance: 0.0", Q_NULLPTR));
        droneStateMode->setText(QApplication::translate("UserInterface", "Offboard: disabled", Q_NULLPTR));
        chargeLabel->setText(QApplication::translate("UserInterface", "Battery charge:", Q_NULLPTR));
        armBtn->setText(QApplication::translate("UserInterface", "Arm", Q_NULLPTR));
        connStateLabel->setText(QApplication::translate("UserInterface", "Disconnected", Q_NULLPTR));
        startBtn->setText(QApplication::translate("UserInterface", "Start", Q_NULLPTR));
        plannerBtn->setText(QApplication::translate("UserInterface", "Planner", Q_NULLPTR));
        heightLabel->setText(QApplication::translate("UserInterface", "Height:", Q_NULLPTR));
        sendPathBtn->setText(QApplication::translate("UserInterface", "Send path", Q_NULLPTR));
        adjustHeightBtn->setText(QApplication::translate("UserInterface", "Adjust height", Q_NULLPTR));
        toleranceLabel->setText(QApplication::translate("UserInterface", "Tolerance:", Q_NULLPTR));
        adjustToleranceBtn->setText(QApplication::translate("UserInterface", "Adjust tolerance", Q_NULLPTR));
        pathSpacingLabel->setText(QApplication::translate("UserInterface", "Path spacing:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class UserInterface: public Ui_UserInterface {};
} // namespace Ui

QT_END_NAMESPACE

#endif // USER_INTERFACENT4368_H
