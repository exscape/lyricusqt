#include "Misc/application.h"
#include "UI/mainwindow.h"
#include <QDesktopWidget>
#include <QMessageBox>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <QTimer>
#endif

void MainWindow::enterEditMode() {
    qDebug() << "Enter edit mode";
    editModeAction->setText("Exit &edit mode");
    lyricsTextEdit->setReadOnly(false);

    auto palette = lyricsTextEdit->palette();
    palette.setColor(QPalette::Base, QColor(255, 220, 220));
    defaultBackgroundColor = palette.color(QPalette::Base);
    lyricsTextEdit->setPalette(palette);
}

void MainWindow::exitEditMode() {
    qDebug() << "Exit edit mode";
    editModeAction->setText("Enter &edit mode");
    lyricsTextEdit->setReadOnly(true);

    auto palette = lyricsTextEdit->palette();
    palette.setColor(QPalette::Base, Qt::white);
    lyricsTextEdit->setPalette(palette);
}

void MainWindow::save() {
    qDebug() << "Save";

    if (mostRecentTrackPath.length() > 0) {
        exitEditMode();
        QString lyricsToSave = lyricsTextEdit->toPlainText();

        if (lyricsToSave == originalLyrics) {
            qDebug() << "Lyrics were unchanged; ignoring save request";
            return;
        }
        else
            originalLyrics = lyricsToSave; // This is required to avoid a bug: editing, saving, reverting and saving again will ignore the revert.

        // On Windows, we can't write to the file if it's open in a music player.
        // Wait until it's not. There's absolutely a possible race condition here, but I'm not sure how to take care of that,
        // without modifying TagLib.
        //
        // Because we need to avoid starting multiple timers for a single path (in case the users edits, saves, then edits and saves again before the first save is written to disk),
        // but still need to support multiple timers (one for each file that we're still trying to write to disk), we use a map structure (specifically QHash) to store the mappings
        // between a file path and its QTimer.
        // If a file path has no entry in the map, it's not currently being saved.
        // If the map is empty, *no* file is currently being saved.

#ifdef Q_OS_WIN
        QTimer *saveTimer = saveTimers.value(mostRecentTrackPath, nullptr);
        if (saveTimer == nullptr) {
            saveTimer = new QTimer;
            saveTimer->setTimerType(Qt::VeryCoarseTimer);
            saveTimers[mostRecentTrackPath] = saveTimer;
        }
        else {
            qDebug() << "Stopping saveTimer" << saveTimer << "for path" << mostRecentTrackPath << "and restarting with updated lyrics";
            saveTimer->stop();
            saveTimer->disconnect();
        }

        // x = x copies the variable, as C++11 lambdas can't capture class members by value ordinarily.
        connect(saveTimer, QTimer::timeout, [this, saveTimer, mostRecentTrackPath = mostRecentTrackPath, lyricsToSave] {
            saveTimer->setInterval(5000);
            qDebug() << "In timer" << saveTimer << ", mostRecent... =" << mostRecentTrackPath;

            HANDLE ret = CreateFile(mostRecentTrackPath.toStdWString().c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (ret == INVALID_HANDLE_VALUE && GetLastError() == ERROR_SHARING_VIOLATION) {
                // File is still in use, try again on the next timer timeout
                qDebug() << "Sharing violation on" << mostRecentTrackPath << "-- waiting and trying again...";
                return;
            }
            else if (ret == INVALID_HANDLE_VALUE && GetLastError() != ERROR_SHARING_VIOLATION) {
                // An error occured, abort and don't try again.
                QMessageBox::warning(this, "Unable to save", QString("Unable to write lyrics to file %1.").arg(mostRecentTrackPath), QMessageBox::Ok);
            }
            else if (ret != INVALID_HANDLE_VALUE) {
                // Open successful, we can save now!
                CloseHandle(ret);
                qDebug() << "Open successful, attempting save...";
                if (!setLyricsForFile(mostRecentTrackPath, lyricsToSave)) {
                    QMessageBox::warning(this, "Unable to save", QString("Unable to write lyrics to file %1.").arg(mostRecentTrackPath), QMessageBox::Ok);
                }
                else {
                    qDebug() << "Save to " << mostRecentTrackPath << "successful!";
                }
            }

            disconnect(saveTimer);
            saveTimer->deleteLater();
            saveTimers.remove(mostRecentTrackPath);
            qDebug() << "Disconnected and removed saveTimer" << saveTimer << "for path" << mostRecentTrackPath;
        });

        saveTimer->setInterval(0);
        saveTimer->start();
#else
        if (!setLyricsForFile(mostRecentTrackPath, lyricsToSave)) {
            QMessageBox::warning(this, "Unable to save", QString("Unable to write lyrics to file %1.").arg(mostRecentTrackPath), QMessageBox::Ok);
        }
        else {
            qDebug() << "Save to " << mostRecentTrackPath << "successful!";
        }
#endif
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    qRegisterMetaType<FetchResult>("FetchResult");

    // Create a new menu bar, with a null parent -- in order to have
    // the same menu bar for all windows on OS X
    menuBar = new QMenuBar(nullptr);
    fileMenu = new QMenu("&File", menuBar);
    editMenu = new QMenu("&Edit", menuBar);
    lyricsMenu = new QMenu("&Lyrics", menuBar);
    displayMenu = new QMenu("&Display", menuBar);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(lyricsMenu);
    menuBar->addMenu(displayMenu);
    this->setMenuBar(menuBar);

    fileMenu->addAction("&Preferences", this, [&] {
        if (configDialog == nullptr) {
            configDialog = new ConfigDialog(this);
            configDialog->setModal(true);
        }

        configDialog->loadSettings();
        configDialog->exec();
    });
    saveAction = fileMenu->addAction("&Save", this, &MainWindow::save, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, [&] { QApplication::quit(); }, QKeySequence::Quit);

    editModeAction = editMenu->addAction("Enter &edit mode", this, [&] {
        if (lyricsTextEdit->isReadOnly()) {
            enterEditMode();
        }
        else {
            exitEditMode();
        }
    }, Qt::CTRL + Qt::Key_E);
    editModeAction->setEnabled(false); // Enabled when a local track (with a known path) is played; disabled again on manual search

    manualDownloaderWindow = new ManualDownloaderWindow(this);
    manualDownloaderWindow->setModal(true);

    connect(manualDownloaderWindow, &ManualDownloaderWindow::fetchStarted, [this](QString artist, QString title) {
        fetchArtist = artist;
        fetchTitle = title;

        exitEditMode();
        mostRecentArtist.clear();
        mostRecentTitle.clear();
        mostRecentTrackPath.clear();
 //     qDebug() << "fetchStarted";
        lyricsTextEdit->setPlainText(QString("Attempting to fetch lyrics for %1 - %2, please wait...").arg(artist, title));
        setWindowTitle(QString("%1 - %2").arg(artist, title));
        manualDownloaderWindow->close();

        saveAction->setEnabled(false);
        editModeAction->setEnabled(false);
    });

    connect(manualDownloaderWindow, &ManualDownloaderWindow::fetchComplete, [this](QString artist, QString title, QString lyrics, FetchResult result) {
        // No need to check for errors, as "lyrics" contains an error message in that case -- which we want to display.
//      qDebug() << "fetchComplete w/ lyrics: " << lyrics.left(40) << "...";
        if (artist == fetchArtist && title == fetchTitle) {
            lyricsTextEdit->setPlainText(lyrics);
            originalLyrics = lyrics;
        }
        else
            qDebug() << "Ignoring result for old request" << artist << "-" << title;
        if (lyrics.length() == 0 || result != FetchResult::Success)
            setWindowTitle("Lyricus");
    });

    lyricsMenu->addAction("&Manual download", this, [&] {
        manualDownloaderWindow->setArtistAndTitle(mostRecentArtist, mostRecentTitle);
        manualDownloaderWindow->exec();
    }, QKeySequence::New);

    alwaysOnTopAction = displayMenu->addAction("&Always on top", this, [&] {
        auto flags = windowFlags();

        bool alwaysOnTop = (flags & Qt::WindowStaysOnTopHint) != 0;
        if (alwaysOnTop) {
            setWindowFlags (flags & ~Qt::WindowStaysOnTopHint);
        }
        else
            setWindowFlags(flags | Qt::WindowStaysOnTopHint);

        alwaysOnTopAction->setChecked(!alwaysOnTop);
        show();
    });

    alwaysOnTopAction->setCheckable(true);

    goToCurrentAction = lyricsMenu->addAction("&Go to current track", this, [&] {
#ifdef Q_OS_WIN
        if (foobarNowPlayingAnnouncer != nullptr)
            foobarNowPlayingAnnouncer->reEmitLastTrack();
#endif
    });
    goToCurrentAction->setEnabled(false);

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

#ifdef Q_OS_WIN
    foobarNowPlayingAnnouncer = new FoobarNowPlayingAnnouncer;
    foobarNowPlayingAnnouncerThread = new QThread;
    foobarNowPlayingAnnouncer->moveToThread(foobarNowPlayingAnnouncerThread);
    connect(foobarNowPlayingAnnouncerThread, &QThread::started, foobarNowPlayingAnnouncer, &FoobarNowPlayingAnnouncer::run);
    connect(foobarNowPlayingAnnouncer, &FoobarNowPlayingAnnouncer::newTrack, this, &MainWindow::trackChanged);

    foobarNowPlayingAnnouncerThread->start();
#endif

    setWindowTitle("Lyricus");
}

void MainWindow::trackChanged(QString artist, QString title, QString path) {
    // Called when the currently playing track changes, and we need to update the display

    mostRecentArtist = artist;
    mostRecentTitle = title;
    mostRecentTrackPath = path;

    saveAction->setEnabled( mostRecentTrackPath.length() > 0 );
    editModeAction->setEnabled( mostRecentTrackPath.length() > 0 );
    goToCurrentAction->setEnabled( mostRecentTrackPath.length() > 0 );

    QString lyrics = lyricsForFile(path);
    if (lyrics.length() > 0) {
        exitEditMode();
        lyricsTextEdit->setPlainText(lyrics);
        originalLyrics = lyrics;
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
#ifdef Q_OS_WIN
    // Ask the user if we should ignore a pending save.
    if (shouldExit && saveTimers.size() != 0) {
        QString paths;
        for (const QString &path : saveTimers.keys()) {
            paths.append(path + "\n");
        }
        paths.chop(1);
        if (QMessageBox::information(this, "Discard lyric edits and exit?", "One or more lyric edits is not yet saved, as the files are in use.\nDo you want to discard your edits and exit anyway?\n\n"
                                     "Affected file(s):\n" + paths, "Don't exit", "Discard and exit", QString(), 0, 0) == 0) {
            shouldExit = false;
        }
    }
#endif

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
