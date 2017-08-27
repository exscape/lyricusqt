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
    QString mostRecentTrackPath;

    // Used when reverting, and to figure out if the lyrics have been edited or not
    QString originalLyrics;

    QMenuBar *menuBar = nullptr;
    QMenu *fileMenu = nullptr;
    QMenu *editMenu = nullptr;
    QMenu *lyricsMenu = nullptr;

    QColor defaultBackgroundColor;

public:
    explicit MainWindow(QWidget *parent = 0);
    void adjustSize();
    void enterEditMode();
    void exitEditMode();
    ~MainWindow() {}
protected:
    void trackChanged(QString artist, QString title, QString path);
    void closeEvent(QCloseEvent *closeEvent) override;
    void save();
};

#endif // MAINWINDOW_H
