QT += quick qml concurrent gui

CONFIG += c++17
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Cuda settings
CUDA_SOURCES += \
    src/cuda/ga.cu

MSVCRT_LINK_FLAG_DEBUG   = "/MDd"
MSVCRT_LINK_FLAG_RELEASE = "/MD"

CUDA_DIR = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.3"
SYSTEM_NAME = x64
SYSTEM_TYPE = 64
CUDA_ARCH = sm_61
NVCC_OPTIONS = --use_fast_math

INCLUDEPATH += $$CUDA_DIR/include \
               $$CUDA_DIR/common/inc \
               $$CUDA_DIR/../shared/inc

QMAKE_LIBDIR += $$CUDA_DIR/lib/$$SYSTEM_NAME \
                $$CUDA_DIR/common/lib/$$SYSTEM_NAME \
                $$CUDA_DIR/../shared/lib/$$SYSTEM_NAME

CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')

CUDA_LIB_NAMES = cudart_static kernel32 user32 gdi32 winspool comdlg32 \
                 advapi32 shell32 ole32 oleaut32 uuid odbc32 odbccp32 \


for(lib, CUDA_LIB_NAMES) {
    CUDA_LIBS += -l$$lib
}

LIBS += $$CUDA_LIBS

CONFIG(debug, debug|release) {
    # Debug mode
    cuda_d.input = CUDA_SOURCES
    cuda_d.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.obj
    cuda_d.commands = $$CUDA_DIR/bin/nvcc.exe -D_DEBUG $$NVCC_OPTIONS $$CUDA_INC $$LIBS \
                      --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH \
                      --compile -cudart static -g -DWIN32 -D_MBCS \
                      -Xcompiler "/wd4819,/EHsc,/W3,/nologo,/Od,/Zi,/RTC1" \
                      -Xcompiler $$MSVCRT_LINK_FLAG_DEBUG \
                      -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda_d.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda_d
}
else {
    # Release mode
    cuda.input = CUDA_SOURCES
    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.obj
    cuda.commands = $$CUDA_DIR/bin/nvcc.exe $$NVCC_OPTIONS $$CUDA_INC $$LIBS \
                    --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH \
                    --compile -cudart static -DWIN32 -D_MBCS \
                    -Xcompiler "/wd4819,/EHsc,/W3,/nologo,/O2,/Zi" \
                    -Xcompiler $$MSVCRT_LINK_FLAG_RELEASE \
                    -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda
}
# End cuda settings


SOURCES += \
        src/backend.cpp \
        src/bbexecutor.cpp \
        src/bbprofiler.cpp \
        src/branchandbound.cpp \
        src/fruchtermanreingold.cpp \
        src/ga.cpp \
        src/gaexecutor.cpp \
        src/gafunctions.cpp \
        src/graphdrawer.cpp \
        src/graphlayout.cpp \
        src/main.cpp \
        src/pdfwriter.cpp \
        src/profilerbackend.cpp \
        src/staticthreadpool.cpp \
        src/tablemodel.cpp \
        src/taskqueue.cpp \
        src/treedrawer.cpp

RESOURCES += qml.qrc
QMAKE_CXXFLAGS += -O2
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/backend.h \
    src/bbexecutor.h \
    src/bbprofiler.h \
    src/branchandbound.h \
    src/common.h \
    src/drawingalgorithm.h \
    src/fruchtermanreingold.h \
    src/ga.h \
    src/gaexecutor.h \
    src/gafunctions.h \
    src/graphdrawer.h \
    src/graphlayout.h \
    src/ialgorithm.h \
    src/matrixloader.h \
    src/matrixmultiplier.h \
    src/pdfwriter.h \
    src/profilerbackend.h \
    src/staticthreadpool.h \
    src/tablemodel.h \
    src/taskqueue.h \
    src/treedrawer.h

DISTFILES += \
    src/cuda/ga.cu
