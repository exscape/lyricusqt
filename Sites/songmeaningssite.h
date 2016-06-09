#ifndef SONGMEANINGSSITE_H
#define SONGMEANINGSSITE_H

#include "Sites/lyricsite.h"
#include <functional>
#include <QtNetwork/QNetworkAccessManager>
#include <QHash>
#include <QPair>
#include <QUrl>

class SongmeaningsSite : public LyricSite
{
public:
    SongmeaningsSite();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) override;
    QString siteName() override { return "Songmeanings"; }
private:
    void artistSearchResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void parseArtistPage(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, const QString &receivedHTML);
    void titleResponseHandler(const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void lyricsPageResponseHandler(std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);

    QNetworkAccessManager accessManager;
    QHash<QPair<QString, QString>, QUrl> titleURLCache;
    QList<QString> nonExistentArtistCache;
};

#endif // SONGMEANINGSSITE_H
