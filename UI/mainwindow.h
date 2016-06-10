#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UI/reversesearchwindow.h"
#include "UI/lyricdownloaderwindow.h"
#include "UI/manualdownloaderwindow.h"
#include "UI/configdialog.h"
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QThread>
#ifdef Q_OS_WIN
#include "Misc/foobarnowplayingannouncer.h"
#endif

class ReverseSearchModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    ReverseSearchModel *reverseSearchModel = nullptr;
    ReverseSearchWindow *reverseSearchWindow = nullptr;
    LyricDownloaderWindow *lyricDownloaderWindow = nullptr;
    QPlainTextEdit *lyricsTextEdit = nullptr;
    ManualDownloaderWindow *manualDownloaderWindow = nullptr;
    ConfigDialog *configDialog = nullptr;
    QThread *foobarNowPlayingAnnouncerThread = nullptr;

#ifdef Q_OS_WIN
    FoobarNowPlayingAnnouncer *foobarNowPlayingAnnouncer = nullptr;
#endif

    QString fetchArtist;
    QString fetchTitle;

    // Set on "now playing" announcements
    QString mostRecentArtist;
    QString mostRecentTitle;

    QMenuBar *menuBar = nullptr;
    QMenu *fileMenu = nullptr;
    QMenu *editMenu = nullptr;
    QMenu *lyricsMenu = nullptr;

public:
    explicit MainWindow(QWidget *parent = 0);
    void adjustSize();
    ~MainWindow() {}
protected:
    void trackChanged(QString artist, QString title, QString path);
    void closeEvent(QCloseEvent *closeEvent) override;
};

#endif // MAINWINDOW_H
