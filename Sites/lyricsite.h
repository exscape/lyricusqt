#ifndef LYRICSITE_H
#define LYRICSITE_H

#include <QString>
#include <functional>
#include "Misc/shared.h"

class LyricSite
{
public:
    LyricSite();
    virtual void fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) = 0;
    virtual QString siteName() = 0;
    virtual ~LyricSite();
};

#endif // LYRICSITE_H
