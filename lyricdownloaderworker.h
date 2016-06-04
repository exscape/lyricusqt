#ifndef LYRICDOWNLOADERWORKER_H
#define LYRICDOWNLOADERWORKER_H

#include <QObject>
#include <QList>
#include <QString>
#include "lyricfetcher.h"

enum class LyricStatus {
    NoLyricsFound = 0,
    DownloadFailed,
    HadLyrics,
    LyricsDownloaded,
};

class LyricDownloaderWorker : public QObject
{
    Q_OBJECT

    int currentIndex = -1;
    QList<QString> filesToProcess;
    LyricFetcher *lyricFetcher = nullptr;

public:
    explicit LyricDownloaderWorker(const QList<QString> &files, QObject *parent = 0);

    void fetchFinished(QString lyrics, FetchResult result);
signals:
    void finished();
    void updateProgress(int index, LyricStatus status);

public slots:
    void process();
};

#endif // LYRICDOWNLOADERWORKER_H
