import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6


Map
{
    id: map
    plugin: Plugin { name: "osm" }
    center: planner.mapCenter
    zoomLevel: 14

    property var droneMarker
    property var workspacePoints: []

    MapPolygon{
        id: workspace
        color: planner.workspaceColor
        border.color: planner.workspaceBorderColor
        border.width: path.length > 2 ? 2 : 0
    }

    MapPolyline{
        id: missionPath
        line.width: path.length > 2 ? 2 : 0
        line.color: path.length > 2 ? planner.missionPathColor : "#00000000"
    }

    MapPolyline{
        id: movementPath
        line.width: path.length > 2 ? 2 : 0
        line.color: path.length > 2 ? planner.movementPathColor : "#00000000"
    }
    
    Component{
        id: point
        MapQuickItem {
            anchorPoint.x: circle.width * 0.5
            anchorPoint.y: circle.width * 0.5
            sourceItem: Rectangle {
                id: circle
                width: 16
                height: width
                radius: width * 0.5
                color: planner.workspacePointColor
                border.width: 0
            }
        }
    }

    Component{
        id: marker
        MapQuickItem {
            anchorPoint.x: image.width / 2
            anchorPoint.y: image.height / 2
            coordinate: planner.dronePos
            rotation: planner.droneHeading
            sourceItem: Image {
                id: image
                horizontalAlignment: Image.AlignHCenter
                verticalAlignment: Image.AlignVCenter
                width: 35
                height: 40
                source: "qrc:///arrow.png"
            }
        }
    }

    Connections{
        target: planner
        onWorkspaceChanged:
        {
            var coords = [];
            var size = planner.GetWorkspaceSize();

            // Remove workspace circles
            for(var i = 0; i < workspacePoints.length; ++i)
            {
                map.removeMapItem(workspacePoints[i]);
            }
            workspacePoints = []

            for(var i = 0; i < size; ++i)
            {
                // Add point
                var pos = planner.workspace.coordinateAt(i);
                var screenPos = map.fromCoordinate(pos, true);
                var pt = point.createObject(map);
                pt.coordinate = pos;
                map.addMapItem(pt);
                workspacePoints[i] = pt;

                // Add polygon path point
                coords[i] = pos;
            }
            workspace.path = coords;
        }

        onMissionPathChanged:
        {
            var coords = [];
            for(var i = 0; i < planner.GetMissionPathSize(); ++i)
            {
                coords[i] = planner.missionPath.coordinateAt(i);
            }
            missionPath.path = coords;
        }

        onMovementPathChanged:
        {
            var coords = [];
            for(var i = 0; i < planner.GetMovementPathSize(); ++i)
            {
                coords[i] = planner.movementPath.coordinateAt(i);
            }
            movementPath.path = coords;
        }

        onDroneConnectionChanged:
        {
            if(planner.droneConnected)
            {
                droneMarker = marker.createObject(map);
                map.addMapItem(droneMarker);
            }
            else
            {
                map.removeMapItem(droneMarker);
            }
        }
    }
}