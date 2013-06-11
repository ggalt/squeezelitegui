#include "playerinfo.h"

#ifdef PLAYERINFO_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif

playerInfo::playerInfo(SlimCLI *c, QByteArray mac, QObject *parent) :
    QObject(parent)
{
    setObjectName("playerInfo");
    m_deviceVol = -1;
    m_deviceMute = true;
    m_deviceMode = MAX_PLAY_MODES;
    m_deviceRepeatMode = MAX_REPEAT_MODES;
    m_deviceShuffleMode = MAX_SHUFFLE_MODES;
    m_deviceState = UNINITIALIZED;
    m_songDuration = 1;
    m_songPlaying = 0;
    m_MaxRequestSize = MAX_REQUEST_SIZE;
    macAddress = mac;
    m_playlistModel = NULL;
    cli = c;
}

playerInfo::~playerInfo(void)
{
    if( m_playlistModel != NULL )
        m_playlistModel->deleteLater();
}

void playerInfo::Init(void)
{
    DEBUGF("Player Init");
    m_deviceState = INITIALIZED;
    m_playlistModel = new ControlListModel();
    m_playerTime.start();
    emit PlayerStatus(m_deviceState);
}

void playerInfo::processCliMessage(void)
{
    DEBUGF("")
            while(cli->MessageAvailable()) {
        QByteArray msg = cli->GetMessage();

        QRegExp MACrx("[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]%3A[0-9a-fA-F][0-9a-fA-F]");

        // if it starts with a MAC address, send it to a device for processing
        if(MACrx.indexIn(QString(msg)) >= 0) {
            if(macAddress.toLower() == MacAddressOfResponse(msg).toLower()) {
                QByteArray resp = ResponseLessMacAddress(msg);
                DEBUGF("MSG with MAC removed:" << resp.left(200));
                if(resp.startsWith("status")) {
                    processDeviceStatusMsg(resp);
                }
                else {
                    processPlaylistInteractionMsg(resp);
                }
            }
            else {  // wait!  Whose MAC address is this?
                DEBUGF(QString("Unknown MAC address: %1 our address is %2").arg(QString(MacAddressOfResponse(msg))).arg(QString(macAddress)));
            }
        }
        else {
            SystemMsgProcessing(msg);
        }
    }
}

void playerInfo::tick(void)
{
    DEBUGF("");
    if(m_deviceMode == PLAY) {
        m_songPlaying++;
        updateTime();
    }
}

void playerInfo::updateTime(void)
{
    DEBUGF("Duration:" << m_songDuration << "Time:" << m_songPlaying);
    if(m_songPlaying > m_songDuration)
        m_songPlaying = m_songDuration;

    emit PlayingTime(m_songDuration,m_songPlaying);
    QString timeStr = QString("%1:%2 / %3:%4").arg(m_songPlaying/60).arg(m_songPlaying%60)
            .arg(m_songDuration/60).arg(m_songDuration%60);
    DEBUGF("Time Text:" << timeStr);
    emit TimeText(QVariant(timeStr));
    m_deviceCurrentSongTime = QByteArray::number(m_songPlaying);
    m_deviceCurrentSongDuration = QByteArray::number(m_songDuration);
}

void playerInfo::processDeviceStatusMsg(QByteArray msg)
{
    DEBUGF("StatusSetupMessage start");
    /*
   * we can't yet remove the percent encoding because some of the msgs like
   * "playlist index" have a space (%20) in them and it will throw off the sectioning of the string
   * using mgs.split below if we do it now, but we do need to convert the %3A to a ':'
  */
    QMutexLocker m(&mutex);

    msg.replace("%3A", ":");
    QList<QByteArray> MsgList = msg.split(' ');    // put all of the status messages into an array for processing

    QListIterator<QByteArray> i(MsgList);
    DEBUGF("list has size: " << MsgList.count());

    // note we need to grab the first 4 field because the first 4 are <"status"> <"0"> <"1000"> <"tags:">
    QByteArray msgCommand = i.next();
    QByteArray msgStartPos = i.next();
    QByteArray msgEndPos = i.next();
    QByteArray msgTags = i.next();

    bool ok;
    m_listStart = msgStartPos.toInt(&ok);
    if(!ok) {
        emit playerInfoError("Failed to get proper start of list");
        return;
    }
    m_listEnd = m_listStart + m_MaxRequestSize;

    DEBUGF("######STATUS MESSAGE" << msgStartPos << m_listStart << "to"  << m_listEnd);

    if(msgStartPos=="0") {  // we haven't yet filled the player setting information
        processPlayerSettingsMsg(i);
        m_playlistModel->clear();   // empty playlist
    }
    else {  // wind through responses until we get to the end of the player information
        while(i.hasNext()){
            QString s = QString(i.next());
            if(s.section(':',0,0)=="playlist_tracks")
                break;
        }
    }

    // Get information on the playlist if any
    DEBUGF("PlayListCount=" << m_devicePlaylistCount );
    if(m_devicePlaylistCount > 0) {
        processPlaylistMsg(i);
    }
    else {  // if we process the playlist, these will be emitted there, otherwise do it here
        m_deviceState = DATAREADY;
        emit PlayerStatus(m_deviceState);
        emit PlayerInfoFilled();
    }

    DEBUGF("Device Status processing ends");
}

void playerInfo::processPlayerSettingsMsg(QListIterator<QByteArray> &i)
{
    DEBUGF("");
    while(i.hasNext()) {
        QString s = QString(i.next());
        if(s.section(':', 0, 0) == "mode")
            if((QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1())) == "play"){
                m_deviceMode = PLAY;
            } else {
                m_deviceMode = PAUSE;
            }
        else if(s.section(':', 0, 0) == "time") {
            QByteArray tempTime = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            m_deviceCurrentSongTime = tempTime.mid(0,tempTime.indexOf(".",0));
            m_songPlaying = m_deviceCurrentSongTime.toInt();
        }
        else if(s.section(':', 0, 0) == "duration") {
            QByteArray tempTime = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            m_deviceCurrentSongDuration = tempTime.mid(0,tempTime.indexOf(".",0));
            m_songDuration = m_deviceCurrentSongDuration.toInt();
        }
        else if(s.section(':', 0, 0) == "mixer%20volume")
            m_deviceVol = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1()).toInt();
        else if(s.section(':', 0, 0) == "playlist%20repeat")
            m_deviceRepeatMode = (RepeatMode)QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1()).toInt();
        else if(s.section(':', 0, 0) == "playlist%20shuffle")
            m_deviceShuffleMode = (ShuffleMode)QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1()).toInt();
        else if(s.section(':', 0, 0) == "playlist_cur_index") {
            m_devicePlaylistIndex = s.section(':', 1, 1).toInt();
        }
        else if(s.section(':', 0, 0) == "playlist_tracks") {  // OK, we've gotten to the portion of the program where the playlist info is ready
            DEBUGF("THIS IS THE PLAYLIST TRACK COUNT MESSAGE:" << s);
            m_devicePlaylistCount = s.section(':', 1, 1).toInt();
            m_devicePlayList.clear();
            return;
        }
    }
}

void playerInfo::addControlListItem(TrackData &track)
{
    QString urlString;
    if(track.coverid.isEmpty()) {
        urlString = QString("http://%1:%2/%3")
                .arg(cli->GetServerAddr())
                .arg(cli->GetServerPort())
                .arg(QString("html/images/artists_40x40.png"));
    }
    else {
        urlString = QString("http://%1:%2/music/%3/%4")
                .arg(cli->GetServerAddr())
                .arg(cli->GetServerPort())
                .arg(QString(track.coverid))
                .arg(QString("cover_40x40"));
    }
    DEBUGF("URL STRING:" << urlString);
    m_playlistModel->appendRow(new ControlListItem(QString(track.title +" - "+track.artist),urlString,QString(track.song_id)));
}

void playerInfo::processPlaylistMsg(QListIterator<QByteArray> &i)
{
    DEBUGF("START" <<  m_playerTime.elapsed());
    while(i.hasNext()) {
        QString s = QString(i.next());
        if(s.section(':', 0, 0) == "playlist%20index") {
//            if(!m_devicePlayList.isEmpty()) {
//                addControlListItem(m_devicePlayList.last());
//            }
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
        else if(s.section(':', 0, 0) == "duration") {
            QByteArray temp = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
            m_devicePlayList.last().duration = temp.left(temp.indexOf("."));
        }
        else if(s.section(':', 0, 0) == "coverid")
            m_devicePlayList.last().coverid = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
        else if(s.section(':', 0, 0) == "id")
            m_devicePlayList.last().song_id = QByteArray::fromPercentEncoding(s.section(':', 1, 1).toLatin1());
    }

//    if(!m_devicePlayList.isEmpty()) {
//        addControlListItem(m_devicePlayList.last());
//    }

    DEBUGF("END" << m_playerTime.elapsed());
    if(m_listEnd < m_devicePlaylistCount) { // we need more data
        QByteArray tempCommand;
        QByteArray num;
        tempCommand.append("status ");
        num.setNum(m_listEnd);
        tempCommand.append(num);
        tempCommand.append(" ");
        num.setNum(m_MaxRequestSize);
        tempCommand.append(num);
        tempCommand.append(" tags:g,a,l,t,e,y,d,c \n");
        emit issueCommand(tempCommand);
    } else {
        m_deviceState = NEWPLAYLIST;
        emit PlayerStatus(m_deviceState);
        emit PlayerInfoFilled();
        emit NewPlayList();
    }
}

void playerInfo::processPlaylistInteractionMsg(QByteArray msg)
{
    DEBUGF("playlist message:" << msg);
    // NOTE: this assumes that the MAC address of the player has been stripped off
    msg.replace("\n", " "); // make sure there are no line feeds!!
    msg = msg.trimmed();

    QMutexLocker m(&mutex);
    if(msg.left(8) == "duration") {  // we're getting length of song
        m_deviceCurrentSongDuration = msg.mid(9, 9-msg.indexOf(".",8));
        m_songDuration = m_deviceCurrentSongDuration.toInt();
        updateTime();
    }
    else if(msg.left(4) == "time") {
        m_deviceCurrentSongTime = msg.mid(5,5-msg.indexOf(".",4));
        m_songPlaying = m_deviceCurrentSongTime.toInt();
        updateTime();
    }
    else if(msg.left(6)== "client") {
        return;
    }
    else if(msg.left(16) == "playlist newsong") { // it's a subscribed message regarding a new song playing on the playlist, so process it
        DEBUGF("New Song" << msg);
        QList< QByteArray >fields = msg.split(' ');

        m_devicePlaylistIndex = fields.at(3).toInt();   // set index of current song

        // increment playlist index, but if it's greater than the number of songs in
        // the playlist, update the playlist info
        if((m_devicePlaylistIndex) >= m_devicePlaylistCount) {
            DEBUGF("Warning: playlist index is greater than the playlist size\nIndex: "
                   << m_devicePlaylistIndex << "\tCount: " << m_devicePlaylistCount);
            emit issueStandardCommand(C_GETSTATUS);
            //            emit issueCommand(QByteArray("status 0 1000 tags:g,a,l,t,e,y,d,c \n"));
        }
        else {
            m_currentTrack = m_devicePlayList.at(m_devicePlaylistIndex);
            emit NewSong();
            m_songDuration = m_currentTrack.duration.toInt();
            DEBUGF("Track duration =" << m_currentTrack.duration << "or" << m_songDuration);
            m_songPlaying = 0;
            updateTime();
        }
    }
    else if(msg.left(19) == "playlist loadtracks") { // it's a subscribed message regarding a new playlist, so process it
        emit issueStandardCommand(C_GETSTATUS);
    }
    else if(msg.left(18) == "playlist addtracks") { // it's a subscribed message regarding an updated playlist, so process it
        emit issueStandardCommand(C_GETSTATUS);
    }
    else if(msg.left(15) == "playlist delete") { // it's a subscribed message regarding an updated playlist, so process it
        emit issueStandardCommand(C_GETSTATUS);
        //            emit issueCommand(QByteArray("status 0 1000 tags:g,a,l,t,e,y,d,c \n"));
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
                m_deviceVol = m_deviceVol + vol;
            else
                m_deviceVol = vol;
            emit VolumeChange(QVariant(vol));
        }
    }
    else if(msg.left(5) == "pause") {
        if(msg.length()==5) { // only the "pause" message meaning to toggle the current state
            if(m_deviceMode==MAX_PLAY_MODES) {
                m_deviceMode=PAUSE;
            } else {
                m_deviceMode=TogglePlayerMode(m_deviceMode);
            }
            //            emit playStatus(QVariant(m_playState));
        } else if(msg.endsWith("1")) {
            if(m_deviceMode==MAX_PLAY_MODES) {   // we haven't set this yet, so establish interface status
                emit playStatus(QVariant(PAUSE));
            }
            m_deviceMode = PAUSE;
        }
        else {
            if(m_deviceMode==MAX_PLAY_MODES) {   // we haven't set this yet, so establish interface status
                emit playStatus(QVariant(PAUSE));
                m_deviceMode = PAUSE;
            } else
                m_deviceMode = PLAY;
        }
    }
    else if(msg.left(9) == "mode play") { // current playing mode of "play", "pause" "stop"
        m_deviceMode = PLAY;
        emit playStatus(QVariant(m_deviceMode));
    }
    else if(msg.left(10) == "mode pause") { // current playing mode of "play", "pause" "stop"
        m_deviceMode = PAUSE;
        emit playStatus(QVariant(m_deviceMode));
    }
    else if(msg.left(9) == "mode stop") { // current playing mode of "play", "pause" "stop"
        m_deviceMode = STOP;
        emit playStatus(QVariant(m_deviceMode));
    }
    else if(msg.left(6) == "status") { // this is a status message, probably because of a new playlist or song
        processDeviceStatusMsg(msg);
    }
}

void playerInfo::SystemMsgProcessing(QByteArray msg)
{
    DEBUGF("SYSTEM MESSAGE: " << msg);
}

void playerInfo::ErrorMessageSender(QString s)
{
    QString msg = objectName()+QString(Q_FUNC_INFO) + s;
    emit playerInfoError(msg);
}

playerMode playerInfo::TogglePlayerMode(playerMode p)
{
    switch(p) {
    case PLAY:
        return PAUSE;
        break;
    case PAUSE:
        return PLAY;
        break;
    case STOP:
        return PLAY;
        break;
    case MAX_PLAY_MODES:
        return PLAY;
        break;
    }
    return MAX_PLAY_MODES;
}


QByteArray playerInfo::MacAddressOfResponse(QByteArray msg)
{
    DEBUGF("");
    if(msg.contains("%3A"))
        return msg.left(27).trimmed().toLower();
    else
        return QByteArray();
}

QByteArray playerInfo::ResponseLessMacAddress(QByteArray msg)
{
    DEBUGF("");
    if(msg.contains("%3A"))
        return msg.right(msg.length() - 27).trimmed();
    else
        return msg.trimmed();
}

