#ifndef LYRICSITE_H
#define LYRICSITE_H

#include <QString>
#include <functional>

enum class FetchResult {
    Success = 0,
    InvalidRequest,
    ConnectionFailed,
    RequestFailed,
    ParsingFailed,
    NoMatch
};

class LyricSite
{
public:
    LyricSite();
    virtual void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) = 0;
};

#endif // LYRICSITE_H
