#include "Models/lyricfetcher.h"
#include "Misc/application.h"
#include <QRegularExpression>
#include <QVector>
#include <QPair>

LyricFetcher::LyricFetcher(QObject *parent) : QObject(parent) {
    darkLyrics = new DarkLyricsSite;
    AZLyrics = new AZLyricsSite;
    songMeanings = new SongmeaningsSite;

    lyricSitesHash["DarkLyrics"] = darkLyrics;
    lyricSitesHash["AZLyrics"] = AZLyrics;
    lyricSitesHash["Songmeanings"] = songMeanings;
}

void LyricFetcher::fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback, int siteIndex) {
    Q_ASSERT(siteIndex < lyricSites.length());

//  qDebug() << "LyricFetcher: trying site #" << siteIndex << QString("(%1)").arg(lyricSites[siteIndex]->siteName());
    LyricSite *site = lyricSites[siteIndex];
    site->fetchLyrics(artist, title, [=](const QString &lyrics, FetchResult result) {
        if (result == FetchResult::Success) {
//          qDebug() << "LyricFetcher: site #" << siteIndex << "gave an answer, returning the result!";
            callback(lyrics, result);
        }
        else if (siteIndex + 1 < lyricSites.length()) {
            // There are other sites to try!
//          qDebug() << "LyricFetcher: site #" << siteIndex << "didn't give a useful reply, trying the next site";
            fetchLyrics(artist, title, callback, siteIndex + 1);
        }
        else {
            // Time to give up.
//          qDebug() << "LyricFetcher: no site gave a useful reply, giving up!";
            callback({}, result);
        }
    });
}

void LyricFetcher::fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) {
    // Set up the list of sites to use.
    // It's a bit wasteful to do this here, but we can't do it in the constructor, since that's basically called once per application start.
    // That would mean that settings would only take effect after an application restart.
    lyricSites.clear();

    auto sites = Application::getSetting("sitePriority").value<QVector<QPair<bool, QString>>>();
    for (const QPair<bool, QString> &site : sites) {
        if (site.first && lyricSitesHash.contains(site.second)) {
            // If this site is enabled, add it to the list of sites to use.
            lyricSites.append(lyricSitesHash[site.second]);
        }
    }

    // Ignore "(live)", "(live at ...)" and similar comments that aren't really part of the track name
    // TODO: these leave a lot of similar stuff behind
    QString fixedTitle = title;
    fixedTitle = fixedTitle.replace(QRegularExpression("\\(live.*", QRegularExpression::CaseInsensitiveOption), "");
    fixedTitle = fixedTitle.replace(QRegularExpression("\\(demo.*", QRegularExpression::CaseInsensitiveOption), "");
    fixedTitle = fixedTitle.replace(QRegularExpression("\\(bonus.*", QRegularExpression::CaseInsensitiveOption), "");
    fixedTitle = fixedTitle.replace(QRegularExpression("\\(.*? version\\)", QRegularExpression::CaseInsensitiveOption), "");

    // Trim whitespace
    fixedTitle = fixedTitle.replace(QRegularExpression("^\\s+|\\s+$"), "");
    return fetchLyrics(artist, fixedTitle, callback, 0);
}

void LyricFetcher::fetchLyrics(const QString &artist, const QString &title) {
    fetchLyrics(artist, title, [=](QString lyrics, FetchResult result) {
//      qDebug() << "!!! LyricFetcher::fetchLyrics wrapper callback called for" << artist << "-" << title;
        emit fetchFinished(lyrics, result);
    });
}
