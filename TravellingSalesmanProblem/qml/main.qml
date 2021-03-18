import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import GraphDrawer 1.0

import "tabs"
import "baseComponents"

ApplicationWindow {
    width: 1400
    height: 950
    visible: true
    title: qsTr("Travelling salesman")

    header: Rectangle {
        height: 40
        color: "#1C1E23"
        TabBar {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 800
            background: Rectangle {
                color: "transparent"
            }
            RTabButton {
                text: "Путь"
                height: parent.height
                onClicked: {
                    stack.currentIndex = 0;
                }
            }
            RTabButton {
                text: "BB"
                height: parent.height
                onClicked: {
                    stack.currentIndex = 1;
                }
            }
            RTabButton {
                text: "Генетический алгоритм"
                height: parent.height
            }
        }
    }

    StackLayout {
        id: stack
        anchors.fill: parent
        currentIndex: 1

        PathTab {

        }

        BBProfiler {

        }
    }
}
