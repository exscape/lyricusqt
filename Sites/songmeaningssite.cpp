#include "songmeaningssite.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

SongmeaningsSite::SongmeaningsSite() {
}

// TODO: Ensure that Qt handles the caching, so that the page is downloaded only once per session,
// TODO: even if we make 10 requests for the same album.
// TODO: See: QNetworkDiskCache -- REMEMBER THAT EACH SITE HAS ITS OWN QNAM FOR NOW! This means multiple places to set up a cache!
// TODO: Attempt to share the QNAM using the LyricFetcher instance!
// TODO: NOTE that each thread needs its own QNAM for thread safety (or locking needs to be used everywhere)!!!

void SongmeaningsSite::fetchLyrics(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback) {
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
        QString simplifiedArtist = artist.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");
        foundArtist = foundArtist.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");

        if (simplifiedArtist == foundArtist) {
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

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        QString url = match.captured(1);
        QString foundTitle = match.captured(2);
        QString simplifiedTitle = title.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");
        foundTitle = foundTitle.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");

        if (simplifiedTitle == foundTitle) {
            url = "http://songmeanings.com/" + url;
            QNetworkRequest networkRequest((QUrl(url)));
            networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
            QNetworkReply *reply = accessManager.get(networkRequest);
            QObject::connect(reply, &QNetworkReply::finished, [=] {
                lyricsResponseHandler(callback, reply);
            });

            return;
        }
    }

    callback({}, FetchResult::NoMatch);
}

void SongmeaningsSite::lyricsResponseHandler(std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
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
