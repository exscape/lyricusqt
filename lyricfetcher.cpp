#include "lyricfetcher.h"
#include <QRegularExpression>

LyricFetcher::LyricFetcher() {
    darkLyrics = new DarkLyricsSite;
    AZLyrics = new AZLyricsSite;
}

void LyricFetcher::fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) {
    // Ignore "(live)", "(live at ...)" and similar comments that aren't really part of the track name
    QString fixedTitle = title;
    if (fixedTitle.contains("(live", Qt::CaseInsensitive) || fixedTitle.contains("(demo", Qt::CaseInsensitive)) {
        fixedTitle = fixedTitle.replace(QRegularExpression("\\(live.*", QRegularExpression::CaseInsensitiveOption), "");
        fixedTitle = fixedTitle.replace(QRegularExpression("\\(demo.*", QRegularExpression::CaseInsensitiveOption), "");
    }

    // Trim whitespace
    fixedTitle = fixedTitle.replace(QRegularExpression("^\\s+|\\s+$"), "");

    //darkLyrics->fetchLyrics(artist, fixedTitle, callback);
    AZLyrics->fetchLyrics(artist, fixedTitle, callback);
}
