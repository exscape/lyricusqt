#ifndef LYRICDOWNLOADERWORKER_H
#define LYRICDOWNLOADERWORKER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QPair>
#include "lyricfetcher.h"

enum class LyricStatus {
    NoLyricsFound = 0,
    DownloadFailed,
    HadLyrics,
    LyricsDownloaded,
    NotProcessed,
};

Q_DECLARE_METATYPE(LyricStatus)

class LyricDownloaderWorker : public QObject
{
    Q_OBJECT

    bool _abort = false;
    bool overwriteLyrics;
    int currentIndex = -1;
    int lastProcessedIndex = -1;
    QList<QPair<int, QString>> filesToProcess;
    LyricFetcher *lyricFetcher = nullptr;

public:
    explicit LyricDownloaderWorker(const QList<QPair<int, QString>> &files, bool overwrite, QObject *parent = 0);

    void fetchFinished(QString lyrics, FetchResult result);
signals:
    void finished();
    void aborted(int lastProcessedIndex);
    void updateProgress(int index, LyricStatus status);

public slots:
    void process();
    void abort();
};

#endif // LYRICDOWNLOADERWORKER_H
