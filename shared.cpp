#include "shared.h"

QString simplifiedRepresentation(const QString &source) {
    return source.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");
}
