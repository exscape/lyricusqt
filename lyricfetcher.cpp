#include "lyricfetcher.h"
#include <QRegularExpression>

LyricFetcher::LyricFetcher() {
    darkLyrics = new DarkLyricsSite;
    AZLyrics = new AZLyricsSite;
    songMeanings = new SongmeaningsSite;

    // TODO: create a UI to order and enable/disable sites
   lyricSites = { darkLyrics, songMeanings, AZLyrics };
}

void LyricFetcher::fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback, int siteIndex) {
    Q_ASSERT(siteIndex < lyricSites.length());

    // Ignore "(live)", "(live at ...)" and similar comments that aren't really part of the track name
    // TODO: these leave a lot of similar stuff behind
    QString fixedTitle = title;
    fixedTitle = fixedTitle.replace(QRegularExpression("\\(live.*", QRegularExpression::CaseInsensitiveOption), "");
    fixedTitle = fixedTitle.replace(QRegularExpression("\\(demo.*", QRegularExpression::CaseInsensitiveOption), "");
    fixedTitle = fixedTitle.replace(QRegularExpression("\\(bonus.*", QRegularExpression::CaseInsensitiveOption), "");
    fixedTitle = fixedTitle.replace(QRegularExpression("\\(.*? version\\)", QRegularExpression::CaseInsensitiveOption), "");

    // Trim whitespace
    fixedTitle = fixedTitle.replace(QRegularExpression("^\\s+|\\s+$"), "");

    qDebug() << "LyricFetcher: trying site #" << siteIndex << QString("(%1)").arg(lyricSites[siteIndex]->siteName());
    LyricSite *site = lyricSites[siteIndex];
    site->fetchLyrics(artist, title, [=](const QString &lyrics, FetchResult result) {
        if (result == FetchResult::Success) {
            qDebug() << "LyricFetcher: site #" << siteIndex << "gave an answer, returning the result!";
            callback(lyrics, result);
        }
        else if (siteIndex + 1 < lyricSites.length()) {
            // There are other sites to try!
            qDebug() << "LyricFetcher: site #" << siteIndex << "didn't give a useful reply, trying the next site";
            fetchLyrics(artist, title, callback, siteIndex + 1);
        }
        else {
            // Time to give up.
            qDebug() << "LyricFetcher: no site gave a useful reply, giving up!";
            callback({}, result);
        }
    });
}

void LyricFetcher::fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) {
    qDebug() << "LyricFetcher: got request for" << artist << "-" << title;
    return fetchLyrics(artist, title, callback, 0);
}
