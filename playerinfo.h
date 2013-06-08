#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QString>
#include <QList>
#include <QListIterator>
#include <QMutex>
#include <QMutexLocker>
#include "squeezedefines.h"

class playerInfo : public QObject
{
    Q_OBJECT
public:
    explicit playerInfo(QObject *parent = 0);
    void Init(void);
    void LockMutex(void) { mutex.lock(); }
    void UnlockMutex(void) {mutex.unlock(); }

    CurrentPlayList &getCurrentPlaylist(void) { QMutexLocker l(&mutex); return m_devicePlayList; }
    playerMode GetDeviceMode(void) { QMutexLocker l(&mutex); return m_deviceMode; } // one of the following: "play", "stop" or "pause"
    RepeatMode GetRepeatMode(void) { QMutexLocker l(&mutex); return m_deviceRepeatMode; }
    ShuffleMode GetShuffleMode(void) { QMutexLocker l(&mutex); return m_deviceShuffleMode; }
    PlayerState GetDeviceState(void) { QMutexLocker l(&mutex); return m_deviceState; }
    int GetDeviceVolume(void) { QMutexLocker l(&mutex); return m_deviceVol; }
    int GetTrackDuration(void) { QMutexLocker l(&mutex); return m_songDuration; }
    int GetTrackPlaytime(void) { QMutexLocker l(&mutex); return m_songPlaying; }

    void SetDeviceMode(playerMode m) { QMutexLocker l(&mutex); m_deviceMode = m; } // one of the following: "play", "stop" or "pause"
    void SetRepeatMode(RepeatMode m) { QMutexLocker l(&mutex); m_deviceRepeatMode = m; }
    void SetShuffleMode(ShuffleMode m) { QMutexLocker l(&mutex); m_deviceShuffleMode = m; }
    void SetDeviceState(PlayerState m) { QMutexLocker l(&mutex); m_deviceState = m; }
    void SetDeviceVolume(int vol) { QMutexLocker l(&mutex); m_deviceVol=vol; }

signals:
    void PlayerInfoFilled(void);
    void playerInfoError(QString s);
    void PlayerStatus(PlayerState s);
    void issueStandardCommand(CliCommand cmd);
    void issueCommand(QByteArray cmd);

    void playStatus(QVariant mode);
    void shuffleStatus(QVariant mode);
    void repeatStatus(QVariant mode);
    void progress(QVariant percent);
    void VolumeChange(QVariant vol);
    void NewSong(void);
    void NewPlayList(void);
    void Mute(bool m);
    void PlayingTime(QVariant songDuration, QVariant songPlaying);

public slots:
    void processDeviceStatusMsg(QByteArray msg);
    void processPlaylistInteractionMsg(QByteArray msg);
//    void controlViewClicked(int idx);
//    void controlViewClicked(QString s);

private:
    void ErrorMessageSender(QString s);
    void GetTracks(int count=-1);
    playerMode TogglePlayerMode(playerMode p);
    void processPlaylistMsg(QListIterator<QByteArray> &i);
    void processPlayerSettingsMsg(QListIterator<QByteArray> &i);

private:
    QMutex mutex;

    int m_deviceVol; // volume (0-100)
    bool m_deviceMute; // is device muted
    playerMode m_deviceMode; // one of the following: "play", "stop" or "pause"
    RepeatMode m_deviceRepeatMode;
    ShuffleMode m_deviceShuffleMode;
    PlayerState m_deviceState;

    QByteArray m_devicePlaylistName; // name of current playlist
    int m_devicePlaylistCount; // number of tracks in current playlist
    int m_devicePlaylistIndex;  // where are we in the current playlist
    int m_deviceOldPlaylistIndex;   // storage so we can "unhighlight" the old playlist item
    int m_songDuration; // duration of the current song in seconds
    int m_songPlaying;  // amount of time song has been playing
    QByteArray m_deviceCurrentSongTime; // time into current song
    QByteArray m_deviceCurrentSongDuration; // length of current song

    CurrentPlayList m_devicePlayList; // all info related to the current device playlist
    TrackData m_currentTrack;
    quint16 m_listStart;
    quint16 m_listEnd;
    quint16 m_MaxRequestSize;

    QTime m_playerTime;   // how long have we been playing?
    QTimer m_tick;
};

#endif // PLAYERINFO_H
