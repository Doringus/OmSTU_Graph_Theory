import QtQuick 2.10
import QtQuick.Window 2.10

import GraphDrawer 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    /*GraphDrawer {
        id: sas
        anchors.fill: parent
    }*/
    FruchtermanReingold {
        id: fr
        width: layout.width
        height: layout.height
        showSteps: true
        adjacencyMatrix: [
            [0, 1, 0, 0, 1, 1, 1],
            [1, 0, 1, 0, 1, 0, 0],
            [0, 1, 0, 1, 0, 0, 0],
            [0, 0, 1, 0, 1, 0, 0],
            [1, 1, 0, 1, 0, 1, 0],
            [1, 0, 0, 0, 1, 0, 0],
            [1, 0, 0, 0, 0, 0, 0]
        ]
        Component.onCompleted: fr.start()
    }
    GraphLayout {
        id: layout
        anchors.fill: parent
        algorithm: fr
    }
}
