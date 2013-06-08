#include <QtGui/QGuiApplication>
#include <QQmlContext>
#include <QSettings>
#include <QNetworkInterface>
#include <QList>
#include <QListIterator>
#include "squeezelitegui.h"

#ifdef SLIMDEVICE_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif

squeezeLiteGui::squeezeLiteGui(QWindow *parent) :
    QQuickView(parent)
{
    setObjectName("squeezeLiteGui");
    m_interfaceState = INTERFACE_UNINITIALIZED;
}

squeezeLiteGui::~squeezeLiteGui(void)
{
    m_cliThread->exit();;
    m_cliThread->deleteLater();

    m_playerInfoThread->exit();
    m_playerInfo->deleteLater();
    m_interfaceState = INTERFACE_CLOSED;
    Close();
}

void squeezeLiteGui::Init(void)
{
    qmlRegisterType<ControlListModel>("net.galtfamily.controlListModel",1,0,"ControlListModel");
    //    qmlRegisterType<ControlListItem>("net.galtfamily.controllistitem",1,0,"ControlListItem");
    setSource(QUrl::fromLocalFile("qml/squeezelitegui/squeezeliteguimain.qml"));
    loadHomeScreen();
    show();

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
    cli = new SlimCLI(0, SqueezeBoxServerAddress, encodedMacAddress, SqueezeBoxServerCLIPort.toInt());
    m_cliThread = new cliThread(cli);
    cli->moveToThread(m_cliThread);
    connect(cli,SIGNAL(isConnected()), this, SLOT(CliReady()));
    connect(cli,SIGNAL(cliError(QString)),this,SLOT(ErrorMessageReceiver(QString)));

    m_cliThread->start();
    m_interfaceState = INTERFACE_CLI_STARTED;

//    cli->Init();
}

void squeezeLiteGui::Close(void)
{
    DEBUGF("");
    if(player) {
        DEBUGF("CLOSING PLAYER");
        player->close();
        player->waitForFinished(5000);
        player->deleteLater();
    }
}

void squeezeLiteGui::ErrorMessageReceiver(QString s)
{
    qDebug() << "SQUEEZELITE ERROR MESSAGE:" << s;
}

void squeezeLiteGui::CliReady(void)
{
    DEBUGF("cliConnected Slot");

    m_playerInfo = new playerInfo();
    m_playerInfoThread = new playerInfoThread(m_playerInfo);
    m_playerInfo->moveToThread(m_playerInfoThread);

    // Error message processing
    connect(m_playerInfo,SIGNAL(playerInfoError(QString)),
            this,SLOT(ErrorMessageReceiver(QString)));
    connect(cli,SIGNAL(cliError(QString)),
            this,SLOT(ErrorMessageReceiver(QString)));

    // interface command issuing
    connect(this,SIGNAL(issueStandardCommand(CliCommand)),
            cli,SLOT(SendStandardCommand(CliCommand)));
    connect(this,SIGNAL(issueCommand(QByteArray)),
            cli,SLOT(SendCommand(QByteArray))); // so device can send messages

    // playerinfo command issuing
    connect(m_playerInfo,SIGNAL(issueStandardCommand(CliCommand)),
            cli,SLOT(SendStandardCommand(CliCommand)));
    connect(m_playerInfo,SIGNAL(issueCommand(QByteArray)),
            cli,SLOT(SendCommand(QByteArray)));

    // sending cli message to playerinfo
    connect(cli,SIGNAL(DeviceStatusMessage(QByteArray)),
            m_playerInfo,SLOT(processDeviceStatusMsg(QByteArray)));
    connect(cli,SIGNAL(PlaylistInteractionMessage(QByteArray)),
            m_playerInfo,SLOT(processPlaylistInteractionMsg(QByteArray)));

    connect(m_playerInfo,SIGNAL(PlayerInfoFilled()),
            this,SLOT(playerInfoReady()));
    connect(m_playerInfo,SIGNAL(PlayerStatus(PlayerState)),
            this,SLOT(PlayerStatus(PlayerState)));

    m_playerInfoThread->start();
    //    connect(this,SIGNAL(deviceStatusReady()),this,SLOT(initInterfaceConnections()));
}

void squeezeLiteGui::PlayerStatus(PlayerState s)
{
    switch(s){
    case UNINITIALIZED:
        break;
    case INITIALIZED:
        emit issueStandardCommand(C_GETSTATUS);
        break;
    case DATAREADY:
        break;
    case RUNNING:
        break;
    case ENDING:
        break;
    case MAX_PLAYERSTATE_MODES:
        break;
    }
}

void squeezeLiteGui::playerInfoReady(void)
{
    /* the playerInfo has been filled -- but only with information about the player, not playlist
     * so establish the interface and allow user interaction.  We are now filling the playlist,
     * if any, and will load that screen when ready
     */
    setupInterfaceConnections();
    refreshScreenSettings();
    loadHomeScreen();
}

void squeezeLiteGui::setupInterfaceConnections(void)
{
    DEBUGF("Initialize Interface Connections");

    QQuickItem *v = rootObject();
    // interface signals to application
    connect(v,SIGNAL(play(int)), this,SLOT(playState(int)));
    connect(v,SIGNAL(nextTrack()), this,SLOT(nextTrackClicked()));
    connect(v,SIGNAL(prevTrack()), this,SLOT(prevTrackClicked()));
    connect(v,SIGNAL(volUp()), this,SLOT(volUp()));
    connect(v,SIGNAL(volDown()), this,SLOT(volDown()));
    connect(v,SIGNAL(setVolume(int)), this,SLOT(setVolume(int)));
    connect(v,SIGNAL(controlClicked(QString)), this,SLOT(controlViewClicked(QString)));
    connect(v,SIGNAL(shuffle(int)), this,SLOT(shuffleState(int)));
    connect(v,SIGNAL(repeat(int)), this,SLOT(repeatState(int)));
//    connect(v,SIGNAL(playButtonClicked()),this,SLOT(playPauseToggle()));

    // application signals to interface
    connect(this,SIGNAL(playlistIndexChange(QVariant)), v, SLOT(setControlViewListIndex(QVariant)));
    connect(this,SIGNAL(updateAlbumCover(QVariant)), v,SLOT(updateAlbumCover(QVariant)));
    connect(this,SIGNAL(playStatus(QVariant)), v, SLOT(updatePlayMode(QVariant)));
    connect(this,SIGNAL(VolumeChange(QVariant)), v,SLOT(setMainVolume(QVariant)));
    connect(this,SIGNAL(songDuration(QVariant)), v,SLOT(setSongDuration(QVariant)));
    connect(this,SIGNAL(progress(QVariant)), v,SLOT(updateProgress(QVariant)));

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

void squeezeLiteGui::refreshScreenSettings(void)
{
    emit VolumeChange(QVariant(m_playerInfo->GetDeviceVolume()));
    emit playStatus(QVariant(m_playerInfo->GetDeviceMode()));
    emit shuffleStatus(QVariant(m_playerInfo->GetShuffleMode()));
    emit repeatStatus(QVariant(m_playerInfo->GetRepeatMode()));
    emit songDuration(QVariant(m_playerInfo->GetTrackDuration()));
    emit progress(QVariant(m_playerInfo->GetTrackPlaytime()));
}

void squeezeLiteGui::loadHomeScreen(void)
{
    if( !controlHierarchy.contains("Home")) {
        ControlListModel *model = new ControlListModel(this);
        model->appendRow(new ControlListItem("Music",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/artists_40x40.png"), model));
        model->appendRow(new ControlListItem("Internet Radio",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/plugins/cache/icons/radiomusic_40x40.png"), model));
        model->appendRow(new ControlListItem("My Apps",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/plugins/AppGallery/html/images/icon_40x40.png"), model));
        model->appendRow(new ControlListItem("Favorites",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/favorites_40x40.png"), model));
        model->appendRow(new ControlListItem("Extras",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/alarm_40x40.png"), model));
        controlHierarchy.insert("Home", model);
        rootContext()->setContextProperty("controlListModel", model);
    }
    else {
        rootContext()->setContextProperty("controlListModel", controlHierarchy["Home"]);
    }
}

void squeezeLiteGui::loadMusicScreen(void)
{
    if( !controlHierarchy.contains("MusicScreen")) {
        ControlListModel *model = new ControlListModel(this);
        model->appendRow(new ControlListItem("Artists",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/artists_40x40.png"), model));
        model->appendRow(new ControlListItem("Albums",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/albums_40x40.png"), model));
        model->appendRow(new ControlListItem("Genres",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/genres_40x40.png"), model));
        model->appendRow(new ControlListItem("Years",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/years_40x40.png"), model));
        model->appendRow(new ControlListItem("New Music",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/newmusic_40x40.png"), model));
        model->appendRow(new ControlListItem("Random Mix",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/plugins/RandomPlay/html/images/icon_40x40.png"), model));
        model->appendRow(new ControlListItem("Music Folder",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/musicfolder_40x40.png"), model));
        model->appendRow(new ControlListItem("Playlists",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/playlists_40x40.png"), model));
        model->appendRow(new ControlListItem("Search",
                                             QString("http://")+SqueezeBoxServerAddress+QString(":")+SqueezeBoxServerHttpPort+QString("/html/images/search_40x40.png"), model));
        controlHierarchy.insert("MusicScreen", model);
        rootContext()->setContextProperty("controlListModel", model);
    }
    else {
        rootContext()->setContextProperty("controlListModel", controlHierarchy["MusicScreen"]);
    }
}

void squeezeLiteGui::updateNowPlayingScreen(void)
{
    ControlListModel *model;
    int rowCounter = 0;
    bool replaceModel = false;
    if( controlHierarchy.contains("NowPlaying")) {
        model = controlHierarchy.value("NowPlaying");
        model->clear();
        replaceModel = true;
    }
    else {
        model = new ControlListModel(this);
    }

    m_playerInfo->LockMutex();

    QListIterator<TrackData> i = QListIterator<TrackData>(m_playerInfo->getCurrentPlaylist());
    while(i.hasNext()) {
        TrackData track = i.next();
        QString urlString;
        if(track.coverid.isEmpty()) {
            urlString = QString("http://%1:%2/%3")
                    .arg(SqueezeBoxServerAddress)
                    .arg(SqueezeBoxServerHttpPort)
                    .arg(QString("html/images/artists_40x40.png"));
        }
        else {
            urlString = QString("http://%1:%2/music/%3/%4")
                    .arg(SqueezeBoxServerAddress)
                    .arg(SqueezeBoxServerHttpPort)
                    .arg(QString(track.coverid))
                    .arg(QString("cover_40x40"));
        }
        model->appendRow(new ControlListItem(QString(track.title +" - "+track.artist),urlString,QString(track.song_id)));
        if(++rowCounter % 50 == 0) { // every fifty rows, process events so we don't lock the gui for too long
            // do we need to unlock the mutex while we process events?
            qApp->processEvents();
        }
    }
    m_playerInfo->UnlockMutex();

    if(replaceModel)
        controlHierarchy.insert("NowPlaying", model);
    rootContext()->setContextProperty("controlListModel", model);
}

void squeezeLiteGui::loadNowPlayingScreen(void)
{
    DEBUGF("LOAD NOW PLAYING SCREEN");
    if( !controlHierarchy.contains("NowPlaying")) {
        updateNowPlayingScreen();
    }
    else {
        rootContext()->setContextProperty("controlListModel", controlHierarchy["NowPlaying"]);
    }
//    NewSong();
}

void squeezeLiteGui::controlViewClicked(int idx)
{
}

void squeezeLiteGui::controlViewClicked(QString s)
{

}

void squeezeLiteGui::ModeChange(QString)
{

}

void squeezeLiteGui::shuffleState(int state)
{
    switch(state) {
    case SHUFFLE_BY_SONG:
        issueStandardCommand(C_SHUFFLE_BY_SONG);
        break;
    case SHUFFLE_BY_ALBUM:
        issueStandardCommand(C_SHUFFLE_BY_PLAYLIST);
        break;
    case SHUFFLE_OFF:
    default:
        issueStandardCommand(C_SHUFFLE_OFF);
        break;
    }
}

void squeezeLiteGui::repeatState(int state)
{
    switch(state){
    case REPEAT_TRACK:
        issueStandardCommand(C_REPEAT_SONG);
        break;
    case REPEAT_PLAYLIST:
        issueStandardCommand(C_REPEAT_PLAYLIST);
        break;
    default:
    case REPEAT_OFF:
        issueStandardCommand(C_REPEAT_OFF);
        break;
    }
}

void squeezeLiteGui::nextTrackClicked(void)
{
    issueStandardCommand(C_NEXTTRACK);
}

void squeezeLiteGui::prevTrackClicked(void)
{
    issueStandardCommand(C_PREVIOUSTRACK);
}

void squeezeLiteGui::playState(int state)
{
    if(state==PLAY) {
        issueStandardCommand(C_PLAY);
        m_playerInfo->SetDeviceMode(PLAY);
    }
    else {
        issueStandardCommand(C_PAUSE);
        m_playerInfo->SetDeviceMode(PAUSE);
    }
}

void squeezeLiteGui::playPauseToggle(void)
{
    if(m_playerInfo->GetDeviceMode()==PLAY) {
        issueStandardCommand(C_PAUSE);
        m_playerInfo->SetDeviceMode(PAUSE);
    }
    else {
        issueStandardCommand(C_PLAY);
        m_playerInfo->SetDeviceMode(PLAY);
    }
}

void squeezeLiteGui::volUp(void)
{

}

void squeezeLiteGui::volDown(void)
{

}

void squeezeLiteGui::NewSong()
{

}

void squeezeLiteGui::NewPlaylist(void)
{

}

void squeezeLiteGui::Mute(bool)
{

}

void squeezeLiteGui::setVolume(int vol)
{

}



void squeezeLiteGui::getplayerMACAddress(void)
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
