#ifndef DEVICESTATUS_H
#define DEVICESTATUS_H

#include <QObject>
#include <QByteArray>
#include <QDateTime>
#include <QQuickItem>
#include <QQuickView>
#include <QModelIndex>
#include <QVariant>

#include "squeezedefines.h"
#include "controllistmodel.h"


class DeviceStatus : public QQuickView
{
    Q_OBJECT

public:
    explicit DeviceStatus(QWindow *parent = 0);
    void Init(QString serverAddr, QString httpPort);
    CurrentPlayList &getCurrentPlaylist(void) { return m_devicePlayList; }

    int getPlayState(void) { return m_playState; }
    void setPlayState(int newstate) { m_playState=newstate; }
    void togglePlayState(void) {
        if(m_playState==PLAY)
            m_playState=PAUSE;
        else        // handles both "PAUSE" and "STOP", which we don't really support
            m_playState=PLAY;
    }

    
signals:
    void issueCommand(QByteArray cmd);
    void playlistIndexChange(QVariant newidx);
    void updateAlbumCover(QVariant imageURL);
    void deviceStatusReady(void);
    void playStatus(QVariant mode);
    void shuffleStatus(QVariant mode);
    void repeatStatus(QVariant mode);
    void progress(QVariant percent);
    void VolumeChange(QVariant vol);
    
public slots:
    void processDeviceStatusMsg(QByteArray msg);
    void processPlaylistInteractionMsg(QByteArray msg);
    void controlViewClicked(int idx);
    void controlViewClicked(QString s);

    void NewSong();
    void NewPlaylist(void);
    void Mute(bool);
    void setVolume(int vol);
    void ModeChange(QString);

private:
    void loadHomeScreen(void);
    void loadMusicScreen(void);
    void loadNowPlayingScreen(void);

private:
    /*
* NOTE: almost all of the variables are exchanged with the server as
* string-based commands and so we store virutually everything as
* QByteArrays rather than as the actual item represented (e.g., an 'int')
* so that server communication doesn't require a bunch of conversions back
* and forth -- except in rare instances.
*/
    //    QByteArray m_deviceNumber; // device number in "Device count" (i.e., if there are 3 devices, which number (0,1,2) is this?)
    //    QByteArray m_deviceMAC; // MAC address of the device
    //    QByteArray m_deviceName; // Name as listed in server (e.g., "SoftSqueeze" or "Kitchen Device")
    //    QByteArray m_deviceIP; // IP address of device

    QByteArray m_deviceVol; // volume (0-100)
    bool m_deviceMute; // is device muted
    QByteArray m_deviceMode; // one of the following: "play", "stop" or "pause"
    int m_playState;
    QByteArray m_deviceRepeatMode;
    QByteArray m_deviceShuffleMode;
    bool m_isPlaying;

    QByteArray m_devicePlaylistName; // name of current playlist
    int m_devicePlaylistCount; // number of tracks in current playlist
    int m_devicePlaylistIndex;  // where are we in the current playlist
    int m_deviceOldPlaylistIndex;   // storage so we can "unhighlight" the old playlist item
    CurrentPlayList m_devicePlayList; // all info related to the current device playlist
    QTime m_playerTime;   // how long have we been playing?
    bool m_deviceInitialized;

    TrackData m_currentTrack;
    QByteArray m_deviceCurrentSongTime; // time into current song
    QByteArray m_deviceCurrentSongDuration; // length of current song

    QString SqueezeBoxServerAddress;
    QString SqueezeBoxServerHttpPort;
    QHash<QString,ControlListModel*> controlHierarchy;
    QModelIndex *nowPlayingIndex;
};
#endif // DEVICESTATUS_H
