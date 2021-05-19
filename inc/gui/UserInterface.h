#pragma once

#include "generated/ui_UserInterface.h"
#include "MissionData.h"
#include "PlannerWidget.h"

#include <QCloseEvent>

class QUserInterface : public QMainWindow
{
    Q_OBJECT

public:
    QUserInterface(QWidget* parent = Q_NULLPTR);
    ~QUserInterface();

    virtual bool event(QEvent* pEvent) override;
    virtual void closeEvent(QCloseEvent *event);

    void SetEnableSendPathButton(bool bState);

private:
    void OnArmBtnClicked();
    void OnStartBtnClicked();
    void OnPlannerBtnClicked();
    void OnSendPathBtnClicked();
    void OnAdjustHeightBtnClicked();
    void OnAdjustToleranceBtnClicked();

    void OnHeightChanged(double val);
    void OnToleranceChanged(double val);
    void OnPathSpacingChanged(double val);

    Ui::UserInterface m_ui;
    QPlannerWidget m_plannerWidget;
    SMissionData m_currentMission;
};