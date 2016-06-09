#include "Misc/Application.h"
#include "UI/mainwindow.h"
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    qRegisterMetaType<FetchResult>("FetchResult");

    // Create a new menu bar, with a null parent -- in order to have
    // the same menu bar for all windows on OS X
    menuBar = new QMenuBar(nullptr);
    fileMenu = new QMenu("&File", menuBar);
    editMenu = new QMenu("&Edit", menuBar);
    lyricsMenu = new QMenu("&Lyrics", menuBar);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(lyricsMenu);
    this->setMenuBar(menuBar);

    fileMenu->addAction("&Preferences", this, [&] {
        if (configDialog == nullptr) {
            configDialog = new ConfigDialog(this);
            configDialog->setModal(true);
        }

        configDialog->loadSettings();
        configDialog->exec();
    });
    fileMenu->addAction("&Save", this, [&] { qDebug() << "Save"; }, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, [&] { QApplication::quit(); }, QKeySequence::Quit);

    manualDownloaderWindow = new ManualDownloaderWindow;
    manualDownloaderWindow->setModal(true);

    connect(manualDownloaderWindow, &ManualDownloaderWindow::fetchStarted, [this](QString artist, QString title) {
        qDebug() << "fetchStarted";
        lyricsTextEdit->setPlainText(QString("Attempting to fetch lyrics for %1 - %2, please wait...").arg(artist, title));
        manualDownloaderWindow->close();
    });

    connect(manualDownloaderWindow, &ManualDownloaderWindow::fetchComplete, [this](QString lyrics, FetchResult result) {
        // No need to check for errors, as "lyrics" contains an error message in that case -- which we want to display.
        qDebug() << "fetchComplete w/ lyrics: " << lyrics.left(40) << "...";
        lyricsTextEdit->setPlainText(lyrics);
        Q_UNUSED(result);
    });

    lyricsMenu->addAction("&Manual download", this, [&] {
        manualDownloaderWindow->exec();
    });

    lyricsMenu->addAction("&Lyric downloader", this, [&] {
        if (lyricDownloaderWindow == nullptr)
            lyricDownloaderWindow = new LyricDownloaderWindow;
        lyricDownloaderWindow->show();
        lyricDownloaderWindow->setFocus();
    });
    lyricsMenu->addAction("&Reverse lyric search", this, [&] {
        if (reverseSearchWindow == nullptr) {
            reverseSearchWindow = new ReverseSearchWindow;
        }
        reverseSearchWindow->show();
        reverseSearchWindow->setFocus();
        reverseSearchWindow->checkIndex();
    });

    lyricsTextEdit = new QPlainTextEdit;
    setCentralWidget(lyricsTextEdit);
    lyricsTextEdit->setReadOnly(true);

    foobarNowPlayingAnnouncer = new FoobarNowPlayingAnnouncer;
    foobarNowPlayingAnnouncerThread = new QThread;
    foobarNowPlayingAnnouncer->moveToThread(foobarNowPlayingAnnouncerThread);
    connect(foobarNowPlayingAnnouncerThread, &QThread::started, foobarNowPlayingAnnouncer, &FoobarNowPlayingAnnouncer::run);
    connect(foobarNowPlayingAnnouncer, &FoobarNowPlayingAnnouncer::newTrack, this, &MainWindow::trackChanged);

    foobarNowPlayingAnnouncerThread->start();

    resize(300, 500);

    QDesktopWidget dw;
    move(dw.width() / 2 - width()/2, dw.height() / 2 - height()/2);
    setWindowTitle("Lyricus");
}

void MainWindow::trackChanged(QString artist, QString title, QString path) {
    // Called when the currently playing track changes, and we need to update the display
    qDebug() << "trackChanged handler called with" << artist << "-" << title << "at" << path;

//    QString lyrics = lyricsForFile(path);
    QString lyrics;
    if (lyrics.length() > 0) {
        lyricsTextEdit->setPlainText(lyrics);
        setWindowTitle(QString("%1 - %2").arg(artist, title));

        return;
    }
    manualDownloaderWindow->fetchLyrics(artist, title);
    setWindowTitle("Lyricus");
}

void MainWindow::closeEvent(QCloseEvent *closeEvent) {
    bool shouldExit = true;

    // Don't exit if the downloader is running -- unless the user says "yes" (in which case close() returns true).
    if (lyricDownloaderWindow) {
        if (!lyricDownloaderWindow->close())
            shouldExit = false;
    }

    if (shouldExit)
        Application::quit();
    else
        closeEvent->ignore(); // Otherwise the main window will close, but the others will live on
}
