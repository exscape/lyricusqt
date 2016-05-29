#include "datamodel.h"
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
#include <QApplication>

#include <QSqlRecord>

DataModel::DataModel() {
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
        qCritical() << "CREATE TABLE query failed!";
        QMessageBox::critical(nullptr, "Unable to create database table", "I was unable to create the database table used to store lyric data. This is a critical error, and the application will now exit.", QMessageBox::Ok, QMessageBox::NoButton);
        exit(1);
    }
    qDebug() << "Database set up/opened successfully";

    /*

    while(q.next()) {
        QString title = q.value(0).toString().trimmed();
        QString lyrics = q.value(1).toString().trimmed();
        QMessageBox::information(this, title, lyrics, QMessageBox::Ok);
    }

    TagLib::FileRef f(R"(i:\Music\iTunes\iTunes Media\Music\Animals as Leaders\Animals As Leaders\01 Tempting Time.mp3)");
    auto s = f.tag()->artist();
    QString art = "Artist: ";
    art += s.toCString(true);
    QMessageBox::information(this, "Hello world!", art, QMessageBox::Ok);
    */
}

void DataModel::indexFilesRecursively(const QString &sDir, int max_depth) {
    QDir dir(sDir);
    QFileInfoList list = dir.entryInfoList(dir.nameFilters(), QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo info : list) {
//        if (Application::shouldTerminate)
//            return {};
        QString sFilePath = info.filePath();
        QString absPath = info.absoluteFilePath();

        // TODO: fix proper support for all file types
        if (absPath.endsWith(".mp3", Qt::CaseInsensitive)|| absPath.endsWith(".m4a", Qt::CaseInsensitive)) {
            indexFile(absPath);
        }

        if (max_depth > 0 && info.isDir() && !info.isSymLink()) {
            indexFilesRecursively(sFilePath, max_depth - 1);
        }
    }
}

void DataModel::indexFile(const QString &path) {
    qDebug() << "Indexing" << path;

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
//            qDebug() << "Read data:" << artist << "-" << title;
    }

    if (artist.length() < 1 || title.length() < 1)
        return;

    QString lyrics = lyricsForFile(path);
    if (lyrics.length() < 1)
        return;





    qDebug() << "Lyrics begin with:" << lyrics.mid(0, 25) << "...";

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
    else
        qDebug() << "Insert successful for" << artist << "-" << title;
}

QList<Track> DataModel::tracksMatchingLyrics(const QString &partialLyrics) {
    QSqlQuery query;
    query.prepare("SELECT artist,title,album,lyrics FROM data WHERE lyrics LIKE :part");
    query.bindValue(":part", "%" + partialLyrics + "%");
    query.exec();
    if (query.size() == 0) {
        qDebug() << "Query size is 0";
        return {};
    }
    else
        qDebug() << "Query succeeded with hits";

    QList<Track> results;

    while (query.next()) {
        QString artist = query.value(0).toString().trimmed();
        QString title = query.value(1).toString().trimmed();
        QString album = query.value(2).toString().trimmed();
        QString lyrics = query.value(3).toString().trimmed();

        results.append(Track(artist, title, album, lyrics));
    }

    /*
    for (const Track &track : results) {
        qDebug() << track.artist << "-" << track.title << "[" << track.album << "]" << ":" << track.lyrics.mid(0, 20);
    }
    */

    return results;
}

QString DataModel::lyricsForFile(const QString &path) {
    if (path.endsWith(".mp3")) {
        TagLib::MPEG::File mpf(QFile::encodeName(path).constData());
        if (!mpf.isOpen() || !mpf.isValid()) {
            qDebug() << "OPEN FAILED, continuing...";
            return {};
        }
        auto *id3v2tag = mpf.ID3v2Tag();
        if (!id3v2tag)
            return {};
    //        qDebug() << "Listing frames";
        TagLib::ID3v2::FrameList frames = id3v2tag->frameListMap()["USLT"];
        if (frames.isEmpty())
            return {};

        auto *frame = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(frames.front());
        if (!frame)
            return {};

        return frame->text().toCString(true);
    }
    else if (path.endsWith(".m4a")) {
        TagLib::MP4::File file(QFile::encodeName(path).constData());
        TagLib::MP4::Item item = file.tag()->itemListMap()["\xa9lyr"];
        TagLib::StringList strings = item.toStringList();
        if (!strings.isEmpty()) {
            return strings.front().toCString(true);
        }
    }
}

void DataModel::updateIndex() {
    QSqlQuery query("DELETE FROM data");
    id = 1;
    query.exec();
    qDebug() << "Updating index...";
    indexFilesRecursively("D:\\Music\\Music\\Dream Theater", 5);
    db.commit();
}
