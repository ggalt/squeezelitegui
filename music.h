#ifndef MUSIC_H
#define MUSIC_H

#include <QString>
#include <QUrl>
#include <QList>
#include <QAbstractItemModel>

//class TrackData
//{
//public:
//    QByteArray playlist_index;
//    QByteArray title;
//    QByteArray genre;
//    QByteArray artist;
//    QByteArray album;
//    QByteArray tracknum;
//    QByteArray year;
//    QByteArray duration;
//    QByteArray coverid;
//    QByteArray album_id;
//};

//class Album
//{
//public:
//    QByteArray songtitle;
//    QByteArray albumtitle;
//    QByteArray album_id;
//    QByteArray year;
//    QByteArray artist;
//    QByteArray artist_id;
//    QByteArray coverid;
//    QString artist_album;
//    QString albumTextKey;   // key for alphasort of album
//    QString artistTextKey;  // what is alpha sort of artist associated with album
//};

//typedef QList< TrackData > CurrentPlayList;
//typedef QHash< QString, QString > SlimItem;
//typedef QHash< QString, QStringList > SlimItemList;
//typedef QHash< QString, Album > SlimAlbumItem;

//class DatabaseInfo
//{
//public:
//    int totalAlbums;
//    int totalArtists;
//    int totalGenres;
//    int totalSongs;
//    SlimItem m_AlbumArtist2Art;         // Album+Artist name to coverid
//    SlimItem m_Artist2AlbumIds;    // Artist name to list of albums
//    SlimAlbumItem m_AlbumID2AlbumInfo;    // AlbumID to Album Info
//};


class Track : public QAbstractItemModel {
    Q_OBJECT
    Q_PROPERTY(QString Title READ getTitle WRITE setTitle)
    Q_PROPERTY(QString Genre READ getGenre WRITE setGenre)
    Q_PROPERTY(QString Artist READ getArtist WRITE setArtist)
    Q_PROPERTY(QString Album READ getAlbum WRITE setAlbum)
    Q_PROPERTY(QString TrackNum READ getTrackNum WRITE setTrackNum)
    Q_PROPERTY(QString Year READ getYear WRITE setYear)
    Q_PROPERTY(QString Duration READ getDuration WRITE setDuration)
    Q_PROPERTY(QString CoverID READ getCoverID WRITE setCoverID)
public:
    Track( QAbstractItemModel *parent = 0 );
    QString getTitle(void) { return m_title; }
    void setTitle(QString str ) { m_title = str; }
    QString getGenre (void) { return m_genre; }
    void setGenre(QString str ) { m_genre = str; }
    QString getArtist (void) { return m_artist; }
    void setArtist(QString str ) { m_artist = str; }
    QString getAlbum (void) { return m_album; }
    void setAlbum(QString str ) { m_album = str; }
    QString getTrackNum (void) { return m_tracknum; }
    void setTrackNum(QString str ) { m_tracknum = str; }
    QString getYear (void) { return m_year; }
    void setYear(QString str ) { m_year = str; }
    QString getDuration (void) { return m_duration; }
    void setDuration(QString str ) { m_duration = str; }
    QString getCoverID (void) { return m_coverid; }
    void setCoverID(QString str ) { m_coverid = str; }

private:
    QString m_title;
    QString m_genre;
    QString m_artist;
    QString m_album;
    QString m_tracknum;
    QString m_year;
    QString m_duration;
    QString m_coverid;
};

class LoadedTrack : public Track {
    Q_OBJECT
    Q_PROPERTY(QString PlaylistIndex READ getPlaylistIndex WRITE setPlaylistIndex)
public:
    LoadedTrack( Track *parent = 0 );

    QString getPlaylistIndex(void) { return m_playlist_index; }
    void setPlaylistIndex( QString str ) { m_playlist_index = str; }

private:
    QString m_playlist_index;
};

class Album : public QAbstractItemModel {
    Q_OBJECT
    Q_PROPERTY(QString album READ getAlbum WRITE setAlbum)
    Q_PROPERTY(QString ArtWork READ getArtWork WRITE setArtWork)
    Q_PROPERTY(QString AlbumID READ getAlbumID WRITE setAlbumID)
    Q_PROPERTY(QString ArtistID READ getArtistID WRITE setArtistID)
    Q_PROPERTY(QString Year READ getYear WRITE setYear)
    Q_PROPERTY(QString albumTextKey READ getalbumTextKey WRITE setalbumTextKey)
    Q_PROPERTY(QString artistTextKey READ getartistTextKey WRITE setartistTextKey)
//    Q_PROPERTY(QList<Track> Tracks READ getTracks)

public:
    Album( QAbstractItemModel *parent = 0 );
    void AddTrack(Track *s);

    QString getAlbum(void) { return m_album; }
    void setAlbum(QString str) { m_album = str; }
    QString getArtWork(void) { return m_artWork; }
    void setArtWork(QString str) { m_artWork = str; }
    QString getAlbumID(void) { return m_albumID; }
    void setAlbumID(QString str) { m_albumID = str; }
    QString getArtistID(void) { return m_artistID; }
    void setArtistID(QString str) { m_artistID = str; }
    QString getYear(void) { return m_year; }
    void setYear(QString str) { m_year = str; }
    QString getalbumTextKey(void) { return m_albumTextKey; }
    void setalbumTextKey(QString str) { m_albumTextKey = str; }
    QString getartistTextKey(void) { return m_artistTextKey; }
    void setartistTextKey(QString str) { m_artistTextKey = str; }
//    QList<Track> getTracks(void) { return m_tracks; }

private:
    QString m_album;
    QString m_artWork;
    QString m_albumID;
    QString m_artistID;
    QString m_year;
    QString m_albumTextKey;   // key for alphasort of album
    QString m_artistTextKey;  // what is alpha sort of artist associated with album
    QList<Track>m_tracks;
};


class Artist : public QAbstractItemModel {
    Q_OBJECT
    Q_PROPERTY(QString artist READ getArtist WRITE setArtist)
    Q_PROPERTY(QString ID READ getID WRITE setID)
    Q_PROPERTY(QString TextKey READ getTextKey WRITE setTextKey)
    Q_PROPERTY(QString Artwork READ getArtwork WRITE setArtwork)
//    Q_PROPERTY(QList<Album> AlbumList READ getAlbumList)

public:
    Artist( QAbstractItemModel *parent = 0 );
    void AddAlbum( Album *album );

    QString getArtist(void) { return m_artist; }
    void setArtist(QString str) { m_artist = str; }
    QString getID (void) { return m_id; }
    void setID(QString str) { m_id = str; }
    QString getTextKey (void) { return m_textkey; }
    void setTextKey(QString str) { m_textkey = str; }
    QString getArtwork (void) { return m_artwork; }
    void setArtwork(QString str) { m_artwork = str; }
//    QList<Album> getAlbumList (void) { return m_albums; }

private:
    QString m_artist;
    QString m_artwork;
    QString m_id;
    QString m_textkey;
    QList< Album >m_albums;
};

#endif // MUSIC_H
