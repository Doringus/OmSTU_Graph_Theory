import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts

import "../baseComponents"

Item {
    Rectangle {
        anchors.fill: parent
        color: "#3E424E"
        RowLayout {
            anchors.fill: parent
            spacing: 40
            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 20

                Row {
                    Layout.alignment: Qt.AlignTop
                    spacing: 15

                    Text {
                        text: qsTr("Бенчмарк")
                        color: "white"
                        font.pointSize: 16
                    }

                    BusyIndicator {
                        width: 50
                        height: 50
                        running: ProfilerBackend.isRunning

                    }

                    RButton {
                        height: 30
                        text: "Запустить"
                        onClicked: ProfilerBackend.startProfiler()
                        enabled: !ProfilerBackend.isRunning
                    }

                    RButton {
                        height: 30
                        width: 160
                        text: "Остановить"
                        onClicked: ProfilerBackend.stopProfiler()
                        visible: ProfilerBackend.isRunning
                    }
                }

                Text {
                    text: qsTr("Метод ветвей и границ")
                    color: "white"
                    font.pointSize: 16
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0

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
                                text: "Память, kb"
                            }
                        }
                    }

                    TableView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        boundsBehavior: Flickable.StopAtBounds
                        model: ProfilerBackend.profilerTable

                        delegate: Rectangle {
                            implicitWidth: 150
                            implicitHeight: 35
                            border.width: 1
                            border.color: "white"
                            color: "transparent"

                            Text {
                                anchors.centerIn: parent
                                font.pointSize: 16
                                color: "white"
                                text: display
                           }
                       }
                    }
                }
            }
            ///////////////////// GA /////////////////
            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.topMargin: 70
                spacing: 20

                Text {
                    text: qsTr("Генетический алгоритм")
                    color: "white"
                    font.pointSize: 16
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0

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
                                text: "Память, kb"
                            }
                        }
                    }

                    TableView {
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
                                font.pointSize: 16
                                color: "white"
                                text: display
                           }
                       }
                    }
                }
            }
        }
    }
}
