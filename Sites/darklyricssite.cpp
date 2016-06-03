#include "darklyricssite.h"
#include "shared.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

DarkLyricsSite::DarkLyricsSite() {
}

void DarkLyricsSite::fetchLyrics(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback) {
    QString simplifiedArtist = simplifiedRepresentation(artist);
    QString simplifiedTitle = simplifiedRepresentation(title);
    if (titleURLCache.contains({simplifiedArtist, simplifiedTitle})) {
        // Nice. That means we can set up a request for the lyrics immediately.
        QUrl url = titleURLCache[{simplifiedArtist, simplifiedTitle}];
        qDebug() << "URL was cached!" << artist << "-" << title << "==>" << url;
        QNetworkRequest networkRequest(url);
        QNetworkReply *reply = accessManager.get(networkRequest);
        QObject::connect(reply, &QNetworkReply::finished, [=] {
            lyricsPageResponseHandler(title, callback, reply);
        });

        return;
    }

    // If we get here, the URL wasn't cached, and so we need to start at the beginning.

    QString artistURL;
    FetchResult result;
    std::tie(artistURL, result) = getArtistURL(artist);
    if (artistURL.length() < 1 || result != FetchResult::Success) {
        callback({}, result);
        return;
    }

    // Time to figure out the track URL. We need to fetch the artist page to do that,
    // which means we need to use some asynchronous programming, due to the QNAM API.
    QNetworkRequest networkRequest((QUrl(artistURL)));
    QNetworkReply *reply = accessManager.get(networkRequest);
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        artistPageResponseHandler(artist, title, callback, reply);
    });
}

std::tuple<QString, FetchResult> DarkLyricsSite::getArtistURL(const QString &_artist) const {
    // These URLs have a simple format, so we can figure them out instead of having to
    // fetch and parse HTML. Nice and speedy!
    //
    // We begin with stripping everything but regular characters, including whitespace, and convert to lowercase.
    // E.g. "Dream Theater" -> "dreamtheater"
    QString artist = _artist;
    artist = artist.toLower().replace(QRegularExpression("[^A-Za-z0-9]"), "");

    if (artist.length() < 1)
        return std::make_tuple(QString(), FetchResult::InvalidRequest);

    if (artist[0].isDigit())
        return std::make_tuple(QString("http://www.darklyrics.com/19/%1.html").arg(artist), FetchResult::Success);
    else
        return std::make_tuple(QString("http://www.darklyrics.com/%1/%2.html").arg(QString(artist[0]), artist), FetchResult::Success);
}

void DarkLyricsSite::artistPageResponseHandler(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    QString receivedHTML = QString(reply->readAll());
    QRegularExpression re(R"##(<a href="../(lyrics/.*?)(?:#\d*)?">(.*?)</a><br />)##");
    QRegularExpressionMatchIterator matchIterator = re.globalMatch(receivedHTML);

    QString simplifiedArtist = simplifiedRepresentation(artist);
    QString simplifiedTitle = simplifiedRepresentation(title);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        QString url = "http://www.darklyrics.com/" + match.captured(1);
        QString foundTitle = simplifiedRepresentation(match.captured(2));
        titleURLCache[{simplifiedArtist, foundTitle}] = QUrl(url);
    }

    if (titleURLCache.contains({simplifiedArtist, simplifiedTitle})) {
        QUrl url = titleURLCache[{simplifiedArtist, simplifiedTitle}];

        // Okay! We have the final URL; we need to fetch it, followed by "parsing" it (with regular expressions,
        // since that works well in practice -- no need to actually parse with a proper HTML parser).
        QNetworkRequest networkRequest(url);
        QNetworkReply *reply = accessManager.get(networkRequest);
        QObject::connect(reply, &QNetworkReply::finished, [=] {
            lyricsPageResponseHandler(title, callback, reply);
        });

        return;
    }

    callback({}, FetchResult::NoMatch);
}

void DarkLyricsSite::lyricsPageResponseHandler(const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    QString receivedHTML = QString(reply->readAll());
    QRegularExpression re(R"##(<h3><a name="\d+">\d+\. ([^<]*?)</a></h3><br />\s*([\s\S]+?)(?=<br /><br />))##");
    re.setPatternOptions(re.patternOptions() | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator matchIterator = re.globalMatch(receivedHTML, 0, QRegularExpression::NormalMatch);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        QString foundTitle = match.captured(1);

        if (simplifiedRepresentation(title) == simplifiedRepresentation(foundTitle)) {
            QString foundLyrics = match.captured(2).replace(QRegularExpression("<[^>]*>"), ""); // Remove HTML tags
            foundLyrics = foundLyrics.replace(QRegularExpression("\\A\\s+|\\s+\\Z"), ""); // Trim whitespace from beginning and end
            callback(foundLyrics, FetchResult::Success);
            return;
        }
    }

    // If we get here, something likely went wrong. We KNOW that the lyrics for this track should be on this page,
    // since we got here from the artist page, which lists all the tracks.
    // Therefore, this most likely happened because of a parsing error, e.g. because the site has been updated such
    // that the regex no longer matches correctly.
    callback({}, FetchResult::ParsingFailed);
}
