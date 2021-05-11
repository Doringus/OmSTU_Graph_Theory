import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts

import "../baseComponents"

ColumnLayout {
    id: root

    property alias name: sectionText.text
    property alias model: table.model
    signal start()
    signal stop()

    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 0

    Row {
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        spacing: 20

        Text {
            id: sectionText
            color: "white"
            font.pointSize: 16
            text: "BB"
        }
        RButton {
            height: 30
            text: "Старт"
            onClicked: root.start()
        }
        RButton {
            height: 30
            text: "Стоп"
            onClicked: root.stop()
        }
    }

    Row {
       Layout.fillWidth: true
       Layout.preferredHeight: 35

       Rectangle {
            implicitWidth: 150
            implicitHeight: 35
            border.width: 1
            border.color: "white"
            color: "transparent"

            Text {
                anchors.centerIn: parent
                font.pointSize: 16
                color: "white"
                text: "N"
            }
        }

       Rectangle {
            implicitWidth: 150
            implicitHeight: 35
            border.width: 1
            border.color: "white"
            color: "transparent"

            Text {
                anchors.centerIn: parent
                font.pointSize: 16
                color: "white"
                text: "Время, ms"
            }
        }

       Rectangle {
            implicitWidth: 150
            implicitHeight: 35
            border.width: 1
            border.color: "white"
            color: "transparent"

            Text {
                anchors.centerIn: parent
                font.pointSize: 16
                color: "white"
                text: "Путь"
            }
        }
    }

    TableView {
        id: table
        Layout.fillWidth: true
        Layout.fillHeight: true
        boundsBehavior: Flickable.StopAtBounds
     //   model: ProfilerBackend.profilerTable

        delegate: Rectangle {
            implicitWidth: 150
            implicitHeight: 35
            border.width: 1
            border.color: "white"
            color: "transparent"

            Text {
                anchors.centerIn: parent
                width: parent.width
                font.pointSize: 16
                color: "white"
                text: display
                elide: Text.ElideRight
           }
       }
    }
}
