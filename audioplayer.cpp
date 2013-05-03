#include <QList>
#include <QListIterator>
#include <QHashIterator>
#include <QNetworkInterface>

#include "audioplayer.h"

#ifdef AUDIOPLAYER_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif



AudioPlayer::AudioPlayer(QObject *parent) :
    QObject(parent)
{
}

AudioPlayer::~AudioPlayer()
{
    Close();
}

void AudioPlayer::Close(void)
{
    DEBUGF("");
    if(player) {
        DEBUGF("CLOSING PLAYER");
        player->close();
        player->waitForFinished(5000);
        player->deleteLater();
    }
}

void AudioPlayer::Init(void)
{
    QSettings *mySettings = new QSettings("squeezelitegui", "squeezelitegui");

    if( !mySettings->contains("Version") || mySettings->value("Version")!=DATAVERSION) {     // no settings file, so create one
        mySettings->setValue("Version", DATAVERSION);
        mySettings->setValue("Server/Address","127.0.0.1");
        //        mySettings->setValue("Server/Address","10.6.67.54");
        mySettings->setValue("Server/AudioPort","3483");
        mySettings->setValue("Server/CLIPort", "9090");
        mySettings->setValue("Server/HttpPort", "9000");
        mySettings->setValue("Server/Username", "");
        mySettings->setValue("Server/Password", "");
        mySettings->setValue("Audio/Device","default");
        mySettings->setValue("Player/Name","squeezelite");
        mySettings->sync();
    }

    lmsUsername = mySettings->value("Server/Username","").toString();
    lmsPassword = mySettings->value("Server/Password","").toString();
    SqueezeBoxServerAddress = mySettings->value("Server/Address","127.0.0.1").toString();
    SqueezeBoxServerAudioPort = mySettings->value("Server/AudioPort","3483").toString();
    SqueezeBoxServerCLIPort = mySettings->value("Server/CLIPort", "9090").toString();
    SqueezeBoxServerHttpPort = mySettings->value("Server/HttpPort", "9000").toString();
    AudioDevice = mySettings->value("Audio/Device","").toString();
    PlayerName = mySettings->value("Player/Name", "squeezelite").toString();

    getplayerMACAddress();  // get the MAC address we are going to use

    devViewer = new DeviceStatus();

    player = new QProcess(this);

#ifdef Q_OS_LINUX
    QString program = "squeezelite";
    program += (QString(" -m " + QString( MacAddress ))) + (QString(" -n " + PlayerName)) + (QString(" -o " + AudioDevice)) + " " + SqueezeBoxServerAddress;
#else
    QString program = QString('"')+QString("c:\\program files (x86)\\squeezelite\\squeezelite-win")+QString('"');
    program += (QString(" -m " + QString( MacAddress ))) + (QString(" -n " + PlayerName)) + (QString(" -o " + AudioDevice)) + " " + SqueezeBoxServerAddress;
#endif

    DEBUGF( "player command " << program);

    player->start( program );
    DEBUGF("ERROR STRING IF ANY" << player->error() << player->errorString() );
    //    player->waitForStarted(2000);


    //    // initialize the CLI interface.  Make sure that you've set the appropriate server address and port
    cli = new SlimCLI(this, "cli", SqueezeBoxServerAddress, encodedMacAddress, SqueezeBoxServerCLIPort.toInt());
    connect(cli,SIGNAL(isConnected()),this,SLOT(cliConnected()));

    cli->Init();
}

void AudioPlayer::cliConnected(void)
{
    DEBUGF("cliConnected Slot");
    connect(devViewer,SIGNAL(issueCommand(QByteArray)),
            cli,SLOT(SendCommand(QByteArray))); // so device can send messages
    connect(cli,SIGNAL(DeviceStatusMessage(QByteArray)),
            devViewer,SLOT(processDeviceStatusMsg(QByteArray)));  // so cli can send message to device
    connect(cli,SIGNAL(PlaylistInteractionMessage(QByteArray)),
            devViewer,SLOT(processPlaylistInteractionMsg(QByteArray)));
    connect(devViewer,SIGNAL(deviceStatusReady()),this,SLOT(initInterfaceConnections()));

    playState(PAUSE);
    devViewer->Init(SqueezeBoxServerAddress, SqueezeBoxServerHttpPort);
}

void AudioPlayer::cliMsgAvailable(void)
{
}

void AudioPlayer::sendCLIcommand(QByteArray cmd)
{
    cli->SendCommand(cmd);
}

void AudioPlayer::ProcessCLIMessage(QByteArray &msg)
{
    while(msg.contains('\n'))
        msg.replace(msg.indexOf('\n'), 1, " ");

    QRegExp MACrx("[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]");

    DEBUGF("raw message:" << msg.left(20));

    if(MACrx.indexIn(QString(msg)) >= 0) { // if it starts with a MAC address, send it to a device for processing
        DeviceMsgProcessing(msg);
    }
    else {
        SystemMsgProcessing(msg);
    }
}

void AudioPlayer::DeviceMsgProcessing(QByteArray &msg)
{
    DEBUGF("Device Message: " << msg.left(200));

    if(encodedMacAddress.toLower() == MacAddressOfResponse(msg).toLower()) {
        QByteArray resp = ResponseLessMacAddress(msg);
        DEBUGF("MSG with MAC removed:" << resp.left(200));
        if(resp.startsWith("status")) {
            emit DeviceStatusMessage(resp);
        }
        else {
            emit PlaylistInteractionMessage(resp);
        }
    }
    else {  // wait!  Whose MAC address is this?
//        DEBUGF(QString("Unknown MAC address: %1 our address is %2").arg(QString(MacAddressOfResponse())).arg(QString(macAddress)));
    }
}

void AudioPlayer::SystemMsgProcessing(QByteArray &msg)
{
    DEBUGF("SYSTEM MESSAGE: " << msg);
}

void AudioPlayer::ProcessLoginMsg(QByteArray &msg)
{
    DEBUGF("");
//    if(msg.left(5) == "login")
//        isAuthenticated = true;
}

void AudioPlayer::ProcessControlMsg(QByteArray &msg)
{
    DEBUGF("CONTROLLING MODE message received: " << msg);
    QList<QByteArray> responseList;
    responseList = msg.split(' ');   // break this up into fields delimited by spaces
    if(msg.left(7) == "artists") {  // it's processing artist information
        if(responseList.at(3).left(9) == "artist_id") {
            for(int c = 3; c < responseList.size(); c++) {
            }
        }
        return;
    }

    if(msg.left(6) == "albums" )  // its processing album information
        return;
}

QByteArray AudioPlayer::MacAddressOfResponse(QByteArray &msg)
{
    DEBUGF("");
    if(msg.contains("%3A"))
        return msg.left(27).trimmed().toLower();
    else
        return QByteArray();
}

QByteArray AudioPlayer::ResponseLessMacAddress(QByteArray &msg)
{
    DEBUGF("");
    if(msg.contains("%3A"))
        return msg.right(msg.length() - 27).trimmed();
    else
        return msg.trimmed();
}

QByteArray AudioPlayer::RemoveNewLineFromResponse(QByteArray &msg)
{
    DEBUGF("");
    while(msg.contains('\n'))
        msg.replace(msg.indexOf('\n'), 1, " ");
    return msg;
}


void AudioPlayer::initInterfaceConnections(void)
{
    DEBUGF("Initialize Interface Connections");

    QQuickItem *v = devViewer->rootObject();
    // interface signals to application
    connect(v,SIGNAL(play(int)), this,SLOT(playState(int)));
    connect(v,SIGNAL(nextTrack()), this,SLOT(nextTrackClicked()));
    connect(v,SIGNAL(prevTrack()), this,SLOT(prevTrackClicked()));
    connect(v,SIGNAL(volUp()), this,SLOT(volUp()));
    connect(v,SIGNAL(volDown()), this,SLOT(volDown()));
    connect(v,SIGNAL(setVolume(int)), this,SLOT(setVolume(int)));
    connect(v,SIGNAL(controlClicked(QString)), devViewer,SLOT(controlViewClicked(QString)));
    connect(v,SIGNAL(shuffle(int)), this,SLOT(shuffleState(int)));
    connect(v,SIGNAL(repeat(int)), this,SLOT(repeatState(int)));
//    connect(v,SIGNAL(playButtonClicked()),this,SLOT(playPauseToggle()));

    // application signals to interface
    connect(devViewer,SIGNAL(playlistIndexChange(QVariant)), v, SLOT(setControlViewListIndex(QVariant)));
    connect(devViewer,SIGNAL(updateAlbumCover(QVariant)), v,SLOT(updateAlbumCover(QVariant)));
    connect(devViewer,SIGNAL(playStatus(QVariant)), v, SLOT(updatePlayMode(QVariant)));
    connect(devViewer,SIGNAL(VolumeChange(QVariant)), v,SLOT(setMainVolume(QVariant)));

/*
 *  messages from device that need to be connected to slots
    void playlistIndexChange(QVariant newidx);
    void NewSong(int newPlayListIndex);
    void NewPlaylist(void);
    void Mute(bool);
    void VolumeChange(int);
    void ModeChange(QString);
*/
}

void AudioPlayer::shuffleState(int state)
{
    DEBUGF("");
    if(state==SHUFFLE_OFF) {
        cli->SendCommand(QByteArray("playlist shuffle 0\n"));
    } else if(state==SHUFFLE_BY_SONG) {
        cli->SendCommand(QByteArray("playlist shuffle 1\n"));
    } else if(state==SHUFFLE_BY_ALBUM) {
        cli->SendCommand(QByteArray("playlist shuffle 2\n"));
    }
}

void AudioPlayer::repeatState(int state)
{
    DEBUGF("");
    if(state==REPEAT_OFF) {
        cli->SendCommand(QByteArray("playlist repeat 0\n"));
    } else if(state==REPEAT_TRACK) {
        cli->SendCommand(QByteArray("playlist repeat 1\n"));
    } else if(state==REPEAT_PLAYLIST) {
        cli->SendCommand(QByteArray("playlist repeat 2\n"));
    }
}

void AudioPlayer::nextTrackClicked(void)
{
    cli->SendCommand(QByteArray("button fwd.single\n"));
    DEBUGF("");
}

void AudioPlayer::prevTrackClicked(void)
{
    cli->SendCommand(QByteArray("button rew\n"));
    DEBUGF("");
}

void AudioPlayer::playState(int state)
{
    if (state==PLAY) {   // play state
        cli->SendCommand(QByteArray("play"));
    }
    else if(state==PAUSE) {   // pause state
        cli->SendCommand(QByteArray("pause"));
    }

    DEBUGF("PlayerState" << state)
}

void AudioPlayer::playPauseToggle(void)
{
    devViewer->togglePlayState();   // toggle the play state
    if(devViewer->getPlayState()==PLAY){
        cli->SendCommand(QByteArray("play"));
    }
    else {
        cli->SendCommand(QByteArray("pause"));
    }
}

void AudioPlayer::volUp(void)
{
    cli->SendCommand(QByteArray("button volup\n"));
}

void AudioPlayer::volDown(void)
{
    cli->SendCommand(QByteArray("button voldown\n"));
}

void AudioPlayer::setVolume(int vol)
{
    devViewer->setVolume(vol);
    QByteArray volume = QByteArray::number(vol);
    cli->SendCommand(QByteArray("mixer volume ") + volume + QByteArray("\n"));
}

void AudioPlayer::getplayerMACAddress( void )
{
    DEBUGF("");
    MacAddress = QByteArray( "00:00:00:00:00:04" );

    QList<QNetworkInterface> netList = QNetworkInterface::allInterfaces();
    QListIterator<QNetworkInterface> i( netList );

    QNetworkInterface t;

    while( i.hasNext() ) {  // note: this grabs the first functional, non-loopback address there is.  It may not the be interface on which you really connect to the slimserver
        t = i.next();
        if( !t.flags().testFlag( QNetworkInterface::IsLoopBack ) &&
                t.flags().testFlag( QNetworkInterface::IsUp ) &&
                t.flags().testFlag( QNetworkInterface::IsRunning ) ) {
            MacAddress = t.hardwareAddress().toLatin1().toLower();
            if(!MacAddress.contains("%3A")) // not escape encoded
                encodedMacAddress = QString(MacAddress).toLatin1().toPercentEncoding();
            return;
        }
    }
}

