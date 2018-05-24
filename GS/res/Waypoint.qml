import QtQuick 2.7
import QtLocation 5.9
import QtPositioning 5.8

MapQuickItem {
        id: waypoint;
        property string label;
        property int r;
        property string pt_color;

        function remove(coord) {
//            missionPath.removeCoordinate(coord);

        }

        function moveTo(newCoord) {
            waypoint.coordinate = QtPositioning.coordinate(newCoord.x, newCoord.y);
            missionPath.replaceCoordinate(parseInt(objectName), waypoint.coordinate);
            setActive();
        }

        function setActive(){
            var coord = waypoint.coordinate.toString().split(",");
            var nextCoord = missionPath.coordinateAt(parseInt(objectName)+1);
            waypointInfo.label = "Waypoint #" + label;
            waypointInfo.coord = waypoint.coordinate;
            waypointInfo.lat = coord[0];
            waypointInfo.lon = coord[1];
            waypointInfo.heading = waypoint.coordinate.azimuthTo(nextCoord);
            waypointInfo.distanceToNext = waypoint.coordinate.distanceTo(nextCoord);
            map.setItemsInactive();
            map.prevWaypoint = waypoint;
            waypointInfo.visible = true;
            pt.border.width = 2;
            pt.border.color = map.activeItemColor;
          //  console.log("set Active",waypoint, "coord",waypoint.coordinate);
        }
        //DEBUG
        /*
        Component.onCompleted:
            console.log("construct waypt",waypoint, "coord", waypoint.coordinate)
        */

        sourceItem:
            Rectangle { id:pt;
                width: r; height: r; color: pt_color; border.width: 0;
                border.color: "white"; smooth: true; radius: r/2
                Text {
                    text:label
                    anchors.centerIn: parent
                    font.bold: true
                    color:"white";
                }
                MouseArea {
                    anchors.fill:parent;
                    onClicked: {
                        setActive();
                    }
                }
            }
        opacity: 1.0
        anchorPoint: Qt.point(sourceItem.width/2, sourceItem.height/2)
}
