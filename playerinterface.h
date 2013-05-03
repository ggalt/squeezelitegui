#ifndef PLAYERINTERFACE_H
#define PLAYERINTERFACE_H

#include <QObject>
#include <QQuickView>

#include "squeezedefines.h"

class playerInterface : public QQuickView
{
    Q_OBJECT
public:
    explicit playerInterface(QWindow *parent = 0);
    void Init(void);
    void loadHomeScreen(void);
    void loadMusicScreen(void);
    void loadNowPlayingScreen(void);

signals:
    
public slots:

private:
    playerMode m_playState;
    RepeatMode m_repeatState;
    ShuffleMode m_shuffleState;

    int m_volume;
    
};

#endif // PLAYERINTERFACE_H
