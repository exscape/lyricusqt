#ifndef LYRICFETCHER_H
#define LYRICFETCHER_H

#include <Sites/darklyricssite.h>
#include <Sites/azlyricssite.h>
#include <Sites/songmeaningssite.h>
#include <QString>
#include <QList>
#include <functional>

class LyricFetcher
{
public:
    LyricFetcher();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback);
private:
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback, int siteIndex);
    DarkLyricsSite *darkLyrics = nullptr;
    AZLyricsSite *AZLyrics = nullptr;
    SongmeaningsSite *songMeanings = nullptr;

    QList<LyricSite*> lyricSites;
};

#endif // LYRICFETCHER_H
