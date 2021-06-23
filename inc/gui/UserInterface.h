#pragma once

#include "generated/ui_UserInterface.h"
#include "PlannerWidget.h"
#include "CmdMsgs.h"

#include <QCloseEvent>

class QUserInterface : public QMainWindow
{
    Q_OBJECT

public:
    QUserInterface(QWidget* parent = Q_NULLPTR);
    ~QUserInterface();

    virtual bool event(QEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    void OnArmBtnClicked();
    void OnStartBtnClicked();
    void OnPlannerBtnClicked();
    void OnSendPathBtnClicked();
    void OnAdjustHeightBtnClicked();
    void OnAdjustToleranceBtnClicked();
    void OnAdjustDensityBtnClicked();
    void OnGetCloudBtnClicked();

    void OnPathSpacingChanged(double val);

    QString FormatBytes(uint32_t bytesNum);

    Ui::UserInterface m_ui;
    QPlannerWidget m_plannerWidget;
    SMissionData m_currentMission;

    bool m_bNeedsUpdateMissionParams = false;
};