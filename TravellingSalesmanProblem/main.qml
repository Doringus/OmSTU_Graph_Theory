import QtQuick 2.10
import QtQuick.Window 2.10

import GraphDrawer 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    GraphDrawer {
        id: sas
        anchors.fill: parent
    }
}
