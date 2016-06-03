#include "azlyricssite.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

AZLyricsSite::AZLyricsSite() {
}

// TODO: Ensure that Qt handles the caching, so that the page is downloaded only once per session,
// TODO: even if we make 10 requests for the same album.

void AZLyricsSite::fetchLyrics(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback) {
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
        QString simplifiedArtist = artist.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");
        foundArtist = foundArtist.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");

        if (simplifiedArtist == foundArtist) {
            QNetworkRequest networkRequest((QUrl(url)));
            networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
            QNetworkReply *reply = accessManager.get(networkRequest);
            QObject::connect(reply, &QNetworkReply::finished, [=] {
                artistPageResponseHandler(title, callback, reply);
            });

            return;
        }
    }

    callback({}, FetchResult::NoMatch);
}

void AZLyricsSite::artistPageResponseHandler(const QString &title, std::function<void (const QString &, FetchResult)> callback, QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error()) {
        qDebug() << "AZLyricsSite::titleResponseHandler error:" << reply->errorString();
        callback(QString(), FetchResult::RequestFailed); // TODO: use more specific errors
        return;
    }

    QString receivedHTML = QString(reply->readAll());
    QRegularExpression re(R"##(<a href="../(lyrics/.*)" target="_blank">(.*?)</a>)##");

    QRegularExpressionMatchIterator matchIterator = re.globalMatch(receivedHTML);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        QString url = match.captured(1);
        QString foundTitle = match.captured(2);
        QString simplifiedTitle = title.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");
        foundTitle = foundTitle.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");

        if (simplifiedTitle == foundTitle) {
            url = "http://www.azlyrics.com/" + url.replace(QRegularExpression("#\\d+$"), "");

            // Okay! We have the final URL; we need to fetch it, followed by "parsing" it (with regular expressions,
            // since that works well in practice -- no need to actually parse with a proper HTML parser).
            QNetworkRequest networkRequest((QUrl(url)));
            networkRequest.setRawHeader("User-Agent", "Google Chrome 50");
            QNetworkReply *reply = accessManager.get(networkRequest);
            QObject::connect(reply, &QNetworkReply::finished, [=] {
                lyricsPageResponseHandler(callback, reply);
            });

            return;
        }
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
