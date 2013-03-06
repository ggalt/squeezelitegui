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

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    music.cpp \
    slimserverinfo.cpp \
    slimdevice.cpp \
    slimdatabasefetch.cpp \
    slimcli.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qml/squeezelitegui/squeezeliteguimain.qml

# Commands to build squeezelite
#mytarget.commands = make -C my_sub_project
#QMAKE_EXTRA_TARGETS += mytarget
#PRE_TARGETDEPS += mytarget

HEADERS += \
    music.h \
    squeezedefines.h \
    slimserverinfo.h \
    slimdevice.h \
    slimdatabasefetch.h \
    slimcli.h
