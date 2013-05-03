#include "devicestatus.h"
#include <QQmlContext>


#ifdef SLIMDEVICE_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif


DeviceStatus::DeviceStatus(QWindow *parent) :
    QQuickView(parent)
{
    //    QByteArray m_deviceVol; // volume (0-100)
    //    bool m_deviceMute; // is device muted
    //    QByteArray m_deviceMode; // one of the following: "play", "stop" or "pause"
    //    QByteArray m_deviceRepeatMode;
    //    QByteArray m_deviceShuffleMode;
    //    bool m_isPlaying;

    //    QByteArray m_devicePlaylistName; // name of current playlist
    //    int m_devicePlaylistCount; // number of tracks in current playlist
    //    int m_devicePlaylistIndex;  // where are we in the current playlist
    //    CurrentPlayList m_devicePlayList; // all info related to the current device playlist
    //    QTime m_playerTime;   // how long have we been playing?

    //    TrackData m_currentTrack;
    //    QByteArray m_deviceCurrentSongTime; // time into current song
    //    QByteArray m_deviceCurrentSongDuration; // length of current song
    m_playState = MAX_PLAY_MODES;
    m_deviceInitialized = false;
}

void DeviceStatus::Init(QString serverAddr, QString httpPort)
{
    DEBUGF("Device Init");
    m_playerTime.start();

    DEBUGF("Initialize Interface");
    // set source
    qmlRegisterType<ControlListModel>("net.galtfamily.controlListModel",1,0,"ControlListModel");
    //    qmlRegisterType<ControlListItem>("net.galtfamily.controllistitem",1,0,"ControlListItem");
    setSource(QUrl::fromLocalFile("qml/squeezeliteqml2/squeezeliteqml2main.qml"));



    SqueezeBoxServerAddress = serverAddr;
    SqueezeBoxServerHttpPort = httpPort;

    loadHomeScreen();
    show();

    // this command gets genre, artist, album, title, year, duration, artwork_id (to get cover image)
    issueCommand(QByteArray("status 0 1000 tags:g,a,l,t,e,y,d,c \n"));

    // this command subscribes us to playlist and volume messages
    issueCommand(QByteArray("subscribe playlist,mixer,pause,sync,client \n"));
}

void DeviceStatus::processDeviceStatusMsg(QByteArray msg)
{
    DEBUGF("StatusSetupMessage start");
    /*
   * we can't yet remove the percent encoding because some of the msgs like
   * "playlist index" have a space (%20) in them and it will throw off the sectioning of the string
   * using mgs.split below if we do it now, but we do need to convert the %3A to a ':'
  */
    m_devicePlayList.clear();
    msg.replace("%3A", ":");
    QList<QByteArray> MsgList = msg.split(' ');    // put all of the status messages into an array for processing
    bool playlistInfo = false;

    QListIterator<QByteArray> i(MsgList);
    DEBUGF("list has size: " << MsgList.count());

    // note we start on the 4th field because the first 4 are <"status"> <"0"> <"1000"> <"tags:"> and we can ignore them
    for(int c = 0; c < 4 && i.hasNext(); c++)
        i.next();

    int t = 0;
    while(i.hasNext()) {
        t++;
        QString s = QString(i.next());
        DEBUGF("List item [ " << t-1 << " ] is: " << s);
        if(!playlistInfo) {
            if(s.section(':', 0, 0) == "mode")
                m_deviceMode = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "time")
                m_deviceCurrentSongTime = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "duration")
                m_deviceCurrentSongDuration = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "mixer%20volume")
                m_deviceVol = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "playlist%20repeat")
                m_deviceRepeatMode = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "playlist%20shuffle")
                m_deviceShuffleMode = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "playlist_cur_index") {
                m_devicePlaylistIndex = s.section(':', 1, 1).toInt();
                m_deviceOldPlaylistIndex = m_devicePlaylistIndex;
            }
            else if(s.section(':', 0, 0) == "playlist_tracks") {  // OK, we've gotten to the portion of the program where the playlist info is ready
                m_devicePlaylistCount = s.section(':', 1, 1).toInt();
                playlistInfo = true;
            }
        }
        else {
            if(s.section(':', 0, 0) == "playlist%20index") {
                m_devicePlayList.append(TrackData());
                m_devicePlayList.last().playlist_index = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            }
            else if(s.section(':', 0, 0) == "title")
                m_devicePlayList.last().title = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "album_id")
                m_devicePlayList.last().album_id = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "genre")
                m_devicePlayList.last().genre = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "artist")
                m_devicePlayList.last().artist = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "album")
                m_devicePlayList.last().album = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "tracknum")
                m_devicePlayList.last().tracknum = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "year")
                m_devicePlayList.last().year = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "duration")
                m_devicePlayList.last().duration = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "coverid")
                m_devicePlayList.last().coverid = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            else if(s.section(':', 0, 0) == "id")
                m_devicePlayList.last().song_id = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
        }
    }
    DEBUGF("Device Status processing ends");
    emit issueCommand(QByteArray("mode ? \n"));
    if(!m_deviceInitialized) {
        m_deviceInitialized = true;
        emit deviceStatusReady();
    }
    loadNowPlayingScreen();
    emit playlistIndexChange(QVariant(0));
    qDebug() << "index message sent";
}

void DeviceStatus::processPlaylistInteractionMsg(QByteArray msg)
{
    DEBUGF("playlist message:" << msg);
    // NOTE: this assumes that the MAC address of the player has been stripped off
    msg.replace("\n", " "); // make sure there are no line feeds!!
    msg = msg.trimmed();
    /*
  if(msg.left(8) == "duration")  // we're getting length of song
  {
    fillTimeInfo(QString(msg.right(msg.length() - 8)).trimmed());
    return;
  }
*/
    if(msg.left(16) == "playlist newsong") { // it's a subscribed message regarding a new song playing on the playlist, so process it
        DEBUGF("New Song" << msg);
        QList< QByteArray >fields = msg.split(' ');
        DEBUGF("New Song" << fields);

        m_deviceOldPlaylistIndex = m_devicePlaylistIndex;
        m_devicePlaylistIndex = fields.at(3).toInt();   // set index of current song

        // increment playlist index, but if it's greater than the number of songs in
        // the playlist, update the playlist info
        if((m_devicePlaylistIndex) >= m_devicePlaylistCount) {
            DEBUGF("Warning: playlist index is greater than the playlist size\nIndex: "
                   << m_devicePlaylistIndex << "\tCount: " << m_devicePlaylistCount);
            emit issueCommand(QByteArray("status 0 1000 tags:g,a,l,t,e,y,d,c \n"));
        }
        else {
            NewSong();
        }
    }
    //    else if(msg.left(13) == "playlist jump") { // it's a subscribed message regarding a new song playing on the playlist, so process it
    //        DEBUGF("jump");
    //        m_devicePlaylistIndex = msg.right(msg.lastIndexOf(' ')).toInt();
    //        if(devicePlaylistIndex >= devicePlaylistCount) {
    //            DEBUGF("Warning: playlist index is greater than the playlist size\nIndex: " << devicePlaylistIndex << "\tCount: " << devicePlaylistCount);
    //            SendDeviceCommand(QString("status 0 1000 tags:g,a,l,t,y,d,J \n"));
    //        }
    //        else {
    //            slotUpdateplaylistCoverFlow();  // update playlistCoverFlow to new song index
    //            NewSong();
    //        }
    //    }
    else if(msg.left(19) == "playlist loadtracks") { // it's a subscribed message regarding a new playlist, so process it
        //    SendDeviceCommand(QString("status 0 1000 tags:g,a,l,t,y,d,J \n"));
        emit issueCommand(QByteArray("status 0 1000 tags:g,a,l,t,e,y,d,c \n"));
        NewPlaylist();
    }
    else if(msg.left(18) == "playlist addtracks") { // it's a subscribed message regarding an updated playlist, so process it
        //    SendDeviceCommand(QString("status 0 1000 tags:g,a,l,t,y,d,J \n"));
        emit issueCommand(QByteArray("status 0 1000 tags:g,a,l,t,e,y,d,c \n"));
        NewPlaylist();
    }
    else if(msg.left(15) == "playlist delete") { // it's a subscribed message regarding an updated playlist, so process it
        //        SendDeviceCommand(QString("status 0 1000 tags:g,a,l,t,y,d,J \n"));
        emit issueCommand(QByteArray("status 0 1000 tags:g,a,l,t,e,y,d,c \n"));
        NewPlaylist();
    }
    else if(msg.left(12) == "mixer muting") {
        if(msg.endsWith("1")) // mute
            m_deviceMute = true;
        else
            m_deviceMute = false;
        emit Mute(m_deviceMute);
    }
    else if(msg.left(12) == "mixer volume") {
        bool ok;
        int vol = msg.right(msg.lastIndexOf(' ')).toInt(&ok);
        if(ok) {
            if(vol == 1 || vol == -1)
                m_deviceVol = QByteArray::number(m_deviceVol.toInt() + vol);
            else
                m_deviceVol = QByteArray::number(vol);
            emit VolumeChange(QVariant(vol));
        }
    }
    else if(msg.left(5) == "pause") {
        if(msg.length()==5) { // only the "pause" message meaning to toggle the current state
            if(m_playState==MAX_PLAY_MODES) {
                m_playState=PAUSE;
            } else {
                m_playState+=1;
                m_playState%=2;
            }
//            emit playStatus(QVariant(m_playState));
        } else if(msg.endsWith("1")) {
            m_isPlaying = false;
            if(m_playState==MAX_PLAY_MODES) {   // we haven't set this yet, so establish interface status
                emit playStatus(QVariant(PAUSE));
            }
            m_playState = PAUSE;
        }
        else {
            m_isPlaying = true;
            if(m_playState==MAX_PLAY_MODES) {   // we haven't set this yet, so establish interface status
                emit playStatus(QVariant(PAUSE));
                m_playState = PAUSE;
            } else
                m_playState = PLAY;
        }
    }
    else if(msg.left(9) == "mode play") { // current playing mode of "play", "pause" "stop"
        m_isPlaying = true;
        m_playState = PLAY;
        emit playStatus(QVariant(m_playState));
    }
    else if(msg.left(10) == "mode pause") { // current playing mode of "play", "pause" "stop"
        m_isPlaying = false;
        m_playState = PAUSE;
        emit playStatus(QVariant(m_playState));
    }
    else if(msg.left(9) == "mode stop") { // current playing mode of "play", "pause" "stop"
        m_isPlaying = false;
        m_playState = STOP;
        emit playStatus(QVariant(m_playState));
    }
    else if(msg.left(6) == "status") { // this is a status message, probably because of a new playlist or song
        processDeviceStatusMsg(msg);
    }
    //    else if(msg.left(6) == "client") { // this is a client message -- options are connect, disconnect, reconnect
    //        emit ClientChange(deviceMAC, msg.right(msg.length() - 6).trimmed());
    //    }

}

void DeviceStatus::loadHomeScreen(void)
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

void DeviceStatus::loadMusicScreen(void)
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

void DeviceStatus::loadNowPlayingScreen(void)
{
    DEBUGF("LOAD NOW PLAYING SCREEN");
    if( !controlHierarchy.contains("NowPlaying")) {
        QListIterator<TrackData> i( getCurrentPlaylist() );

        ControlListModel *model = new ControlListModel(this);
        DEBUGF("There are currently " << getCurrentPlaylist().count() << " items in the playlist");

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
        }
        controlHierarchy.insert("NowPlaying", model);
        rootContext()->setContextProperty("controlListModel", model);
    }
    else {
        rootContext()->setContextProperty("controlListModel", controlHierarchy["NowPlaying"]);
    }
    NewSong();
}

void DeviceStatus::controlViewClicked(int idx)
{
    DEBUGF("control view clicked with name:" << idx);
}

void DeviceStatus::controlViewClicked(QString itemClicked)
{
    DEBUGF("control view clicked with name:" << itemClicked);
    if(itemClicked=="Music") {
        loadMusicScreen();
    }
    else if (itemClicked=="Home") {
        loadHomeScreen();
    }
    else if(itemClicked=="NowPlaying") {
        loadNowPlayingScreen();
    }
}

void DeviceStatus::NewSong(void)
{
    DEBUGF("");
    if(controlHierarchy.contains("NowPlaying") &&
            controlHierarchy.value("NowPlaying")->rowCount() >= m_devicePlaylistIndex) {
        emit playlistIndexChange(QVariant(m_devicePlaylistIndex));
        TrackData track = getCurrentPlaylist().at(m_devicePlaylistIndex);
        QString imageURL;
        if(track.coverid.isEmpty()) {
            imageURL = QString("http://%1:%2/%3")
                    .arg(SqueezeBoxServerAddress)
                    .arg(SqueezeBoxServerHttpPort)
                    .arg(QString("html/images/artists_300x300.png"));
        }
        else {
            imageURL = QString("http://%1:%2/music/%3/%4")
                    .arg(SqueezeBoxServerAddress)
                    .arg(SqueezeBoxServerHttpPort)
                    .arg(QString(track.coverid))
                    .arg(QString("cover_300x300"));
        }
        emit updateAlbumCover(QVariant(imageURL));


        //        DEBUGF("ENOUGH INFO");
        //        dynamic_cast<ControlListItem*>(controlHierarchy.value("NowPlaying")->getRow(m_deviceOldPlaylistIndex))->setHighlight(false);
        //        dynamic_cast<ControlListItem*>(controlHierarchy.value("NowPlaying")->getRow(m_devicePlaylistIndex))->setHighlight(true);
    }
}

void DeviceStatus::NewPlaylist(void)
{
    DEBUGF("New Playlist");
    if(controlHierarchy.contains("NowPlaying")) {
        ControlListModel *model = controlHierarchy.value("NowPlaying");
        model->deleteLater();
        controlHierarchy.remove("NowPlaying");  // delete current playlist
    }
}

void DeviceStatus::Mute(bool)
{
    DEBUGF("");

}

void DeviceStatus::setVolume(int vol)
{
    m_deviceVol = QByteArray::number(vol);
    DEBUGF("");

}

void DeviceStatus::ModeChange(QString)
{
    DEBUGF("");

}

//QByteArray DeviceStatus::MacAddressOfResponse(QByteArray response)
//{
//    DEBUGF("");
//    if(response.contains("%3A"))
//        return response.left(27).trimmed().toLower();
//    else
//        return QByteArray();
//}

//QByteArray DeviceStatus::ResponseLessMacAddress(QByteArray response)
//{
//    DEBUGF("");
//    if(response.contains("%3A"))
//        return response.right(response.length() - 27).trimmed();
//    else
//        return response.trimmed();
//}

//QByteArray DeviceStatus::RemoveNewLineFromResponse(QByteArray response)
//{
//    DEBUGF("");
//    while(response.contains('\n'))
//        response.replace(response.indexOf('\n'), 1, " ");
//    return response.trimmed();
//}

