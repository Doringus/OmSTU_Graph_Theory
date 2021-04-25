import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../baseComponents"


Item {

    property alias populationSize: popSizeField.text
    property alias generations: generationsField.text

    height: 200

    Column {
        anchors.fill: parent
        GridLayout {
            columns: 2
            rows: 2
            columnSpacing: 15
            rowSpacing: 20

            Text {
                color: "white"
                font.pointSize: 16
                text: "Размер популяции"
            }

            RTextField {
                id: popSizeField
                text: "20"
                validator: IntValidator {
                    bottom: 5
                }
            }

            Text {
                color: "white"
                font.pointSize: 16
                text: "Число поколений"
            }

            RTextField {
                id: generationsField
                text: "60"
                validator: IntValidator {
                    bottom: 2
                }
            }
        }
    }
}
