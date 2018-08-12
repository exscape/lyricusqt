#ifndef DARKLYRICSSITE_H
#define DARKLYRICSSITE_H

#include "Sites/lyricsite.h"
#include <functional>
#include <QtNetwork/QNetworkAccessManager>
#include <QHash>
#include <QPair>
#include <QUrl>

class DarkLyricsSite : public LyricSite
{
public:
    DarkLyricsSite();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) override;
    QString siteName() override { return "DarkLyrics"; }
    ~DarkLyricsSite() override {}
private:
    QPair<QString, FetchResult> getArtistURL(const QString &_artist) const;
    void artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);
    void lyricsPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply);

    QNetworkAccessManager accessManager;
    QHash<QPair<QString, QString>, QUrl> titleURLCache;
    QHash<QPair<QString, QString>, QString> lyricsCache;
    QList<QString> nonExistentArtistCache;
};

#endif // DARKLYRICSSITE_H
