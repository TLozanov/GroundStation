import QtQuick 2.0
import QtLocation 5.9

ItemInfo {
    width: 155; height: 63
    anchors.horizontalCenter: parent.horizontalCenter;
    anchors.bottom: parent.bottom;
    property string lat;
    property string lon;
    property var coord;
    Item {
        anchors.fill: parent;
        Column {
            anchors.bottom: parent.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.bottomMargin: 10;
            Text { text: '<b>Lat:</b> ' + lat; color: "white";}
            Text { text: '<b>Lon:</b> ' + lon; color: "white";}
        }
    }
}
