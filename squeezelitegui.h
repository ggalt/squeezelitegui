#ifndef SQUEEZELITEGUI_H
#define SQUEEZELITEGUI_H

#include <QObject>
#include <QByteArray>
#include <QDateTime>
#include <QQuickItem>
#include <QQuickView>
#include <QModelIndex>
#include <QVariant>
#include <QProcess>
#include <QHash>
#include <QHashIterator>
#include <QTimer>

#include "squeezedefines.h"
#include "Threads.h"
#include "slimcli.h"
#include "playerinfo.h"
#include "controllistmodel.h"

class squeezeLiteGui : public QQuickView
{
    Q_OBJECT
public:
    explicit squeezeLiteGui(QWindow *parent = 0);
    ~squeezeLiteGui(void);
    void Init(void);
    void Close(void);
    
signals:
    void issueStandardCommand(CliCommand c);
    void issueCommand(QByteArray cmd);
    void playlistIndexChange(QVariant newidx);
    void updateAlbumCover(QVariant imageURL);
    void deviceStatusReady(void);
    void playStatus(QVariant mode);
    void shuffleStatus(QVariant mode);
    void repeatStatus(QVariant mode);
    void songDuration(QVariant len);
    void progress(QVariant time);
    void VolumeChange(QVariant vol);

public slots:
    void ErrorMessageReceiver(QString s);
    void CliReady(void);
    void playerInfoReady(void);
    void PlayerStatus(PlayerState s);

    void controlViewClicked(int idx);
    void controlViewClicked(QString itemClicked);

    void NewSong();
    void NewPlaylist(void);
    void ModeChange(QString);
    void shuffleState(int state);
    void repeatState(int state);
    void nextTrackClicked(void);
    void prevTrackClicked(void);
    void playState(int state);
    void playPauseToggle(void);
    void volUp(void);
    void volDown(void);
    void Mute(bool);
    void setVolume(int vol);

private:
    void getplayerMACAddress(void);

    void refreshScreenSettings(void);
    void setupInterfaceConnections(void);

    void loadHomeScreen(void);
    void loadMusicScreen(void);
    void updateNowPlayingScreen(void);
    void loadNowPlayingScreen(void);

private:
    QString lmsUsername;
    QString lmsPassword;
    QString SqueezeBoxServerAddress;
    QString SqueezeBoxServerCLIPort;
    QString SqueezeBoxServerHttpPort;
    QString SqueezeBoxServerAudioPort;
    QString AudioDevice;
    QString PlayerName;

    QByteArray MacAddress;      // MAC address of this machine (which will become the MAC address for our player)
    QByteArray encodedMacAddress;   // percent encoded mac address for use with the CLI

    InterfaceState m_interfaceState;

    QHash<QString,ControlListModel*> controlHierarchy;
    QModelIndex *nowPlayingIndex;

    cliThread *m_cliThread;
    playerInfoThread *m_playerInfoThread;

    QProcess *player;
    SlimCLI *cli;
    playerInfo *m_playerInfo;
    QTimer m_tick;
};

#endif // SQUEEZELITEGUI_H
