#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QSqlDatabase>
#include <QList>
#include "track.h"

class DataModel
{
    void indexFilesRecursively(const QString &sDir, int max_depth);
    void indexFile(const QString &path);

public:
    DataModel();
    void updateIndex();
    QList<Track> tracksMatchingLyrics(const QString &partialLyrics);
    ~DataModel() {}

private:
    int id = 1;
    QString lyricsForFile(const QString &path);
    bool setLyricsForFile(const QString &path, const QString &lyrics);
    QSqlDatabase db;
};

#endif // DATAMODEL_H
