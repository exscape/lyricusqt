#ifndef SONGMEANINGSSITE_H
#define SONGMEANINGSSITE_H

#include "lyricsite.h"
#include <functional>
#include <tuple>
#include <QtNetwork/QNetworkAccessManager>

class SongmeaningsSite : public LyricSite
{
public:
    SongmeaningsSite();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) override;
private:
    std::tuple<QString, FetchResult> getArtistURL(const QString &_artist) const;
    void artistSearchResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void parseArtistPage(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, const QString &receivedHTML);
    void titleResponseHandler(const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void lyricsResponseHandler(std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    QString siteName() override { return "Songmeanings"; }

    QNetworkAccessManager accessManager;
};

#endif // SONGMEANINGSSITE_H