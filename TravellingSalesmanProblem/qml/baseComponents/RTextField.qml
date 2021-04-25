import QtQuick
import QtQuick.Controls 2.12

TextField {
    id: root

    height: 40
    font.pointSize: 16
    selectByMouse: true
    color: "white"

    background: Rectangle {
        color: "#272A31"
        border.color: "#383B41"
        radius: 3
    }
}

