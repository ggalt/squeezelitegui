# Add more folders to ship with the application, here
folder_01.source = qml/squeezelitegui
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    slimcli.cpp \
    controllistmodel.cpp \
    Threads.cpp \
    squeezelitegui.cpp \
    playerinfo.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    squeezedefines.h \
    slimcli.h \
    controllistmodel.h \
    Threads.h \
    squeezelitegui.h \
    playerinfo.h

# Commands to build squeezelite
#mytarget.commands = make -C my_sub_project
#QMAKE_EXTRA_TARGETS += mytarget
#PRE_TARGETDEPS += mytarget

RESOURCES += \
    squeezelitegui.qrc

