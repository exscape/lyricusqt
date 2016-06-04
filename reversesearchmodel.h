#ifndef REVERSESEARCHMODEL_H
#define REVERSESEARCHMODEL_H

#include <QSqlDatabase>
#include <QList>
#include "track.h"

class ReverseSearchModel
{
    void indexFilesRecursively(const QString &sDir, int max_depth);
    void indexFile(const QString &path);

public:
    ReverseSearchModel();
    void updateIndex();
    QList<Track> tracksMatchingLyrics(const QString &partialLyrics);
    ~ReverseSearchModel() {}

private:
    int id = 1;
    QSqlDatabase db;
};

#endif // REVERSESEARCHMODEL_H
