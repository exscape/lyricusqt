#ifndef AZLYRICSSITE_H
#define AZLYRICSSITE_H

#include "Sites/lyricsite.h"
#include <functional>
#include <QtNetwork/QNetworkAccessManager>
#include <QHash>
#include <QPair>
#include <QUrl>

class AZLyricsSite : public LyricSite
{
public:
    AZLyricsSite();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) override;
    QString siteName() override { return "AZLyrics"; }
    ~AZLyricsSite() override {}
private:
    void artistSearchResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void lyricsPageResponseHandler(std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);

    QNetworkAccessManager accessManager;
    QHash<QPair<QString, QString>, QUrl> titleURLCache;
    QList<QString> nonExistentArtistCache;
};

#endif // AZLYRICSSITE_H
