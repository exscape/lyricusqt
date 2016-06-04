#include "lyricdownloaderworker.h"
#include "shared.h"

// Note that this constructor COPIES filesToProcess, to avoid having to share data
// between threads.
LyricDownloaderWorker::LyricDownloaderWorker(const QList<QPair<int, QString>> &files, bool overwrite, QObject *parent) : QObject(parent), overwriteLyrics(overwrite), filesToProcess(files) {
    qRegisterMetaType<LyricStatus>("LyricStatus");
}

void LyricDownloaderWorker::fetchFinished(QString lyrics, FetchResult result) {
    int listIndex = filesToProcess[currentIndex].first;
    QString path = filesToProcess[currentIndex].second;

    if (result == FetchResult::Success) {
        // Save lyrics
//      if (setLyricsForFile(path, lyrics)) {
        if (true) {
            // Success
            emit updateProgress(listIndex, LyricStatus::LyricsDownloaded);
            qDebug() << "LyricDownloadWorker::fetchFinished: saved lyrics to" << path;
        }
        else {
            // Failure
            emit updateProgress(listIndex, LyricStatus::DownloadFailed);
        }
    }
    else if (result == FetchResult::NoMatch)
        emit updateProgress(listIndex, LyricStatus::NoLyricsFound);
    else
        emit updateProgress(listIndex, LyricStatus::DownloadFailed);

    lastProcessedIndex = listIndex;
    process();
}

void LyricDownloaderWorker::process() {
    if (lyricFetcher == nullptr)
        lyricFetcher = new LyricFetcher;

    currentIndex++;
    if (currentIndex >= filesToProcess.length() || _abort) {
        if (_abort && currentIndex > 0)
            emit aborted(lastProcessedIndex);
        emit finished();
        return;
    }

    if (!overwriteLyrics && lyricsForFile(filesToProcess[currentIndex].second).length() > 0) {
        qDebug() << "LyricDownloadWorker::process: " << filesToProcess[currentIndex].second << "already had lyrics, skipping";
        emit updateProgress(filesToProcess[currentIndex].first, LyricStatus::HadLyrics);
        QMetaObject::invokeMethod(this, "process", Qt::QueuedConnection); // To avoid increasing the recursion depth by calling process() as usual
        return;
    }

    auto pair = artistAndTitleForFile(filesToProcess[currentIndex].second);
    QString artist = pair.first;
    QString title = pair.second;

    qDebug() << "Calling fetchLyrics for" << artist << "-" << title;
    connect(lyricFetcher, &LyricFetcher::fetchFinished, this, &LyricDownloaderWorker::fetchFinished, static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    lyricFetcher->fetchLyrics(artist, title);
}

void LyricDownloaderWorker::abort() {
    // This is checked by the code above, every "loop".
    _abort = true;
}
