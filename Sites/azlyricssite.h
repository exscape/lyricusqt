#ifndef AZLYRICSSITE_H
#define AZLYRICSSITE_H

#include "lyricsite.h"
#include <functional>
#include <tuple>
#include <QtNetwork/QNetworkAccessManager>
#include <QHash>
#include <QPair>
#include <QUrl>

class AZLyricsSite : public LyricSite
{
public:
    AZLyricsSite();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) override;
private:
    std::tuple<QString, FetchResult> getArtistURL(const QString &_artist) const;
    void artistSearchResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void lyricsPageResponseHandler(std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    QString siteName() override { return "AZLyrics"; }

    QNetworkAccessManager accessManager;
    QHash<QPair<QString, QString>, QUrl> titleURLCache;
};

#endif // AZLYRICSSITE_H
