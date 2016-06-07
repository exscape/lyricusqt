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

#include "application.h"

/*
#ifdef Q_OS_MAC
#include <objc/objc.h>
#include <objc/message.h>
void setupDockClickHandler();
bool dockClickHandler(id self,SEL _cmd,...);
#endif
*/

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

/*
#ifdef Q_OS_MAC
    setupDockClickHandler();
#endif
*/

    //
    // Set up the default settings
    //

    /*
    _defaultSettings.insert("activationHotkey", QKeySequence(Qt::ALT + Qt::Key_Space));

    _defaultSettings.insert("displayResultIcons", true);
    _defaultSettings.insert("displayActionIcons", true);
    _defaultSettings.insert("resultsIconSize", 28);
    _defaultSettings.insert("actionsIconSize", 28);
    */

    //
    // Files/folders page
    //
    const QString home = QDir::homePath();
    QVector<Path> pathsToIndex = {
                                   { joinPath(home, "Music"), 10 },
                                 };
    _defaultSettings.insert("pathsToIndex", QVariant::fromValue(pathsToIndex));

    // Stored as a pair, <siteEnabled, siteName>
    _defaultSettings.insert("sitePriority", QVariant::fromValue(QVector<QPair<bool, QString>>({ { true, "Songmeanings" }, { true, "AZLyrics" }, { false, "DarkLyrics" } })));

    /*
    // Use the default size, which is calculated based on screen resolution, if the user hasn't resized the window to override this
    _defaultSettings.insert("mainWindowSize", QSize(0, 0));
    */

    // Load settings from disk
    QSettings qs;
    _settings = qs.value("settings").toMap();
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

/*
#ifdef Q_OS_MAC
void setupDockClickHandler() {
    Class cls = objc_getClass("NSApplication");
    objc_object *appInst = objc_msgSend((id)cls, sel_registerName("sharedApplication"));

    if(appInst != NULL) {
        objc_object* delegate = objc_msgSend(appInst, sel_registerName("delegate"));
        Class delClass = (Class)objc_msgSend(delegate,  sel_registerName("class"));
        SEL shouldHandle = sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:");
        if (class_getInstanceMethod(delClass, shouldHandle)) {
            if (class_replaceMethod(delClass, shouldHandle, (IMP)dockClickHandler, "B@:"))
                qDebug() << "Registered dock click handler (replaced original method)";
            else
                qWarning() << "Failed to replace method for dock click handler";
        }
        else {
            if (class_addMethod(delClass, shouldHandle, (IMP)dockClickHandler,"B@:"))
                qDebug() << "Registered dock click handler";
            else
                qWarning() << "Failed to register dock click handler";
        }
    }
}

bool dockClickHandler(id self,SEL _cmd,...) {
    Q_UNUSED(self)
    Q_UNUSED(_cmd)
    static_cast<Application*>(qApp)->mainWindow->hotkeyPressed();
    return false;
}
#endif
*/
