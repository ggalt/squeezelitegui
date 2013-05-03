#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QString>
#include <QByteArray>
#include <QQuickView>
#include <QModelIndex>

#include "squeezedefines.h"
#include "slimcli.h"
#include "devicestatus.h"

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayer(QObject *parent = 0);
    ~AudioPlayer();

    void Init(void);
    void Close(void);
    
signals:
    void startCLI(void);
    void DeviceStatusMessage(QByteArray msg);
    void PlaylistInteractionMessage(QByteArray msg);

public slots:
    /*
     *  signal shuffle(int state)
        signal repeat(int state)
        signal nextTrack()
        signal play(int state)
        signal prevTrack()
        signal volUp()
        signal volDown()

     */
    void cliConnected(void);
    void cliMsgAvailable(void);
    void sendCLIcommand(QByteArray cmd);

    void initInterfaceConnections(void);
    void shuffleState(int state);
    void repeatState(int state);
    void nextTrackClicked(void);
    void prevTrackClicked(void);
    void playState(int state);
    void playPauseToggle(void);
    void volUp(void);
    void volDown(void);
    void setVolume(int vol);

private:
    void getplayerMACAddress(void);

    void ProcessCLIMessage(QByteArray &msg);
    void DeviceMsgProcessing(QByteArray &msg); // messages forwarded to devices
    void SystemMsgProcessing(QByteArray &msg); // messages forwarded to the system for processing

    void ProcessLoginMsg(QByteArray &msg);
    void ProcessControlMsg(QByteArray &msg);

    QByteArray MacAddressOfResponse( QByteArray &msg );
    QByteArray ResponseLessMacAddress( QByteArray &msg );
    QByteArray RemoveNewLineFromResponse( QByteArray &msg );

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

    QProcess *player;
    SlimCLI *cli;
    DeviceStatus *devViewer;
};

#endif // AUDIOPLAYER_H
