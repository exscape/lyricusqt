#include "reversesearchmodel.h"
#include "shared.h"
#include "application.h"
#include <taglib/fileref.h>
#include <taglib/mpeg/mpegfile.h>
#include <taglib/mpeg/id3v2/frames/unsynchronizedlyricsframe.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mp4/mp4file.h>
#include <QString>
#include <QDebug>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QMessageBox>

ReverseSearchModel::ReverseSearchModel() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbpath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir path = dbpath;
    path.mkpath(path.absolutePath());
    dbpath.append("/test.sqlite");
    qDebug() << "Using database path:" << dbpath;
    db.setDatabaseName(dbpath);

    if (!db.open()) {
        qCritical() << "Unable to open database!";
        // TODO: error handling
        return;
    }
    else
        qDebug() << "Database opened! isValid =" << (db.isValid() ? "true" : "false");

    // This database might not have existed a few milliseconds ago, so we'll have to
    // set it up, if necessary.
    QSqlQuery createQuery;
    if(!createQuery.exec("CREATE TABLE IF NOT EXISTS data ("
                 "id INTEGER PRIMARY KEY,"
                     "artist TEXT,"
                     "title TEXT,"
                     "album TEXT,"
                     "lyrics TEXT);"
                     )) {
        QMessageBox::critical(nullptr, "Unable to create database table", "I was unable to create the database table used to store lyric data. This is a critical error, and the application will now exit.", QMessageBox::Ok, QMessageBox::NoButton);
        exit(1);
    }
    qDebug() << "Database set up/opened successfully";
}

void ReverseSearchModel::indexFile(const QString &path) {
    QString artist, title, album;
    {
        // For some reason, The MPEG::File open below fails if this is still open.
        // Additionally, there doesn't seem to be a close() method, so we'll have to
        // abuse scope a bit to get the file to close when we're done reading the tags.
        TagLib::FileRef f(QFile::encodeName(path).constData());
        if (f.isNull()) {
            qDebug() << "Open failed for" << path << ", ignoring...";
            return;
        }
        auto tag = f.tag();

        artist = tag->artist().toCString(true);
        title = tag->title().toCString(true);
        album = tag->album().toCString(true);
    }

    if (artist.length() < 1 || title.length() < 1)
        return;

    QString lyrics = lyricsForFile(path);
    if (lyrics.length() < 1)
        return;

    QSqlQuery query;
    query.prepare("INSERT INTO data (id, artist, title, album, lyrics) VALUES (:id, :artist, :title, :album, :lyrics)");
    query.bindValue(":id", id++);
    query.bindValue(":artist", artist);
    query.bindValue(":title", title);
    query.bindValue(":album", album);
    query.bindValue(":lyrics", lyrics);
    if (!query.exec()) {
        qDebug() << "Insert query for " << artist << "-" << title << "failed!";
        qDebug() << "Query was: " << query.executedQuery();
        qDebug() << "database last error: " << db.lastError().databaseText();
        qDebug() << "driver last error: " << db.lastError().driverText();
        qDebug() << "---------------------------------------------------------------";
    }
}

QList<Track> ReverseSearchModel::tracksMatchingLyrics(const QString &partialLyrics) {
    QSqlQuery query;
    query.prepare("SELECT artist,title,album,lyrics FROM data WHERE lyrics LIKE :part");
    query.bindValue(":part", "%" + partialLyrics + "%");
    query.exec();

    QList<Track> results;

    while (query.next()) {
        QString artist = query.value(0).toString().trimmed();
        QString title = query.value(1).toString().trimmed();
        QString album = query.value(2).toString().trimmed();
        QString lyrics = query.value(3).toString().trimmed();

        results.append(Track(artist, title, album, lyrics));
    }

    return results;
}

void ReverseSearchModel::updateIndex() {
    _abort = false;

    QSqlQuery query("DELETE FROM data");
    id = 1;
    query.exec();
    qDebug() << "Updating reverse search index...";

    QVector<Path> paths = Application::getSetting("pathsToIndex").value<QVector<Path>>();

    QVector<QString> songsToIndex;

    for (const Path &path : paths) {
        QDir dir(path.path);
        if (dir.exists()) {
            qDebug() << "Indexing files from" << path.path << "with a depth of" << path.depth;
            songsToIndex << findSongsRecursively(path.path, path.path, path.depth);
        }
        else
            qDebug() << "Ignoring non-existent directory" << path.path;
    }

    qDebug() << "Found" << songsToIndex.length() << "songs to index";

    if (_abort) {
        db.commit();
        return;
    }

    emit indexingStarted(songsToIndex.length());

    int finished = 0;
    const int total = songsToIndex.length();
    for (const QString &path : songsToIndex) {
        if (_abort) {
            db.commit();
            return;
        }
        indexFile(path);
        emit indexingProgressUpdate(finished++, total);
        Application::processEvents();
    }

    db.commit();
    qDebug() << "Index updated";

    emit indexingFinished();
}

QVector<QString> ReverseSearchModel::findSongsRecursively(QString rootDir, const QString &sDir, int max_depth) {
    QVector<QString> v;
    QDir dir(sDir);
    QFileInfoList list = dir.entryInfoList(dir.nameFilters(), QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo info : list) {
        if (_abort)
          return {};
        if (!rootDir.endsWith('/') && !rootDir.endsWith('\\'))
            rootDir.append('/');
        QString sFilePath = info.filePath();
        if (info.suffix().toLower() == "mp3" || info.suffix().toLower() == "m4a")
            v.push_back(info.absoluteFilePath());

        if (max_depth > 0 && info.isDir() && !info.isSymLink()) {
            v << findSongsRecursively(rootDir, sFilePath, max_depth - 1);
        }
    }

    return v;
}

int ReverseSearchModel::numberOfTracksInIndex() {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM data");
    query.exec();

    int rows = 0;
    if (query.next())
        rows = query.value(0).toInt();

    return rows;
}

void ReverseSearchModel::abortIndexUpdate() {
    _abort = true;
}
