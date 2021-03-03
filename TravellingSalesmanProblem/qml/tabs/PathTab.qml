import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels 1.0

import GraphDrawer 1.0

import "../baseComponents"

Item { 

    Rectangle {
        anchors.fill: parent
        color: "#3E424E"
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        /* Matrix input */
        Rectangle {
            Layout.preferredWidth: parent.width / 2
            Layout.fillHeight: true
            color: "transparent"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                /* Graph matrix */
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 20

                        Row {
                            spacing: 15
                            Text {
                                text: qsTr("Матрица графа")
                                color: "white"
                                font.pointSize: 16
                            }

                            RButton {
                                height: 30
                                text: "Загрузить"
                            }
                        }

                        TableView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            boundsBehavior: Flickable.StopAtBounds
                            model: TableModel {
                                   TableModelColumn { display: "name" }
                                   TableModelColumn { display: "color" }

                                   rows: [
                                       {
                                           "name": "1",
                                           "color": "1"
                                       },
                                       {
                                           "name": "0",
                                           "color": "1"
                                       },
                                       {
                                           "name": "1",
                                           "color": "1"
                                       }
                                   ]
                               }

                            delegate: Rectangle {
                                implicitWidth: 30
                                implicitHeight: 30
                                border.width: 1
                                border.color: "white"
                                color: "transparent"

                                Text {
                                    anchors.centerIn: parent
                                    text: display
                                    font.pointSize: 16
                                    color: "white"
                               }
                           }
                        }
                    }




                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight:  1
                    color: "#CCCDCF"
                }

                /* Amplification factor matrix */
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"

                    Row {
                        spacing: 15
                        Text {
                            text: qsTr("Матрица усложнения")
                            color: "white"
                            font.pointSize: 16
                        }
                        RButton {
                            height: 30
                            text: "Загрузить"
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: parent.width / 2
            Layout.fillHeight: true
            border.color: "#33363F"
            border.width: 15
            color: "#272A31"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight:  45
                    color: "#33363F"

                    Text {
                        anchors.fill: parent
                        color: "white"
                        font.pointSize: 18
                        text: "Граф"
                    }
                }

                /* Graph */
                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 400
                    color: "transparent"

                    FruchtermanReingold {
                            id: fr
                            width: layout.width - 45
                            height: layout.height - 45
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

                /* Path algorithms */

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight:  45
                    color: "#33363F"
                    Row {
                        anchors.fill: parent
                        spacing: 10

                        Text {
                            color: "white"
                            font.pointSize: 18
                            text: "Метод ветвей и границ"
                        }

                        RButton {
                            anchors.verticalCenter: parent.verticalCenter
                            height: 30
                            text: "Запустить"
                        }

                        RButton {
                            anchors.verticalCenter: parent.verticalCenter
                            height: 30
                            width: 120
                            text: "Подробнее"
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight:  45
                    color: "#33363F"

                    Row {
                        anchors.fill: parent
                        spacing: 10

                        Text {
                            color: "white"
                            font.pointSize: 18
                            text: "Генетический алгоритм"
                        }

                        RButton {
                            anchors.verticalCenter: parent.verticalCenter
                            height: 30
                            text: "Запустить"
                        }

                        RButton {
                            anchors.verticalCenter: parent.verticalCenter
                            height: 30
                            width: 120
                            text: "Подробнее"
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                }
            }
        }
    }
}