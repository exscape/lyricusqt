#ifndef SHARED_H
#define SHARED_H

#include <QString>
#include <QRegularExpression>

enum class FetchResult {
    Success = 0,
    InvalidRequest,
    ConnectionFailed,
    RequestFailed,
    ParsingFailed,
    NoMatch
};

Q_DECLARE_METATYPE(FetchResult)

QString simplifiedRepresentation(const QString &source);
QString lyricsForFile(const QString &path);
bool setLyricsForFile(const QString &path, const QString &lyrics);
QPair<QString, QString> artistAndTitleForFile(const QString &path);

#endif // SHARED_H
