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

QString simplifiedRepresentation(const QString &source);

#endif // SHARED_H
