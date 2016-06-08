#include "Sites/songmeaningssite.h"
#include "Misc/shared.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

SongmeaningsSite::SongmeaningsSite() {
}

void SongmeaningsSite::fetchLyrics(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback) {
    // First, check if we have the lyrics URL in our cache.
    // If we do, we can skip one or even two HTTP requests (search results for an artist, and the artist page itself)!
    QString simplifiedArtist = simplifiedRepresentation(artist);
    QString simplifiedTitle = simplifiedRepresentation(title);

    if (nonExistentArtistCache.contains(simplifiedArtist)) {
        callback({}, FetchResult::NoMatch);
        return;
    }

    if (titleURLCache.contains({simplifiedArtist, simplifiedTitle})) {
        // Nice. That means we can set up a request for the lyrics, instead of searching for the artist.
        QUrl url = titleURLCache[{simplifiedArtist, simplifiedTitle}];
        qDebug() << "URL was cached!" << artist << "-" << title << "==>" << url;
        QNetworkRequest networkRequest(url);
        networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
        QNetworkReply *reply = accessManager.get(networkRequest);
        QObject::connect(reply, &QNetworkReply::finished, [=] {
            lyricsPageResponseHandler(callback, reply);
        });

        return;
    }

    // If we got here, we had no such luck, and we'll have to start at the beginning.
    // QUrl handles escaping characters automatically, below
    QString searchURL = QString("http://songmeanings.com/query/?query=%1&type=artists").arg(artist);

    QNetworkRequest networkRequest((QUrl(searchURL)));
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
    QNetworkReply *reply = accessManager.get(networkRequest);
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        artistSearchResponseHandler(artist, title, callback, reply);
    });
}

void SongmeaningsSite::artistSearchResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        qDebug() << "SongmeaningsSite::artistSearchResponseHandler error:" << reply->errorString();
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    if (reply->url().toString().contains("/artist/view/songs/")) {
        // There was only a single hit for our search, and so the site redirected us directly to the artist page.
        // No need to parse a search results page and follow a link!
        parseArtistPage(artist, title, callback, QString(reply->readAll()));
        return;
    }

    QString receivedHTML = QString(reply->readAll());
    QRegularExpression re(R"##(<a href="//songmeanings.com/(artist/view/songs/\d+/?)"[^>]*>(.*?)</a>)##");
    QRegularExpressionMatchIterator matchIterator = re.globalMatch(receivedHTML);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        QString url = match.captured(1);
        QString foundArtist = match.captured(2);

        if (simplifiedRepresentation(foundArtist) == simplifiedRepresentation(artist)) {
            url = "http://songmeanings.com/" + url;
            QNetworkRequest networkRequest((QUrl(url)));
            networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
            QNetworkReply *reply = accessManager.get(networkRequest);
            QObject::connect(reply, &QNetworkReply::finished, [=] {
                artistPageResponseHandler(artist, title, callback, reply);
            });

            return;
        }
    }

    nonExistentArtistCache.append(simplifiedRepresentation(artist));
    callback({}, FetchResult::NoMatch);
}

void SongmeaningsSite::artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        qDebug() << "SongmeaningsSite::artistPageResponseHandler error:" << reply->errorString();
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    parseArtistPage(artist, title, callback, (reply->readAll()));
}

void SongmeaningsSite::parseArtistPage(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, const QString &receivedHTML) {
    QRegularExpression re(R"##(<a.*?href="//songmeanings.com/(songs/view/\d+)/?"[^>]*>(.*?)</a>)##");
    QRegularExpressionMatchIterator matchIterator = re.globalMatch(receivedHTML);

    QString simplifiedArtist = simplifiedRepresentation(artist);
    QString simplifiedTitle = simplifiedRepresentation(title);
    while (matchIterator.hasNext()) {
        // Parse and cache every track URL for future usage
        QRegularExpressionMatch match = matchIterator.next();
        QString url = "http://songmeanings.com/" + match.captured(1);
        QString foundTitle = simplifiedRepresentation(match.captured(2));
        titleURLCache[{simplifiedArtist, foundTitle}] = QUrl(url);
    }

    // Next, use the cache to get going on this request
    if (titleURLCache.contains({simplifiedArtist, simplifiedTitle})) {
        QUrl url = titleURLCache[{simplifiedArtist, simplifiedTitle}];
        QNetworkRequest networkRequest(url);
        networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
        QNetworkReply *reply = accessManager.get(networkRequest);
        QObject::connect(reply, &QNetworkReply::finished, [=] {
            lyricsPageResponseHandler(callback, reply);
        });

        return;
    }

    callback({}, FetchResult::NoMatch);
}

void SongmeaningsSite::lyricsPageResponseHandler(std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    QString receivedHTML = QString(reply->readAll());
    QRegularExpression re(R"##(<div class="holder lyric-box">(.*?)<br\s*/><div)##");
    re.setPatternOptions(re.patternOptions() | QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(receivedHTML);

    if (match.hasMatch()) {
        QString foundLyrics = match.captured(1).replace(QRegularExpression("<[^>]*>"), ""); // Remove HTML tags
        foundLyrics = foundLyrics.replace(QRegularExpression("\\A\\s+|\\s+\\Z"), ""); // Trim whitespace from beginning and end

        if (foundLyrics.contains("we are not authorized to display these lyrics", Qt::CaseInsensitive)) {
            callback(foundLyrics, FetchResult::NoMatch);
            return;
        }

        callback(foundLyrics, FetchResult::Success);
        return;
    }

    // If we get here, something likely went wrong. We KNOW that the lyrics for this track should be on this page.
    // Therefore, this most likely happened because of a parsing error, e.g. because the site has been updated such
    // that the regex no longer matches correctly.
    callback({}, FetchResult::ParsingFailed);
}
