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
#include "slimcli.h"

class playerInfo : public QObject
{
    Q_OBJECT
public:
    explicit playerInfo(SlimCLI *c, QByteArray mac, QObject *parent = 0);
    void Init(void);
    void LockMutex(void) { mutex.lock(); }
    void UnlockMutex(void) {mutex.unlock(); }

    CurrentPlayList &GetCurrentPlaylist(void) { QMutexLocker l(&mutex); return m_devicePlayList; }
    TrackData &GetCurrentTrackInfo(void) {  QMutexLocker l(&mutex); return m_currentTrack; }
    CurrentPlayList &GetCurrentPlaylistNoMutex(void) { return m_devicePlayList; }
    TrackData &GetCurrentTrackInfoNoMutex(void) {  return m_currentTrack; }
    playerMode GetDeviceMode(void) { QMutexLocker l(&mutex); return m_deviceMode; } // one of the following: "play", "stop" or "pause"
    RepeatMode GetRepeatMode(void) { QMutexLocker l(&mutex); return m_deviceRepeatMode; }
    ShuffleMode GetShuffleMode(void) { QMutexLocker l(&mutex); return m_deviceShuffleMode; }
    PlayerState GetDeviceState(void) { QMutexLocker l(&mutex); return m_deviceState; }
    int GetDeviceVolume(void) { QMutexLocker l(&mutex); return m_deviceVol; }
    int GetTrackDuration(void) { QMutexLocker l(&mutex); return m_songDuration; }
    int GetTrackPlaytime(void) { QMutexLocker l(&mutex); return m_songPlaying; }
    int GetPlaylistIndex(void) { QMutexLocker l(&mutex); return m_devicePlaylistIndex; }
    int GetPlaylistCount(void) { QMutexLocker l(&mutex); return m_devicePlaylistCount; }

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
    void processCliMessage(void);
//    void controlViewClicked(int idx);
//    void controlViewClicked(QString s);

private:
    void ErrorMessageSender(QString s);
    void GetTracks(int count=-1);
    playerMode TogglePlayerMode(playerMode p);

    void processDeviceStatusMsg(QByteArray msg);
    void processPlaylistInteractionMsg(QByteArray msg);
    void processPlaylistMsg(QListIterator<QByteArray> &i);
    void processPlayerSettingsMsg(QListIterator<QByteArray> &i);
    void SystemMsgProcessing(QByteArray msg);
    QByteArray MacAddressOfResponse(QByteArray msg);
    QByteArray ResponseLessMacAddress(QByteArray msg);

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

    SlimCLI *cli;   // need pointer
    QByteArray macAddress;       // NOTE: this is stored in URL escaped form, since that is how we mostly use it.  If you need it in plain text COPY IT to another string and use QUrl::decode() on that string.
};

#endif // PLAYERINFO_H
