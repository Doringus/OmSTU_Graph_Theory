import QtQuick
import QtQuick.Controls

TabButton {
    id: control
    font.pointSize: 16
    hoverEnabled: true
    background: Rectangle {
       anchors.fill: parent
       color: "transparent"
       Rectangle {
           anchors.left: parent.left
           anchors.right: parent.right
           anchors.bottom: parent.bottom
           height: 2
           color: control.checked ? "white" : "transparent"
       }
    }
    contentItem: Text {
        font.pointSize: control.font.pointSize
        text: control.text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: control.checked ? "white" : "#7D7F83"
    }
}
