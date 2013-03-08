#include "music.h"

Track::Track( QAbstractItemModel *parent) : QAbstractItemModel(parent) {}
//QString Track::getTitle(void) { return m_title; }
//void Track::setTitle(QString str ) { m_title = str; }
//QString Track::getGenre (void) { return m_genre; }
//void Track::setGenre(QString str ) { m_genre = str; }
//QString Track::getArtist (void) { return m_artist; }
//void Track::setArtist(QString str ) { m_artist = str; }
//QString Track::getAlbum (void) { return m_album; }
//void Track::setAlbum(QString str ) { m_album = str; }
//QString Track::getTrackNum (void) { return m_trackum; }
//void Track::setTrackNum(QString str ) { m_tracknum = str; }
//QString Track::getYear (void) { return m_year; }
//void Track::setYear(QString str ) { m_year = str; }
//QString Track::getDuration (void) { return m_duration; }
//void Track::setDuration(QString str ) { m_duration = str; }
//QString Track::getCoverID (void) { return m_coverid; }
//void Track::setCoverID(QString str ) { m_coverid = str; }

//class LoadedTrack : public Track {
//    Q_OBJECT
//    Q_PROPERTY(QString PlaylistIndex READ getPlaylistIndex WRITE setPlaylistIndex)
//public:
//    LoadedTrack( Track *parent = 0 );

//private:
//    QString m_playlist_index;
//};

Album::Album( QAbstractItemModel *parent ) : QAbstractItemModel(parent) {}
//QString Album::getAlbum(void) { return m_album; }
//void Album::setAlbum(QString str) { m_album = str; }
//QString Album::getArtWork(void) { return m_artWork; }
//void Album::setArtWork(QString str) { m_artWork = str; }
//QString Album::getAlbumID(void) { return m_albumID; }
//void Album::setAlbumID(QString str) { m_albumID = str; }
//QString Album::getArtistID(void) { return m_artistID; }
//void Album::setArtistID(QString str) { m_artistID = str; }
//QString Album::getYear(void) { return m_year; }
//void Album::setYear(QString str) { m_year = str; }
//QString Album::getalbumTextKey(void) { return m_albumTextKey; }
//void Album::setalbumTextKey(QString str) { m_albumTextKey = str; }
//QString Album::getartistTextKey(void) { return m_artistTextKey; }
//void Album::setartistTextKey(QString str) { m_artistTextKey = str; }
//QList<Track> Album::getTracks(void) { return m_tracks; }

void Album::AddTrack(Track *s)
{
}

Artist::Artist( QAbstractItemModel *parent ) : QAbstractItemModel(parent) {}
//QString Artist::getArtist(void) { return m_artist; }
//void Artist::setArtist(QString str) { m_artist = str; }
//QString Artist::getID (void) { return m_id; }
//void Artist::setID(QString str) { m_id = str; }
//QString Artist::getTextKey (void) { return m_textkey; }
//void Artist::setTextKey(QString str) { m_textkey = str; }
//QString Artist::getArtwork (void) { return m_artwork; }
//void Artist::setArtwork(QString str) { m_artwork = str; }
//QList<Album> Artist::getAlbumList (void) { return m_albums; }

void Artist::AddAlbum( Album *album )
{

}
