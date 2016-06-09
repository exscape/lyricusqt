#ifndef SHARED_H
#define SHARED_H

#include <QString>
#include <QRegularExpression>
#include <QVector>

enum class FetchResult {
    Success = 0,
    InvalidRequest,
    ConnectionFailed,
    RequestFailed,
    ParsingFailed,
    NoMatch
};

Q_DECLARE_METATYPE(FetchResult)

enum Roles {
    LyricStatusRole = Qt::UserRole,   // Stores the shared_ptr<IndexedObject> represented by a list item
    LyricsRole = Qt::UserRole + 1,    // Stores lyrics to a track
    PathRole = Qt::UserRole + 2,      // Stores an unmodified path, in cases where the DisplayRole might differ
};

// Represents a file path to index. An array of these describes the list of locations to search for files and folders.
struct Path
{
    Path() {}
    Path(const QString &path, int depth) : path(path), depth(depth) {}
    QString path;
    int depth;
    ~Path() {}
};

// Defined in application.cpp
QDataStream &operator<<(QDataStream& stream, const Path &path);
QDataStream &operator>>(QDataStream& stream, Path &path);
QDataStream &operator<<(QDataStream& stream, const QVector<Path> &paths);
QDataStream &operator>>(QDataStream& stream, QVector<Path> &paths);

Q_DECLARE_METATYPE(Path)
Q_DECLARE_METATYPE(QVector<Path>)

inline QString joinPath() { return QStringLiteral(""); }

template<typename... Types>
QString joinPath(QString part, Types... rest) {
    // QDir::separator() can be used for a platform-specific separator character, but Qt paths
    // use forward slashes even on Windows. Since using a mix is ugly, I decided to use forward
    // slashes internally for all OS:es, and then (for Windows only) change them to backslashes
    // on display,instead.
    return part + "/" + joinPath(rest...);
}

// Defined in shared.cpp
QString simplifiedRepresentation(const QString &source);
QString lyricsForFile(const QString &path);
bool setLyricsForFile(const QString &path, const QString &lyrics);
QPair<QString, QString> artistAndTitleForFile(const QString &path);

#endif // SHARED_H
