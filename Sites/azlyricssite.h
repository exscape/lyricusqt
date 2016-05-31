#ifndef AZLYRICSSITE_H
#define AZLYRICSSITE_H

#include "lyricsite.h"
#include <functional>
#include <tuple>
#include <QtNetwork/QNetworkAccessManager>

class AZLyricsSite : public LyricSite
{
public:
    AZLyricsSite();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) override;
private:
    std::tuple<QString, FetchResult> getArtistURL(const QString &_artist) const;
    void artistResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void titleResponseHandler(const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void lyricsResponseHandler(std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);

    QNetworkAccessManager accessManager;
};

#endif // AZLYRICSSITE_H
