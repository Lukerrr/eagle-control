#include "PlannerWidget.h"
#include "Core.h"
#include "Log.h"
#include "Line2D.h"
#include <QQuickItem>
#include <QGeoRectangle>
#include <QQmlContext>
#include <QPainter>

#include <math.h>

QPlannerWidget::QPlannerWidget(QWidget* parent /*= Q_NULLPTR*/)
{
    m_ui.setupUi(this);
    
    SetMapCenter(QGeoCoordinate(MAP_DEFAULT_CENTER));
    SetWorkspaceColor(QColor(WORKSPACE_CLR_VALID));
    SetWorkspaceBorderColor(QColor(WORKSPACE_CLR_BRD_VALID));
    SetWorkspacePointColor(QColor(WORKSPACE_CLR_PT_VALID));
    SetMissionPathColor(QColor(MISSION_PATH_CLR));
    SetMovementPathColor(QColor(MOVEMENT_PATH_CLR));

    rootContext()->setContextProperty("planner", this);
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSource(QUrl(QLatin1String("qrc:/main.qml")));
}

QPlannerWidget::~QPlannerWidget()
{
}

void QPlannerWidget::mousePressEvent(QMouseEvent* event)
{
    if((!m_bDragMode && !m_bAddMode) || !m_bEditable)
    {
        QQuickWidget::mousePressEvent(event);
        return;
    }

    // Whether the workspace was updated
    bool bRedrawWorkspace = false;

    switch(event->button())
    {
    case Qt::MouseButton::LeftButton:
    {
        int x = event->x();
        int y = event->y();
        double lat, lon;
        if(ToWorldSpace(x, y, lat, lon))
        {
            // Add new point into the workspace
            if(m_bAddMode)
            {
                int newPointId = m_workspace.InsertPoint(Vec2(lat, lon));

                // Drag a new point
                m_nDragPoint = newPointId;
                m_bDrag = true;

                bRedrawWorkspace = true;
            }

            // Drag the nearest point from the workspace
            if (m_bDragMode)
            {
                int nNearPtIdx = GetNearestPointId(x, y);
                if (nNearPtIdx >= 0)
                {
                    // Mark the point as dragged
                    m_nDragPoint = nNearPtIdx;
                    m_bDrag = true;

                    m_workspace.ReplacePoint(m_nDragPoint, Vec2(lat, lon));

                    bRedrawWorkspace = true;
                }
            }
        }
        
        break;
    }
    case Qt::MouseButton::RightButton:
    {
        if(m_bAddMode)
        {
            // Remove nearest point
            int nNearPtIdx = GetNearestPointId(event->x(), event->y());
            if (nNearPtIdx >= 0)
            {
                m_bDrag = false;
                m_workspace.RemovePoint(nNearPtIdx);
                bRedrawWorkspace = true;
            }
        }
        break;
    }
    default:
        break;
    }

    // Update workspace in QML if necessary
    if(bRedrawWorkspace)
    {
        UpdateWorkspace();
    }
}

void QPlannerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QQuickWidget::mouseReleaseEvent(event);

    if (event->button() == Qt::MouseButton::LeftButton)
    {
        m_bDrag = false;
    }
}

void QPlannerWidget::keyPressEvent(QKeyEvent* event)
{
    QQuickWidget::keyPressEvent(event);

    switch(event->key())
    {
    case Qt::Key_Shift:
        m_bDragMode = true;
        break;
    case Qt::Key_Control:
        m_bAddMode = true;
        break;
    case Qt::Key_R:
        if(m_bDroneConnected)
        {
            SetMapCenter(m_dronePos);
            update();
        }
        break;
    default:
        break;
    }
}

void QPlannerWidget::keyReleaseEvent(QKeyEvent* event)
{
    QQuickWidget::keyReleaseEvent(event);

    switch(event->key())
    {
    case Qt::Key_Shift:
        m_bDragMode = false;
        break;
    case Qt::Key_Control:
        m_bAddMode = false;
        break;
    default:
        break;
    }
}

void QPlannerWidget::mouseMoveEvent(QMouseEvent* event)
{
    if((!m_bDragMode && !m_bAddMode) || !m_bEditable)
    {
        QQuickWidget::mouseMoveEvent(event);
        return;
    }

    int x = event->x();
    int y = event->y();
    double lat, lon;

    if (m_bDrag && ToWorldSpace(x, y, lat, lon))
    {
        m_workspace.ReplacePoint(m_nDragPoint, Vec2(lat, lon));
        UpdateWorkspace();
    }
}

void QPlannerWidget::SetEditable(bool bEditable)
{
    if(m_bEditable == bEditable)
    {
        return;
    }

    m_bEditable = bEditable;

    setCursor(QCursor(m_bEditable ? Qt::CrossCursor : Qt::ArrowCursor));
}

void QPlannerWidget::SetMissionSpacing(double spacing)
{
    m_pathSpacing = spacing;
    UpdateMissionPath();
}

int QPlannerWidget::GetNearestPointId(int x, int y)
{
    int nNearPtIdx = -1;
    int nearPtSqDist = 0.;

    int size = m_workspace.GetSize();
    
    for (int i = 0; i < size; ++i)
    {
        Vec2 pt = m_workspace.GetPoint(i);
        double ptX = 0., ptY = 0.;
        ToScreenSpace(pt.x, pt.y, ptX, ptY);
        int nDistX = ptX - x, nDistY = ptY - y;
        int ptSqDist = nDistX * nDistX + nDistY * nDistY;
        if (MAX_DIST_TO_REMOVE_POINT * MAX_DIST_TO_REMOVE_POINT >= ptSqDist)
        {
            if (nNearPtIdx == -1 || ptSqDist < nearPtSqDist)
            {
                nearPtSqDist = ptSqDist;
                nNearPtIdx = i;
            }
        }
    }
    return nNearPtIdx;
}

bool QPlannerWidget::ToWorldSpace(int x, int y, double& lat, double& lon)
{
    if(x < 0 || y < 0 || x >= width() || y >= height())
    {
        return false;
    }

    QQuickItem* mapObj = rootObject();
    QGeoCoordinate coords;
    QPointF pt(x, y);
    bool bResult = QMetaObject::invokeMethod(mapObj, "toCoordinate", Q_RETURN_ARG(QGeoCoordinate, coords), Q_ARG(QPointF, pt));
    if(bResult)
    {
        lat = coords.latitude();
        lon = coords.longitude();
    }
    return bResult;
}

bool QPlannerWidget::ToScreenSpace(double lat, double lon, double& x, double& y)
{
    QQuickItem* mapObj = rootObject();
    QGeoCoordinate coords(lat, lon);
    QPointF pt;
    bool bResult = QMetaObject::invokeMethod(mapObj, "fromCoordinate", Q_RETURN_ARG(QPointF, pt), Q_ARG(QGeoCoordinate, coords), Q_ARG(bool, false));
    if(bResult)
    {
        x = pt.x();
        y = pt.y();
    }
    return bResult;
}

int QPlannerWidget::GetPathOrientation(CLinePath2D linePath)
{
    // Find signed area
    vector<Vec2> points = m_workspace.GetPoints();
    double area = 0.;
    for(int i = 1; i <= points.size(); ++i)
    {
        double x1, x2, y1, y2;

        Vec2 pt1 = m_workspace.GetPoint(i-1);
        Vec2 pt2 = m_workspace.GetPoint(i == points.size() ? 0 : i);

        ToScreenSpace(pt1.x, pt1.y, x1, y1);
        ToScreenSpace(pt2.x, pt2.y, x2, y2);

        area += x1 * y2 - x2 * y1;
    }

    // Get path orientation
    return sign(area);
}

QGeoPath QPlannerWidget::GetGeoPath(CLinePath2D linePath)
{
    QGeoPath geoPath = QGeoPath();
    for(Vec2 pt : linePath.GetPoints())
    {
        geoPath.addCoordinate(QGeoCoordinate(pt.x, pt.y));
    }
    return geoPath;
}

void QPlannerWidget::UpdateWorkspace()
{
    SetWorkspace(GetGeoPath(m_workspace));
    UpdateMissionPath();
}

void QPlannerWidget::UpdateMissionPath()
{
    bool bMissionValid = m_workspace.GetSize() > 2 && !m_workspace.IntersectsSelf();

    m_missionPath = CLinePath2D();

    if(bMissionValid)
    {
        double spacingDeg = rad2deg(m2geo(m_pathSpacing, m_workspaceGeo.center().latitude()));
        QGeoRectangle boundingBox = m_workspaceGeo.boundingGeoRectangle();
        QGeoCoordinate minGeo = boundingBox.bottomLeft();
        QGeoCoordinate maxGeo = boundingBox.topRight();
        Vec2 min(minGeo.latitude(), minGeo.longitude());
        Vec2 max(maxGeo.latitude(), maxGeo.longitude());

        m_missionPath = CLinePath2D(m_workspace.GetPointsCloud(min, max, spacingDeg));
    }

    bMissionValid &= m_missionPath.GetSize() > 2;

    SetWorkspaceColor(QColor(bMissionValid ? WORKSPACE_CLR_VALID : WORKSPACE_CLR_INVALID));
    SetWorkspaceBorderColor(QColor(bMissionValid ? WORKSPACE_CLR_BRD_VALID : WORKSPACE_CLR_BRD_INVALID));
    SetWorkspacePointColor(QColor(bMissionValid ? WORKSPACE_CLR_PT_VALID : WORKSPACE_CLR_PT_INVALID));

    SetMissionPath(GetGeoPath(m_missionPath));

    g_pCore->SetMissionPath(bMissionValid ? m_missionPath : CLinePath2D());
}

QGeoPath QPlannerWidget::GetWorkspace() const
{
    return m_workspaceGeo;
}

void QPlannerWidget::SetWorkspace(const QGeoPath& workspace)
{
    if(m_workspaceGeo == workspace)
    {
        return;
    }
    m_workspaceGeo = workspace;
    emit workspaceChanged();
}

QGeoPath QPlannerWidget::GetMissionPath() const
{
    return m_missionPathGeo;
}

void QPlannerWidget::SetMissionPath(const QGeoPath& missionPath)
{
    if(m_missionPathGeo == missionPath)
    {
        return;
    }
    m_missionPathGeo = missionPath;
    emit missionPathChanged();
}

QGeoPath QPlannerWidget::GetMovementPath() const
{
    return m_movementPathGeo;
}

void QPlannerWidget::SetMovementPath(const QGeoPath& movementPath)
{
    if(m_movementPathGeo == movementPath)
    {
        return;
    }
    m_movementPathGeo = movementPath;
    emit movementPathChanged();
}

QGeoCoordinate QPlannerWidget::GetMapCenter() const
{
    return m_mapCenter;
}

void QPlannerWidget::SetMapCenter(const QGeoCoordinate& mapCenter)
{
    if(m_mapCenter == mapCenter)
    {
        return;
    }
    m_mapCenter = mapCenter;
    emit mapCenterChanged();
}

QGeoCoordinate QPlannerWidget::GetDronePos() const
{
    return m_dronePos;
}

void QPlannerWidget::SetDronePos(const QGeoCoordinate& dronePos)
{
    if(m_dronePos == dronePos)
    {
        return;
    }
    m_dronePos = dronePos;
    emit dronePosChanged();
}

float QPlannerWidget::GetDroneHeading() const
{
    return m_droneHeading;
}

void QPlannerWidget::SetDroneHeading(const float& droneHeading)
{
    if(m_droneHeading == droneHeading)
    {
        return;
    }
    m_droneHeading = droneHeading;
    emit droneHeadingChanged();
}

bool QPlannerWidget::IsDroneConnected() const
{
    return m_bDroneConnected;
}

void QPlannerWidget::SetDroneConnected(const bool& bDroneConnected)
{
    if(m_bDroneConnected == bDroneConnected)
    {
        return;
    }
    m_bDroneConnected = bDroneConnected;
    m_ui.noGpsLabel->setVisible(!bDroneConnected);
    emit droneConnectionChanged();
}

QColor QPlannerWidget::GetWorkspaceColor() const
{
    return m_workspaceColor;
}

void QPlannerWidget::SetWorkspaceColor(const QColor& workspaceColor)
{
    if(m_workspaceColor == workspaceColor)
    {
        return;
    }
    m_workspaceColor = workspaceColor;
    emit workspaceColorChanged();
}

QColor QPlannerWidget::GetWorkspaceBorderColor() const
{
    return m_workspaceBorderColor;
}

void QPlannerWidget::SetWorkspaceBorderColor(const QColor& workspaceBorderColor)
{
    if(m_workspaceBorderColor == workspaceBorderColor)
    {
        return;
    }
    m_workspaceBorderColor = workspaceBorderColor;
    emit workspaceBorderColorChanged();
}

QColor QPlannerWidget::GetWorkspacePointColor() const
{
    return m_workspacePointColor;
}

void QPlannerWidget::SetWorkspacePointColor(const QColor& workspacePointColor)
{
    if(m_workspacePointColor == workspacePointColor)
    {
        return;
    }
    m_workspacePointColor = workspacePointColor;
    emit workspacePointColorChanged();
}

QColor QPlannerWidget::GetMissionPathColor() const
{
    return m_missionPathColor;
}

void QPlannerWidget::SetMissionPathColor(const QColor& missionPathColor)
{
    if(m_missionPathColor == missionPathColor)
    {
        return;
    }
    m_missionPathColor = missionPathColor;
    emit missionPathColorChanged();
}

QColor QPlannerWidget::GetMovementPathColor() const
{
    return m_movementPathColor;
}

void QPlannerWidget::SetMovementPathColor(const QColor& movementPathColor)
{
    if(m_movementPathColor == movementPathColor)
    {
        return;
    }
    m_movementPathColor = movementPathColor;
    emit movementPathColorChanged();
}

int QPlannerWidget::GetWorkspaceSize()
{
    return m_workspace.GetSize();
}

int QPlannerWidget::GetMissionPathSize()
{
    return m_missionPath.GetSize();
}

int QPlannerWidget::GetMovementPathSize()
{
    return m_movementPathGeo.path().size();
}