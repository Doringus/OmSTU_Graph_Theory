import QtQuick
import QtQuick.Controls

import "../baseComponents"

Item {
    height:  120
    Row {
        anchors.fill: parent
        spacing: 20

        Text {
            color: "white"
            font.pointSize: 16
            text: "Число потоков"
        }
        RComboBox {

        }
    }
}
