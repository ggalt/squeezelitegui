#include <QApplication>
#include <QSettings>
#include "qmlapplicationviewer.h"

#include "squeezedefines.h"
#include "audioplayer.h"

#ifdef SQUEEZEMAINWINDOW_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/squeezelitegui/squeezeliteguimain.qml"));
    viewer.showExpanded();

    AudioPlayer *p = new AudioPlayer(0);
    p->Init();

//    QObject *rootObject = dynamic_cast<QObject*>(viewer.rootObject());

    QObject::connect(&viewer, SIGNAL(shuffleClicked()), p, SLOT(shuffleClicked()));
    QObject::connect(&viewer, SIGNAL(forwardClicked()), p, SLOT(forwardClicked()));

    int retVal = app->exec();
    p->Close();
    return retVal;
}
