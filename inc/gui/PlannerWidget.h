#pragma once

#include "generated/ui_PlannerWidget.h"
#include "LinePath2D.h"
#include <QQuickWidget>
#include <QPolygonF>
#include <QGeoPath>

#define MAX_DIST_TO_REMOVE_POINT 100
#define SIZE_POINT 8

#define MAP_DEFAULT_CENTER 43.03f, 131.89f

// #ARGB hex colors
#define WORKSPACE_CLR_VALID "#3015EB43"
#define WORKSPACE_CLR_INVALID "#30EB1543"
#define WORKSPACE_CLR_BRD_VALID "#A80E7D26"
#define WORKSPACE_CLR_BRD_INVALID "#A87D0E26"
#define WORKSPACE_CLR_PT_VALID "#FF0E7D26"
#define WORKSPACE_CLR_PT_INVALID "#FF7D0E26"
#define MISSION_PATH_CLR "#B8204BEC"
#define MOVEMENT_PATH_CLR "#B8EB4034"

class QPlannerWidget : public QQuickWidget
{
    Q_OBJECT
    Q_PROPERTY(QGeoPath workspace READ GetWorkspace WRITE SetWorkspace NOTIFY workspaceChanged)
    Q_PROPERTY(QGeoPath missionPath READ GetMissionPath WRITE SetMissionPath NOTIFY missionPathChanged)
    Q_PROPERTY(QGeoPath movementPath READ GetMovementPath WRITE SetMovementPath NOTIFY movementPathChanged)
    Q_PROPERTY(QGeoCoordinate mapCenter READ GetMapCenter WRITE SetMapCenter NOTIFY mapCenterChanged)
    Q_PROPERTY(QGeoCoordinate dronePos READ GetDronePos WRITE SetDronePos NOTIFY dronePosChanged)
    Q_PROPERTY(float droneHeading READ GetDroneHeading WRITE SetDroneHeading NOTIFY droneHeadingChanged)
    Q_PROPERTY(bool droneConnected READ IsDroneConnected WRITE SetDroneConnected NOTIFY droneConnectionChanged)
    Q_PROPERTY(QColor workspaceColor READ GetWorkspaceColor WRITE SetWorkspaceColor NOTIFY workspaceColorChanged)
    Q_PROPERTY(QColor workspaceBorderColor READ GetWorkspaceBorderColor WRITE SetWorkspaceBorderColor NOTIFY workspaceBorderColorChanged)
    Q_PROPERTY(QColor workspacePointColor READ GetWorkspacePointColor WRITE SetWorkspacePointColor NOTIFY workspacePointColorChanged)
    Q_PROPERTY(QColor missionPathColor READ GetMissionPathColor WRITE SetMissionPathColor NOTIFY missionPathColorChanged)
    Q_PROPERTY(QColor movementPathColor READ GetMovementPathColor WRITE SetMovementPathColor NOTIFY movementPathColorChanged)

public:
    QPlannerWidget(QWidget* parent = Q_NULLPTR);
    ~QPlannerWidget();

    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

    void SetEditable(bool bEditable);
    void SetMissionSpacing(double spacing);

    QGeoPath GetWorkspace() const;
    void SetWorkspace(const QGeoPath& workspace);

    QGeoPath GetMissionPath() const;
    void SetMissionPath(const QGeoPath& missionPath);

    QGeoPath GetMovementPath() const;
    void SetMovementPath(const QGeoPath& movementPath);

    QGeoCoordinate GetMapCenter() const;
    void SetMapCenter(const QGeoCoordinate& mapCenter);

    QGeoCoordinate GetDronePos() const;
    void SetDronePos(const QGeoCoordinate& dronePos);

    float GetDroneHeading() const;
    void SetDroneHeading(const float& droneHeading);

    bool IsDroneConnected() const;
    void SetDroneConnected(const bool& bDroneConnected);

    QColor GetWorkspaceColor() const;
    void SetWorkspaceColor(const QColor& workspaceColor);

    QColor GetWorkspaceBorderColor() const;
    void SetWorkspaceBorderColor(const QColor& workspaceBorderColor);

    QColor GetWorkspacePointColor() const;
    void SetWorkspacePointColor(const QColor& workspacePointColor);

    QColor GetMissionPathColor() const;
    void SetMissionPathColor(const QColor& missionPathColor);

    QColor GetMovementPathColor() const;
    void SetMovementPathColor(const QColor& movementPathColor);

    Q_INVOKABLE int GetWorkspaceSize();
    Q_INVOKABLE int GetMissionPathSize();
    Q_INVOKABLE int GetMovementPathSize();

private:
    bool ToWorldSpace(int x, int y, double& lat, double& lon);
    bool ToScreenSpace(double lat, double lon, double& x, double& y);
    int GetNearestPointId(int x, int y);
    int GetPathOrientation(CLinePath2D linePath);
    QGeoPath GetGeoPath(CLinePath2D linePath);

    void UpdateWorkspace();
    void UpdateMissionPath();

    Ui::PlannerWidget m_ui;

    CLinePath2D m_workspace;
    CLinePath2D m_missionPath;

    QGeoPath m_workspaceGeo;
    QGeoPath m_missionPathGeo;
    QGeoPath m_movementPathGeo;

    QGeoCoordinate m_mapCenter;
    QGeoCoordinate m_dronePos;
    float m_droneHeading;

    QColor m_workspaceColor;
    QColor m_workspaceBorderColor;
    QColor m_workspacePointColor;
    QColor m_missionPathColor;
    QColor m_movementPathColor;

    bool m_bEditable = true;
    bool m_bDragMode = false;
    bool m_bAddMode = false;
    bool m_bDrag = false;
    uint8_t m_nDragPoint = 0;

    double m_pathSpacing = 100.;

    bool m_bDroneConnected = false;    
signals:
    void workspaceChanged();
    void missionPathChanged();
    void movementPathChanged();
    void mapCenterChanged();
    void dronePosChanged();
    void droneHeadingChanged();
    void workspaceColorChanged();
    void workspaceBorderColorChanged();
    void workspacePointColorChanged();
    void missionPathColorChanged();
    void movementPathColorChanged();
    void droneConnectionChanged();
};