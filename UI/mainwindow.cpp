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

    manualDownloaderWindow = new ManualDownloaderWindow(this);
    manualDownloaderWindow->setModal(true);

    connect(manualDownloaderWindow, &ManualDownloaderWindow::fetchStarted, [this](QString artist, QString title) {
        fetchArtist = artist;
        fetchTitle = title;
 //     qDebug() << "fetchStarted";
        lyricsTextEdit->setPlainText(QString("Attempting to fetch lyrics for %1 - %2, please wait...").arg(artist, title));
        setWindowTitle(QString("%1 - %2").arg(artist, title));
        manualDownloaderWindow->close();
    });

    connect(manualDownloaderWindow, &ManualDownloaderWindow::fetchComplete, [this](QString artist, QString title, QString lyrics, FetchResult result) {
        // No need to check for errors, as "lyrics" contains an error message in that case -- which we want to display.
//      qDebug() << "fetchComplete w/ lyrics: " << lyrics.left(40) << "...";
        if (artist == fetchArtist && title == fetchTitle)
            lyricsTextEdit->setPlainText(lyrics);
        else
            qDebug() << "Ignoring result for old request" << artist << "-" << title;
        if (lyrics.length() == 0 || result != FetchResult::Success)
            setWindowTitle("Lyricus");
    });

    lyricsMenu->addAction("&Manual download", this, [&] {
        manualDownloaderWindow->setArtistAndTitle(mostRecentArtist, mostRecentTitle);
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

        // This is a bit of a hack, and should really be done in ReverseSearchWindow, when the window opens.
        // I'm not sure how to do that, though; showEvent isn't good enough, as that runs too often, e.g.
        // when the window is un-minimized. It should only run when the window is *opened*, so this will have
        // to do, for now.
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

    setWindowTitle("Lyricus");
}

void MainWindow::trackChanged(QString artist, QString title, QString path) {
    // Called when the currently playing track changes, and we need to update the display

    mostRecentArtist = artist;
    mostRecentTitle = title;

    QString lyrics = lyricsForFile(path);
    if (lyrics.length() > 0) {
        lyricsTextEdit->setPlainText(lyrics);
        setWindowTitle(QString("%1 - %2").arg(artist, title));

        // Set so that any previous fetch that still hasn't finished won't overwrite the lyrics when it DOES finish.
        // Since these don't match the artist/title of that previous fetch, the result will be thrown away, leaving
        // this one be.
        fetchArtist = artist;
        fetchTitle = title;

        return;
    }
    setWindowTitle("Lyricus");
    manualDownloaderWindow->fetchLyrics(artist, title);
}

void MainWindow::closeEvent(QCloseEvent *closeEvent) {
    Application::setSetting("mainWindowGeometry", saveGeometry());
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

void MainWindow::adjustSize() {
    if (Application::hasSetting("mainWindowGeometry"))
        restoreGeometry(Application::getSetting("mainWindowGeometry").toByteArray());
    else {
        // Use a default value, centered on the screen
        resize(300, 500);
        QRect geom = Application::desktop()->availableGeometry();
        setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), geom));

        /*
        // Use a default value in the bottom-right corner
        // I can't get this to work on Windows 10. geometry() is the inner window, so using that
        // causes the window to be too far right and down.
        // frameGeometry() is SUPPOSED to be the outer window as far as I understand, but the frame it returns
        // is bigger than the visible edges of the window, so that leaves a gap between the taskbar/window and right edge/window.
        // Ugh!
        QRect geom = Application::desktop()->availableGeometry();
        QPoint p;
        qDebug() << "Available geometry is" << geom;
        qDebug() << "Window geometry is" << geometry();
        qDebug() << "Frame geometry is" << frameGeometry();

        int windowWidth = frameGeometry().width();
        int windowHeight = frameGeometry().height();

        p.setX(geom.width() - windowWidth);
        p.setY(geom.height() - windowHeight);

        move(p);
        */
    }
}
