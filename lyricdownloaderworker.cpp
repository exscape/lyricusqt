#include "lyricdownloaderworker.h"
#include "shared.h"

// Note that this constructor COPIES filesToProcess, to avoid having to share data
// between threads.
LyricDownloaderWorker::LyricDownloaderWorker(const QList<QString> &files, QObject *parent) : QObject(parent), filesToProcess(files) {
}

void LyricDownloaderWorker::fetchFinished(QString lyrics, FetchResult result) {
    if (result == FetchResult::Success) {
        // Save lyrics
//      if (setLyricsForFile(filesToProcess[currentIndex], lyrics)) {
        if (true) {
            // Success
            emit updateProgress(currentIndex, LyricStatus::LyricsDownloaded);
            qDebug() << "LyricDownloadWorker::fetchFinished: saved lyrics to" << filesToProcess[currentIndex];
        }
        else {
            // Failure
            emit updateProgress(currentIndex, LyricStatus::DownloadFailed);
        }
    }
    else if (result == FetchResult::NoMatch)
        emit updateProgress(currentIndex, LyricStatus::NoLyricsFound);
    else
        emit updateProgress(currentIndex, LyricStatus::DownloadFailed);

    process();
}

void LyricDownloaderWorker::process() {
    if (lyricFetcher == nullptr)
        lyricFetcher = new LyricFetcher;

    currentIndex++;
    if (currentIndex >= filesToProcess.length()) {
        emit finished();
        return;
    }

    // TODO: check for existing lyrics and emit HadLyrics if necessary
    if (lyricsForFile(filesToProcess[currentIndex]).length() > 0) {
        qDebug() << "LyricDownloadWorker::process: " << filesToProcess[currentIndex] << "already had lyrics, skipping";
        emit updateProgress(currentIndex, LyricStatus::HadLyrics);
        QMetaObject::invokeMethod(this, "process", Qt::QueuedConnection); // To avoid increasing the recursion depth by calling process() as usual
        return;
    }

    auto pair = artistAndTitleForFile(filesToProcess[currentIndex]);
    QString artist = pair.first;
    QString title = pair.second;

    qDebug() << "Calling fetchLyrics for" << artist << "-" << title;
    connect(lyricFetcher, &LyricFetcher::fetchFinished, this, &LyricDownloaderWorker::fetchFinished, static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    lyricFetcher->fetchLyrics(artist, title);
}
