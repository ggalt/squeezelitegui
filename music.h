#ifndef MUSIC_H
#define MUSIC_H

#include <QString>
#include <QUrl>
#include <QList>

class TrackData
{
public:
    QByteArray playlist_index;
    QByteArray title;
    QByteArray genre;
    QByteArray artist;
    QByteArray album;
    QByteArray tracknum;
    QByteArray year;
    QByteArray duration;
    QByteArray coverid;
    QByteArray album_id;
};

class Album
{
public:
    QByteArray songtitle;
    QByteArray albumtitle;
    QByteArray album_id;
    QByteArray year;
    QByteArray artist;
    QByteArray artist_id;
    QByteArray coverid;
    QString artist_album;
    QString albumTextKey;   // key for alphasort of album
    QString artistTextKey;  // what is alpha sort of artist associated with album
};

typedef QList< TrackData > CurrentPlayList;
typedef QHash< QString, QString > SlimItem;
typedef QHash< QString, QStringList > SlimItemList;
typedef QHash< QString, Album > SlimAlbumItem;

class DatabaseInfo
{
public:
    int totalAlbums;
    int totalArtists;
    int totalGenres;
    int totalSongs;
    SlimItem m_AlbumArtist2Art;         // Album+Artist name to coverid
    SlimItem m_Artist2AlbumIds;    // Artist name to list of albums
    SlimAlbumItem m_AlbumID2AlbumInfo;    // AlbumID to Album Info
};









class Track : public QAbstractItemModel {
    Q_OBJECT
    //    Q_PROPERTY
public:
    Track( QAbstractItemModel *parent = 0 );

private:
    QString m_track;
    QURL *m_artWork;
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
    Q_PROPERTY(QList<Track> Tracks READ getTracks)

public:
    Album( QAbstractItemModel *parent = 0 );
    void AddTrack(Track s);

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
    Q_PROPERTY(QList<Album> AlbumList READ getAlbumList)
public:
    Artist( QAbstractItemModel *parent = 0 );
    void AddAlbum( Album *album );

private:
    QString m_artist;
    QString m_artwork;
    QString m_id;
    QString m_textkey;
    QList< Album >m_albums;
};

#endif // MUSIC_H
