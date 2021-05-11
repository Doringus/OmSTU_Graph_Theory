import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts

import "../baseComponents"
import "../components"

Item {
    Rectangle {
        anchors.fill: parent
        color: "#3E424E"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            ProfilerSection {
                name: "BB"
                model: ProfilerBackend.bbProfilerTable
                onStart: ProfilerBackend.startBbProfiler()
            }
            ProfilerSection {
                name: "GA"
                model: ProfilerBackend.gaProfilerTable
                onStart: ProfilerBackend.startGaProfiler()
            }
            ProfilerSection {
                name: "GA Cuda"
                model: ProfilerBackend.cudaProfilerTable
                onStart: ProfilerBackend.startCudaProfiler()
            }
        }
    }
}


