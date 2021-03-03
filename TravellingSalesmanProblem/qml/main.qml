import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import GraphDrawer 1.0

import "tabs"

ApplicationWindow {
    width: 1200
    height: 950
    visible: true
    title: qsTr("Travelling salesman")

    header: Rectangle {
        height: 40
        color: "#1C1E23"
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: 0

        PathTab {

        }
    }

    /*FruchtermanReingold {
        id: fr
        width: layout.width - 30
        height: layout.height - 30
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
    }*/

}
