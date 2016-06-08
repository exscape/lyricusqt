#ifndef REVERSESEARCHMODEL_H
#define REVERSESEARCHMODEL_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include "Misc/track.h"

class ReverseSearchModel : public QObject
{
    Q_OBJECT

public:
    ReverseSearchModel();
    void updateIndex();
    void abortIndexUpdate();
    int numberOfTracksInIndex();
    QList<Track> tracksMatchingLyrics(const QString &partialLyrics);
    ~ReverseSearchModel() {}

protected:
    QVector<QString> findSongsRecursively(QString rootDir, const QString &sDir, int max_depth);
    void indexFile(const QString &path);
private:
    int id = 1;
    QSqlDatabase db;
    bool _abort = false;

signals:
    void indexingStarted(int total); // Emitted when the main indexing step begins, after having found all music file paths
    void indexingProgressUpdate(int finished, int total);
    void indexingFinished();
};

#endif // REVERSESEARCHMODEL_H
