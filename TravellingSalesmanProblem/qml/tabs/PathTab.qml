import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Qt.labs.platform

import GraphDrawer 1.0
import BranchAndBound 1.0
import Utils 1.0

import "../baseComponents"
import "../components"

Item { 

    FileDialog {
        id: graphMatrixDialog

        onAccepted: {
            Backend.openGraphMatrixFile(currentFile)
        }
    }

    FileDialog {
        id: penaltyMatrixDialog

        onAccepted: {
            Backend.openPenaltyMatrixFile(currentFile)
        }
    }

    FileDialog {
        id: savePdfDialog

        fileMode: FileDialog.SaveFile
        nameFilters: ["Pdf files (*.pdf)"]
        onAccepted: {
            layout.grabToImage(function(result) {
                writer.saveToFile(savePdfDialog.currentFile, result.image, Backend.optimalPathBB)
            })
        }
    }

    PdfWriter {
        id: writer
    }

    Connections {
        target: Backend
        function onAdjacencyMatrixLoaded(matrix) {
            layout.adjacencyMatrix = matrix
        }
        function onGraphPathChanged(path) {
            layout.drawPath(path)
        }
        function onTreeNodeReceived(node) {
            treeDrawer.root = node
        }
    }

    Popup {
        id: treePopup

        anchors.centerIn: parent
        width: parent.width - 300
        height: parent.height - 300
        ScrollView {
            anchors.fill: parent
            contentWidth:  treeDrawer.width
            contentHeight: treeDrawer.height
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            TreeDrawer {
                id: treeDrawer
            }
        }
    }


    Rectangle {
        anchors.fill: parent
        color: "#3E424E"
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        Rectangle {
            Layout.preferredWidth: parent.width * 0.6
            Layout.fillHeight: true
            color: "#3E424E"

            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"

                    FruchtermanReingold {
                        id: fr
                        width: layout.width - 30
                        height: layout.height - 30
                        showSteps: false
                    }

                    GraphLayout {
                        id: layout
                        anchors.fill: parent
                        algorithm: fr
                        drawEdges: false
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height * 0.25
                    color: "#1C1E23"

                    TextArea {
                        anchors.fill: parent
                        anchors.margins: 10
                        color: "white"
                        font.pointSize: 14
                        text: Backend.optimalPath
                        readOnly: true
                        background: Rectangle {
                            color: "transparent"
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 2
            Layout.fillHeight: true
            color: "#282C35"
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#33363F"
            Column {
                anchors.fill: parent
                spacing: 10

                RComboBox {
                    id: algorithmBox

                    height: 50
                    width: parent.width
                    model: ["Метод ветвей и границ", "Генетический алгоритм"]
                    Component.onCompleted: currentIndex = Backend.currentAlgorithmIndex
                    onActivated: Backend.currentAlgorithmIndex = currentIndex
                }

                //// Matrix input ////
                Rectangle {
                    id: matrixInput

                    height: 100
                    width: parent.width
                    color: "transparent"
                    GridLayout {
                        columns: 4
                        rows: 2
                        columnSpacing: 15
                        rowSpacing: 20

                        Text {
                            text: "Матрица"
                            font.pointSize: 16
                            color: "white"
                        }
                        RButton {
                            height: 30
                            text: "Загрузить"
                            onClicked: graphMatrixDialog.open()
                        }
                        Text {
                            Layout.row: 1
                            text: "Усложнение"
                            font.pointSize: 16
                            color: "white"
                        }
                        RButton {
                            height: 30
                            text: "Загрузить"
                            onClicked: penaltyMatrixDialog.open()
                        }
                        RButton {
                            height: 30
                            text: "Включить"
                            onClicked: {
                                if(text === "Включить") {
                                    text = "Выключить"
                                } else {
                                    text = "Включить"
                                }
                                Backend.enablePenalties()
                            }
                        }
                    }
                }
                //// Spacing ////
                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#42454A"
                }

                //// Algorithm settings ////
                Rectangle {
                    height: parent.height - start.height - matrixInput.height - 100
                    width: parent.width
                    color: "transparent"

                    StackLayout {
                        anchors.fill: parent
                        currentIndex: algorithmBox.currentIndex

                        Item {

                        }
                        GASettings {
                            id: gaSettings

                            Binding {
                                target: Backend.currentAlgorithm
                                property: "populationSize"
                                value: gaSettings.populationSize
                            }
                            Binding {
                                target: Backend.currentAlgorithm
                                property: "generations"
                                value: gaSettings.generations
                            }
                        }
                    }
                }

                //// Start ////
                RButton {
                    id: start
                    height: 60
                    width: parent.width * 0.7
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Запустить"
                    onClicked: Backend.currentAlgorithm.start(Backend.getLoadedMatrix())
                }
            }
        }
    }
}
