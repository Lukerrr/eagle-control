#include "UserInterface.h"
#include "InterfaceEvents.h"

#include "Core.h"
#include "MathUtils.h"

#include <QFileDialog>
#include <QtGui/QPalette>

QUserInterface::QUserInterface(QWidget* parent /*= Q_NULLPTR*/)
{
    m_ui.setupUi(this);
    m_ui.centralwidget->setEnabled(false);

    connect(m_ui.armBtn, &QPushButton::clicked, this, &QUserInterface::OnArmBtnClicked);
    connect(m_ui.startBtn, &QPushButton::clicked, this, &QUserInterface::OnStartBtnClicked);
    connect(m_ui.plannerBtn, &QPushButton::clicked, this, &QUserInterface::OnPlannerBtnClicked);
    connect(m_ui.sendPathBtn, &QPushButton::clicked, this, &QUserInterface::OnSendPathBtnClicked);
    connect(m_ui.adjustHeightBtn, &QPushButton::clicked, this, &QUserInterface::OnAdjustHeightBtnClicked);
    connect(m_ui.adjustToleranceBtn, &QPushButton::clicked, this, &QUserInterface::OnAdjustToleranceBtnClicked);
    connect(m_ui.adjustDensityBtn, &QPushButton::clicked, this, &QUserInterface::OnAdjustDensityBtnClicked);
    connect(m_ui.getCloudBtn, &QPushButton::clicked, this, &QUserInterface::OnGetCloudBtnClicked);

    connect(m_ui.pathSpacingSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &QUserInterface::OnPathSpacingChanged);

    setFixedSize(geometry().width(), geometry().height());

    m_plannerWidget.SetMissionSpacing(m_ui.pathSpacingSpinBox->value());
}

QUserInterface::~QUserInterface()
{
}

bool QUserInterface::event(QEvent* event)
{
    switch(event->type())
    {
    case UI_EVT_CONN:
    {
        QConnectionEvent* pConnEvt = static_cast<QConnectionEvent*>(event);
        bool bConnected = pConnEvt->m_bConnected;

        m_ui.connStateLabel->setText(bConnected ? "Connected" : "Disconnected");
        m_ui.centralwidget->setEnabled(bConnected);

        m_bNeedsUpdateMissionParams = bConnected;
        break;
    }
    case UI_EVT_DRONE_STATE:
    {
        QDroneStateEvent* pStateEvt = static_cast<QDroneStateEvent*>(event);
        SDroneState state = pStateEvt->m_state;

        if(m_bNeedsUpdateMissionParams)
        {
            m_ui.heightSpinBox->setValue(state.height);
            m_ui.toleranceSpinBox->setValue(state.tolerance);
            m_ui.densitySpinBox->setValue(state.density);
            m_bNeedsUpdateMissionParams = false;
        }
        
        QString posStr, rotStr, cloudStr;
        posStr.sprintf("Position: [%.2f, %.2f, %.2f]", state.x, state.y, state.z);
        rotStr.sprintf("Rotation: [%.2f, %.2f, %.2f]", state.roll, state.pitch, state.yaw);

        cloudStr.sprintf("Cloud size: %d (%s)",
            state.cloudSize,
            FormatBytes(sizeof(SPointCloud::CloudPt) * state.cloudSize).toStdString().c_str());

        m_ui.droneStatePos->setText(posStr);
        m_ui.droneStateRot->setText(rotStr);
        m_ui.droneStateCloud->setText(cloudStr);

        m_ui.getCloudBtn->setEnabled(m_ui.cloudDownloadBar->isEnabled() || state.cloudSize > 0);
        m_ui.chargeBar->setValue(state.charge * 100.f);

        m_ui.armBtn->setText(state.bArmed ? "Disarm" : "Arm");

        bool bWorking = state.systemState > ST_IDLE;
        bool bCanStart = state.systemState == ST_IDLE && state.missionHash == m_currentMission.hash && m_currentMission.hash != -1;
        bool bCanStop = state.systemState == ST_WORKING;

        m_ui.startBtn->setText(bWorking ? "Stop" : "Start");
        m_ui.startBtn->setEnabled(state.bArmed && (bCanStart || bCanStop));

        bool bCanAdjustHeight = !equal(float(m_ui.heightSpinBox->value()), state.height,
            float(m_ui.heightSpinBox->singleStep()) / 2.f);
        bool bCanAdjustTolerance = !equal(float(m_ui.toleranceSpinBox->value()), state.tolerance,
            float(m_ui.toleranceSpinBox->singleStep()) / 2.f);
        bool bCanAdjustDensity = !bWorking && !equal(float(m_ui.densitySpinBox->value()), state.density,
            float(m_ui.densitySpinBox->singleStep()) / 2.f);

        m_ui.adjustHeightBtn->setEnabled(bCanAdjustHeight);
        m_ui.adjustToleranceBtn->setEnabled(bCanAdjustTolerance);
        m_ui.adjustDensityBtn->setEnabled(bCanAdjustDensity);

        QGeoCoordinate dronePos = QGeoCoordinate(state.lat, state.lon);

        m_plannerWidget.SetEditable(!bWorking);
        m_plannerWidget.SetDronePos(dronePos);
        m_plannerWidget.SetDroneHeading(-state.heading);
        m_plannerWidget.SetDroneConnected(state.bGlobalPos);

        if(bWorking)
        {
            QGeoPath movementGeoPath = m_plannerWidget.GetMovementPath();
            movementGeoPath.addCoordinate(dronePos);
            int pathSize = movementGeoPath.path().size();
            if(pathSize == 1 || movementGeoPath.length(pathSize - 2, pathSize - 1) >= 1.0)
            {
                m_plannerWidget.SetMovementPath(movementGeoPath);
            }
        }
        break;
    }
    case UI_EVT_MISSION_CHANGED:
    {
        QMissionChangedEvent* pMissionEvt = static_cast<QMissionChangedEvent*>(event);
        m_currentMission = pMissionEvt->m_data;
        m_ui.sendPathBtn->setEnabled(!pMissionEvt->m_bWorking && m_currentMission.hash != -1);
        break;
    }
    case UI_EVT_MISSION_STARTED:
    {
        m_plannerWidget.SetMovementPath(QGeoPath());
        break;
    }
    case UI_EVT_GET_CLOUD_START:
    {
        m_ui.getCloudBtn->setText("Cancel");
        m_ui.cloudDownloadBar->setValue(0);
        m_ui.cloudDownloadBar->setEnabled(true);
        break;
    }
    case UI_EVT_GET_CLOUD_PERCENT:
    {
        QGetCloudPercentEvent* pCloudPercentEvent = static_cast<QGetCloudPercentEvent*>(event);
        m_ui.cloudDownloadBar->setValue(pCloudPercentEvent->m_percent);
        break;
    }
    case UI_EVT_GET_CLOUD_STOP:
    {
        m_ui.getCloudBtn->setText("Get cloud");
        m_ui.cloudDownloadBar->setValue(0);
        m_ui.cloudDownloadBar->setEnabled(false);
        break;
    }
    default:
        break;
    }

    return QMainWindow::event(event);
}

void QUserInterface::closeEvent(QCloseEvent* event)
{
    if(m_plannerWidget.isVisible())
    {
        m_plannerWidget.hide();
    }
    event->accept();
}

void QUserInterface::keyPressEvent(QKeyEvent* event)
{
    QMainWindow::keyPressEvent(event);

    if(!m_ui.centralwidget->isEnabled())
    {
        return;
    }

    switch(event->key())
    {
    case Qt::Key_A:
        OnArmBtnClicked();
        break;
    case Qt::Key_S:
        OnStartBtnClicked();
        break;
    default:
        break;
    }
}

void QUserInterface::OnArmBtnClicked()
{
    g_pCore->RequestArmDisarm();
}

void QUserInterface::OnStartBtnClicked()
{
    g_pCore->RequestStartStop();
}

void QUserInterface::OnSendPathBtnClicked()
{
    g_pCore->RequestSendMission();
}

void QUserInterface::OnAdjustHeightBtnClicked()
{
    g_pCore->RequestSendHeight(m_ui.heightSpinBox->value());
}

void QUserInterface::OnAdjustToleranceBtnClicked()
{
    g_pCore->RequestSendTolerance(m_ui.toleranceSpinBox->value());
}

void QUserInterface::OnAdjustDensityBtnClicked()
{
    g_pCore->RequestSendDensity(m_ui.densitySpinBox->value());
}

void QUserInterface::OnGetCloudBtnClicked()
{
    if(!m_ui.cloudDownloadBar->isEnabled())
    {
        QString filename = QFileDialog::getSaveFileName( 
            this, 
            tr("Save cloud as..."), 
            QDir::currentPath(), 
            tr("CSV files (*.csv);;All files (*.*)") );

        if(!filename.isNull())
        {
            g_pCore->RequestGetCloud(filename.toStdString());
        }
    }
    else
    {
        g_pCore->RequestStopGetCloud();
    }
}

void QUserInterface::OnPlannerBtnClicked()
{
    if(m_plannerWidget.isVisible())
    {
        m_plannerWidget.setWindowState(Qt::WindowActive);
    }
    else
    {
        m_plannerWidget.show();
    }
}

void QUserInterface::OnPathSpacingChanged(double val)
{
    m_plannerWidget.SetMissionSpacing(m_ui.pathSpacingSpinBox->value());
}

QString QUserInterface::FormatBytes(uint32_t bytesNum)
{
    QString unit("bytes");

    if(bytesNum < 1024)
    {
        // No conversion
        return QString().setNum(bytesNum) + " " + unit;
    }

    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";
    QStringListIterator i(list);

    float num = bytesNum;
    while(num >= 1024.f && i.hasNext())
    {
        unit = i.next();
        num /= 1024.f;
    }

    return QString().setNum(num, 'f', 2) + " " + unit;
}