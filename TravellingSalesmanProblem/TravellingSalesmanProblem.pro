QT += quick qml concurrent gui

CONFIG += c++17
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/backend.cpp \
        src/branchandbound.cpp \
        src/fruchtermanreingold.cpp \
        src/graphdrawer.cpp \
        src/graphlayout.cpp \
        src/main.cpp \
        src/pdfwriter.cpp \
        src/tablemodel.cpp \
        src/treedrawer.cpp

RESOURCES += qml.qrc

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
    src/branchandbound.h \
    src/drawingalgorithm.h \
    src/fruchtermanreingold.h \
    src/graphdrawer.h \
    src/graphlayout.h \
    src/matrixloader.h \
    src/pdfwriter.h \
    src/tablemodel.h \
    src/treedrawer.h
