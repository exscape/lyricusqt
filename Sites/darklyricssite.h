#ifndef DARKLYRICSSITE_H
#define DARKLYRICSSITE_H

#include "lyricsite.h"
#include <functional>
#include <tuple>
#include <QtNetwork/QNetworkAccessManager>
#include <QHash>
#include <QPair>
#include <QUrl>

class DarkLyricsSite : public LyricSite
{
public:
    DarkLyricsSite();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) override;
private:
    std::tuple<QString, FetchResult> getArtistURL(const QString &_artist) const;
    void artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void lyricsPageResponseHandler(const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    QString siteName() override { return "DarkLyrics"; }

    QNetworkAccessManager accessManager;
    QHash<QPair<QString, QString>, QUrl> titleURLCache;
};

#endif // DARKLYRICSSITE_H
