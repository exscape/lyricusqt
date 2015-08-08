#ifndef TRACK_H
#define TRACK_H

#include <QString>

struct Track
{
    Track(const QString &artist, const QString &title, const QString &album, const QString &lyrics) :
        artist(artist), title(title), album(album), lyrics(lyrics) {}
    QString artist;
    QString title;
    QString album;
    QString lyrics;
    ~Track() {}
};

#endif // TRACK_H
