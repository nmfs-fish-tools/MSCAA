#-------------------------------------------------
#
# Project created by QtCreator 2019-04-29T16:42:32
#
#-------------------------------------------------

QT       += core gui charts sql datavisualization uitools concurrent testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MSCAA
TEMPLATE = app

PRECOMPILED_HEADER = /home/rklasky/workspaceQtCreator/MSCAA/MSCAA_Main/precompiled_header.h
CONFIG += precompile_header
#CONFIG += static

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -std=c++0x

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14
LIBS += -lboost_system -lboost_filesystem

SOURCES += \
        main.cpp \
        nmfMainWindow.cpp

HEADERS += \
        nmfMainWindow.h

FORMS += \
        nmfMainWindow.ui

RESOURCES += \
    resource.qrc \
    qdarkstyle/style.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/release/ -lnmfCharts
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/debug/ -lnmfCharts
else:unix: LIBS += -L$$PWD/../../build-nmfCharts-Qt_5_12_3_gcc64-Release/ -lnmfCharts

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfCharts
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfCharts

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfGuiDialogs-Qt_5_12_3_gcc64-Release/release/ -lnmfGuiDialogs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfGuiDialogs-Qt_5_12_3_gcc64-Release/debug/ -lnmfGuiDialogs
else:unix: LIBS += -L$$PWD/../../build-nmfGuiDialogs-Qt_5_12_3_gcc64-Release/ -lnmfGuiDialogs

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfGuiDialogs
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfGuiDialogs

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfDataModels-Qt_5_12_3_gcc64-Release/release/ -lnmfDataModels
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfDataModels-Qt_5_12_3_gcc64-Release/debug/ -lnmfDataModels
else:unix: LIBS += -L$$PWD/../../build-nmfDataModels-Qt_5_12_3_gcc64-Release/ -lnmfDataModels

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfDataModels
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfDataModels

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfDynamics-Qt_5_12_3_gcc64-Release/release/ -lnmfDynamics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfDynamics-Qt_5_12_3_gcc64-Release/debug/ -lnmfDynamics
else:unix: LIBS += -L$$PWD/../../build-nmfDynamics-Qt_5_12_3_gcc64-Release/ -lnmfDynamics

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfDynamics
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfDynamics

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfRecruitment-Qt_5_12_3_gcc64-Release/release/ -lnmfRecruitment
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfRecruitment-Qt_5_12_3_gcc64-Release/debug/ -lnmfRecruitment
#else:unix: LIBS += -L$$PWD/../../build-nmfRecruitment-Qt_5_12_3_gcc64-Release/ -lnmfRecruitment

#INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfRecruitment
#DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfRecruitment

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-nmfGuiComponentsMain-Qt_5_12_3_gcc64-Release/release/ -lnmfGuiComponentsMain
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-nmfGuiComponentsMain-Qt_5_12_3_gcc64-Release/debug/ -lnmfGuiComponentsMain
else:unix: LIBS += -L$$PWD/../../build-nmfGuiComponentsMain-Qt_5_12_3_gcc64-Release/ -lnmfGuiComponentsMain

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfGuiComponentsMain
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfGuiComponentsMain

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiSingleSpecies-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_GuiSingleSpecies
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiSingleSpecies-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_GuiSingleSpecies
else:unix: LIBS += -L$$PWD/../../build-MSCAA_GuiSingleSpecies-Qt_5_12_3_gcc64-Release/ -lMSCAA_GuiSingleSpecies

INCLUDEPATH += $$PWD/../MSCAA_GuiSingleSpecies
DEPENDPATH += $$PWD/../MSCAA_GuiSingleSpecies

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiMultiSpecies-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_GuiMultiSpecies
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiMultiSpecies-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_GuiMultiSpecies
else:unix: LIBS += -L$$PWD/../../build-MSCAA_GuiMultiSpecies-Qt_5_12_3_gcc64-Release/ -lMSCAA_GuiMultiSpecies

INCLUDEPATH += $$PWD/../MSCAA_GuiMultiSpecies
DEPENDPATH += $$PWD/../MSCAA_GuiMultiSpecies

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiSetup-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_GuiSetup
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiSetup-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_GuiSetup
else:unix: LIBS += -L$$PWD/../../build-MSCAA_GuiSetup-Qt_5_12_3_gcc64-Release/ -lMSCAA_GuiSetup

INCLUDEPATH += $$PWD/../MSCAA_GuiSetup
DEPENDPATH += $$PWD/../MSCAA_GuiSetup

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiUtilities-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_GuiUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiUtilities-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_GuiUtilities
else:unix: LIBS += -L$$PWD/../../build-MSCAA_GuiUtilities-Qt_5_12_3_gcc64-Release/ -lMSCAA_GuiUtilities

INCLUDEPATH += $$PWD/../MSCAA_GuiUtilities
DEPENDPATH += $$PWD/../MSCAA_GuiUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiSimulation-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_GuiSimulation
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiSimulation-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_GuiSimulation
else:unix: LIBS += -L$$PWD/../../build-MSCAA_GuiSimulation-Qt_5_12_3_gcc64-Release/ -lMSCAA_GuiSimulation

INCLUDEPATH += $$PWD/../MSCAA_GuiSimulation
DEPENDPATH += $$PWD/../MSCAA_GuiSimulation

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiOutput-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_GuiOutput
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_GuiOutput-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_GuiOutput
else:unix: LIBS += -L$$PWD/../../build-MSCAA_GuiOutput-Qt_5_12_3_gcc64-Release/ -lMSCAA_GuiOutput

INCLUDEPATH += $$PWD/../MSCAA_GuiOutput
DEPENDPATH += $$PWD/../MSCAA_GuiOutput

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-MSCAA_Optimization-Qt_5_12_3_gcc64-Release/release/ -lMSCAA_Optimization
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-MSCAA_Optimization-Qt_5_12_3_gcc64-Release/debug/ -lMSCAA_Optimization
else:unix: LIBS += -L$$PWD/../../build-MSCAA_Optimization-Qt_5_12_3_gcc64-Release/ -lMSCAA_Optimization

INCLUDEPATH += $$PWD/../MSCAA_Optimization
DEPENDPATH += $$PWD/../MSCAA_Optimization





# ------------------------ static libraries below --------------------------- #

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-nmfDatabase-Qt_5_13_1_static-Release/release/ -lnmfDatabase
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-nmfDatabase-Qt_5_13_1_static-Release/debug/ -lnmfDatabase
#else:unix: LIBS += -L$$PWD/../build-nmfDatabase-Qt_5_13_1_static-Release/ -lnmfDatabase

#INCLUDEPATH += $$PWD/../nmfDatabase
#DEPENDPATH += $$PWD/../nmfDatabase

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-nmfDatabase-Qt_5_13_1_static-Release/release/libnmfDatabase.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-nmfDatabase-Qt_5_13_1_static-Release/debug/libnmfDatabase.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-nmfDatabase-Qt_5_13_1_static-Release/release/nmfDatabase.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-nmfDatabase-Qt_5_13_1_static-Release/debug/nmfDatabase.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../build-nmfDatabase-Qt_5_13_1_static-Release/libnmfDatabase.a

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-nmfUtilities-Qt_5_13_1_static-Release/release/ -lnmfUtilities
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-nmfUtilities-Qt_5_13_1_static-Release/debug/ -lnmfUtilities
#else:unix: LIBS += -L$$PWD/../build-nmfUtilities-Qt_5_13_1_static-Release/ -lnmfUtilities

#INCLUDEPATH += $$PWD/../nmfUtilities
#DEPENDPATH += $$PWD/../nmfUtilities

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-nmfUtilities-Qt_5_13_1_static-Release/release/libnmfUtilities.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-nmfUtilities-Qt_5_13_1_static-Release/debug/libnmfUtilities.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-nmfUtilities-Qt_5_13_1_static-Release/release/nmfUtilities.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-nmfUtilities-Qt_5_13_1_static-Release/debug/nmfUtilities.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../build-nmfUtilities-Qt_5_13_1_static-Release/libnmfUtilities.a
