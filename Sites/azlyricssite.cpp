#include "Sites/azlyricssite.h"
#include "Misc/shared.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

AZLyricsSite::AZLyricsSite() {
}

void AZLyricsSite::fetchLyrics(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback) {
    QString simplifiedArtist = simplifiedRepresentation(artist);
    QString simplifiedTitle = simplifiedRepresentation(title);

    if (nonExistentArtistCache.contains(simplifiedArtist)) {
        // We need to check this, so that we don't request the same page dozens or even hundreds of times,
        // getting a 404/no result every single time.
        callback({}, FetchResult::NoMatch);
        return;
    }

    if (titleURLCache.contains({simplifiedArtist, simplifiedTitle})) {
        // We had the URL cached, so we don't have to search for the artist, and all that!
        QUrl url = titleURLCache[{simplifiedArtist, simplifiedTitle}];
//      qDebug() << "URL was cached!" << artist << "-" << title << "==>" << url;
        QNetworkRequest networkRequest(url);
        networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
        QNetworkReply *reply = accessManager.get(networkRequest);
        QObject::connect(reply, &QNetworkReply::finished, [=] {
            lyricsPageResponseHandler(callback, reply);
        });

        return;
    }

    // If we got here, we didn't have anything cached, and need to figure out the artist page URL first.
    // Unfortunately, unlike DarkLyrics (which appears to be affiliated with AZLyrics), AZLyrics doesn't use a simple
    // naming scheme for artist pages. "Michael Jackson" is under "/j/jackson", while Gary Moore is under "/g/garymoore",
    // and in cases where multiple artists have the same last name, it seems random which (if any!) uses that last name
    // without the first name.
    //
    // To take care of this, we use the search function to find the artist page URL.

    // QUrl handles escaping characters automatically, below
    QString searchURL = QString("http://search.azlyrics.com/search.php?q=%1&p=0&w=artists").arg(artist);

    QNetworkRequest networkRequest((QUrl(searchURL)));
    networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
    QNetworkReply *reply = accessManager.get(networkRequest);
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        artistSearchResponseHandler(artist, title, callback, reply);
    });
}

void AZLyricsSite::artistSearchResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        qDebug() << "AZLyricsSite::artistResponseHandler error:" << reply->errorString();
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    QString receivedHTML = QString(reply->readAll());
    QRegularExpression re(R"##(<tr><td[^>]+>\s*\d+\.\s*<a href="(.*?)" target="_blank"><b>(.*?)</b></a>)##");

    QRegularExpressionMatchIterator matchIterator = re.globalMatch(receivedHTML);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        QString url = match.captured(1);
        QString foundArtist = match.captured(2);

        if (simplifiedRepresentation(artist) == simplifiedRepresentation(foundArtist)) {
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

void AZLyricsSite::artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        qDebug() << "AZLyricsSite::titleResponseHandler error:" << reply->errorString();
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    QString receivedHTML = QString(reply->readAll());
    QRegularExpression re(R"##(<a href="../(lyrics/.*)(?:\\d+#)?" target="_blank">(.*?)</a>)##");

    QRegularExpressionMatchIterator matchIterator = re.globalMatch(receivedHTML);

    QString simplifiedArtist = simplifiedRepresentation(artist);
    QString simplifiedTitle = simplifiedRepresentation(title);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        QString url = "http://www.azlyrics.com/" + match.captured(1);
        QString foundTitle = simplifiedRepresentation(match.captured(2));
        titleURLCache[{simplifiedArtist, foundTitle}] = QUrl(url);
    }

    if (titleURLCache.contains({simplifiedArtist, simplifiedTitle})) {
        // Okay! We have the final URL; we need to fetch it, followed by "parsing" it (with regular expressions,
        // since that works well in practice -- no need to actually parse with a proper HTML parser).
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

void AZLyricsSite::lyricsPageResponseHandler(std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    QString receivedHTML = QString(reply->readAll());
    QRegularExpression re(R"##(<div>\s*<!-- Usage of azlyrics.*? -->\s*(.*?)\s*</div>)##");
    re.setPatternOptions(re.patternOptions() | QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(receivedHTML);

    if (match.hasMatch()) {
        QString foundLyrics = match.captured(1).replace(QRegularExpression("<[^>]*>"), ""); // Remove HTML tags
        foundLyrics = foundLyrics.replace(QRegularExpression("\\A\\s+|\\s+\\Z"), ""); // Trim whitespace from beginning and end
        callback(foundLyrics, FetchResult::Success);
        return;
    }

    // If we get here, something likely went wrong. We KNOW that the lyrics for this track should be on this page.
    // Therefore, this most likely happened because of a parsing error, e.g. because the site has been updated such
    // that the regex no longer matches correctly.
    callback({}, FetchResult::ParsingFailed);
}
