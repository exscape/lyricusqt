#ifndef LYRICFETCHER_H
#define LYRICFETCHER_H

#include <Sites/darklyricssite.h>
#include <Sites/azlyricssite.h>
#include <QString>
#include <functional>

class LyricFetcher
{
public:
    LyricFetcher();
    void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback);
private:
    DarkLyricsSite *darkLyrics = nullptr;
    AZLyricsSite *AZLyrics = nullptr;
};

#endif // LYRICFETCHER_H
