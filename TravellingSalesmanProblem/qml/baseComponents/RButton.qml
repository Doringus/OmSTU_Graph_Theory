import QtQuick
import QtQuick.Controls.Universal

Item {
    id: root

    property color color: "#7289DA"
    property color hoveredColor: "#677BC4"
    property color pressedColor: "#5B6EAE"
    property color textColor: "white"

    property alias text: buttonText.text
    property alias font: buttonText.font

    signal clicked()

    width: 100
    height: 50


    Rectangle {
        id: shape
        anchors.fill: parent
        color: root.color
        radius: 3

        Text {
            id: buttonText
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: root.textColor
            elide: Text.ElideRight
            font.pointSize: 14
            font.family: "Roboto"
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onEntered: {
                root.state = "hovered"
            }
            onExited: {
                root.state = ""
            }
            onClicked: {
                root.clicked()
            }
            onPressed: {
                root.state = "pressed"
            }
            onReleased: {
                if(containsMouse) {
                    root.state = "hovered"
                } else {
                    root.state = ""
                }
            }
        }
    }
    states: [
        State {
            name: "hovered"
            PropertyChanges {
                target: shape
                color: hoveredColor
            }
        },
        State {
            name: "pressed"
            PropertyChanges {
                target: shape
                color: pressedColor

            }
        }
    ]
    transitions: [
        Transition {
            ColorAnimation {
                duration: 200
            }

        }
    ]
}
