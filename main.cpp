#include <QtGui/QGuiApplication>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlContext>
#include <QImage>
#include "qtquick2applicationviewer.h"

#include "squeezedefines.h"
#include "squeezelitegui.h"

#ifdef SQUEEZEMAINWINDOW_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qRegisterMetaType<playerMode> ("playerMode");
    qRegisterMetaType<RepeatMode> ("RepeatMode");
    qRegisterMetaType<ShuffleMode> ("ShuffleMode");
    qRegisterMetaType<ResumeMode> ("ResumeMode");
    qRegisterMetaType<PlayerState> ("PlayerState");
    qRegisterMetaType<InterfaceState> ("InterfaceState");
    qRegisterMetaType<CliCommand> ("CliCommand");


    squeezeLiteGui *p = new squeezeLiteGui(0);
    p->Init();
    int retVal = app.exec();
    p->Close();
    return retVal;
}
