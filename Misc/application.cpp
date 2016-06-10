#include <QDir>
#include <QVector>
#include <QDebug>
#include <QKeySequence>
#include <QReadLocker>
#include <QWriteLocker>
#include <QDateTime>
#include <QSettings>
#include <QPair>
#include "UI/mainwindow.h"
#include "Misc/application.h"

QMap<QString, QVariant> Application::_settings;
QMap<QString, QVariant> Application::_defaultSettings;
QReadWriteLock Application::rwLock;

QDataStream &operator<<(QDataStream& stream, const Path &path) {
    stream << path.path << path.depth;
    return stream;
}

QDataStream &operator>>(QDataStream& stream, Path &path) {
    stream >> path.path >> path.depth;
    return stream;
}

QDataStream &operator<<(QDataStream& stream, const QVector<Path> &paths) {
    stream << paths.length();
    for (const Path &path : paths) {
        stream << path;
    }
    return stream;
}

QDataStream &operator>>(QDataStream& stream, QVector<Path> &paths) {
    int len = 0;
    stream >> len;
    while (len-- > 0) {
        Path path;
        stream >> path;
        paths.append(std::move(path));
    }
    return stream;
}

Application::Application(int &argc, char **argv) : QApplication(argc, argv) {
    // These settings makes for storage at ~/.config/lyricusqt/lyricusqt.conf on Linux.
    QCoreApplication::setOrganizationName("lyricusqt");
    QCoreApplication::setOrganizationDomain("exscape.org");
    QCoreApplication::setApplicationName("lyricusqt");

    // Allows storing Path objects in QSettings
    qRegisterMetaTypeStreamOperators<Path>("Path");
    qRegisterMetaTypeStreamOperators<QVector<Path>>("QVector<Path>");

    // Used for site priority data.
    // Note that there is no implementation of this supplied -- apparently, Qt takes care
    // of that. With this it works, without this it doesn't!
    qRegisterMetaTypeStreamOperators<QVector<QPair<bool, QString>>>("QVector<QPair<bool, QString>>");

    //
    // Set up the default settings
    //

    const QString home = QDir::homePath();
    QVector<Path> pathsToIndex = {
                                   { joinPath(home, "Music"), 10 },
                                 };
    _defaultSettings.insert("pathsToIndex", QVariant::fromValue(pathsToIndex));

    // Stored as a pair, <siteEnabled, siteName>
    // The vector order is the site priority order, so the default is Songmeanings first, AZLyrics second, and DarkLyrics disabled.
    _defaultSettings.insert("sitePriority", QVariant::fromValue(QVector<QPair<bool, QString>>({ { true, "Songmeanings" }, { true, "AZLyrics" }, { false, "DarkLyrics" } })));

    // Load settings from disk
    QSettings qs;
    _settings = qs.value("settings").toMap();
}

bool Application::hasSetting(const QString &key) {
    QReadLocker locker(&rwLock);
    return (_settings.contains(key) || _defaultSettings.contains(key));
}

QVariant Application::getSetting(const QString &key) {
    QReadLocker locker(&rwLock);

    if (_settings.contains(key))
        return _settings[key];
    else if (_defaultSettings.contains(key))
        return _defaultSettings[key];
    else {
        qWarning() << "invalid setting" << key << "requested in getSetting!";
        Q_ASSERT(false && "Invalid setting requested in getSetting"); // Only used in debug builds
        return {};
    }
}

void Application::setSetting(const QString &key, const QVariant &value) {
    QWriteLocker locker(&rwLock);
    _settings[key] = value;
}

void Application::resetSetting(const QString &key) {
    rwLock.lockForWrite();
    _settings.remove(key);
    rwLock.unlock();

    writeSettings();
}

void Application::resetAllSettings() {
    rwLock.lockForWrite();
    _settings.clear();
    rwLock.unlock();

    writeSettings();
}

void Application::writeSettings() {
    // Despite the name, this function doesn't write the *locked resource*, _settings, so
    // we want a read locker here
    QReadLocker locker(&rwLock);

    QSettings qs;
    qs.setValue("settings", _settings);
    qs.sync();
}
