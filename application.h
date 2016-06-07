#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QSettings>
#include <QMap>
#include <QReadWriteLock>

class MainWindow;

class Application : public QApplication
{
    Q_OBJECT

    static QMap<QString, QVariant> _settings;
    static QMap<QString, QVariant> _defaultSettings;
    static QReadWriteLock rwLock;

public:
    Application(int &argc, char **argv);
    MainWindow *mainWindow = nullptr;
    void setMainWindow(MainWindow *win) { this->mainWindow = win; }
    ~Application() { writeSettings(); }

    static QVariant getSetting(const QString &key);
    static void setSetting(const QString &key, const QVariant &value); // Note: doesn't store results to disk; call writeSettings() when you're done changing settings
    static void resetSetting(const QString &key);
    static void resetAllSettings();
    static void writeSettings();

//signals:
//public slots:
//protected slots:
//    void indexerTimerElapsed();
};

#endif // APPLICATION_H
