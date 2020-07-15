#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T21:44:54
#
#-------------------------------------------------

QT       += core gui charts sql datavisualization uitools

TARGET = MSCAA_GuiMultiSpecies
TEMPLATE = lib

PRECOMPILED_HEADER = /home/rklasky/workspaceQtCreator/MSCAA/MSCAA_GuiUtilities/precompiled_header.h
CONFIG += precompile_header

DEFINES += MSCAA_GUIMULTISPECIES_LIBRARY
CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    nmfMSCAATab01.cpp \
    nmfMSCAATab02.cpp \
    nmfMSCAATab03.cpp \
    nmfMSCAATab04.cpp \
    nmfMSCAATab05.cpp

HEADERS += \
    nmfMSCAATab01.h \
    nmfMSCAATab02.h \
    nmfMSCAATab03.h \
    nmfMSCAATab04.h \
    nmfMSCAATab05.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfDatabase-Qt_5_12_3_gcc64-Release/release/ -lnmfDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfDatabase-Qt_5_12_3_gcc64-Release/debug/ -lnmfDatabase
else:unix: LIBS += -L$$PWD/../../build-nmfDatabase-Qt_5_12_3_gcc64-Release/ -lnmfDatabase

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfDatabase
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfDatabase

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfUtilities-Qt_5_12_3_gcc64-Release/release/ -lnmfUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfUtilities-Qt_5_12_3_gcc64-Release/debug/ -lnmfUtilities
else:unix: LIBS += -L$$PWD/../../build-nmfUtilities-Qt_5_12_3_gcc64-Release/ -lnmfUtilities

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiUtilities-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_GuiUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiUtilities-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_GuiUtilities
else:unix: LIBS += -L$$PWD/../../build-MSCAA_GuiUtilities-Qt_5_12_3_gcc64-Release/ -lMSCAA_GuiUtilities

INCLUDEPATH += $$PWD/../MSCAA_GuiUtilities
DEPENDPATH += $$PWD/../MSCAA_GuiUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/release/ -lnmfCharts
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/debug/ -lnmfCharts
else:unix: LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/ -lnmfCharts

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfCharts
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfCharts

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_Optimization-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_Optimization
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_Optimization-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_Optimization
else:unix: LIBS += -L$$PWD/../../build-MSCAA_Optimization-Qt_5_12_3_gcc64-Release/ -lMSCAA_Optimization

INCLUDEPATH += $$PWD/../MSCAA_Optimization
DEPENDPATH += $$PWD/../MSCAA_Optimization
