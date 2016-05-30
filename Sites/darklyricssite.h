#ifndef DARKLYRICSSITE_H
#define DARKLYRICSSITE_H

#include "lyricsite.h"
#include <functional>
#include <tuple>

class DarkLyricsSite : public LyricSite
{
public:
    DarkLyricsSite();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) override;
private:
    std::tuple<QString, FetchResult> getArtistURL(const QString &_artist) const;
    std::tuple<QString, FetchResult> getTrackURL(const QString &title, const QString &artistURL) const;
};

#endif // DARKLYRICSSITE_H
