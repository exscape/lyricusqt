#include "UI/mainwindow.h"
#include <QDesktopWidget>
#include <QApplication>
#include "reversesearchmodel.h"

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

    lyricsMenu->addAction("&Manual download", this, [&] {
        if (manualDownloaderWindow == nullptr)
            manualDownloaderWindow = new ManualDownloaderWindow;
        manualDownloaderWindow->setModal(true);

        connect(manualDownloaderWindow, &ManualDownloaderWindow::fetchStarted, [this](QString artist, QString title) {
            lyricsTextEdit->setPlainText(QString("Attempting to fetch lyrics for %1 - %2, please wait...").arg(artist, title));
            manualDownloaderWindow->close();
        });

        connect(manualDownloaderWindow, &ManualDownloaderWindow::fetchComplete, [this](QString lyrics, FetchResult result) {
            // No need to check for errors, as "lyrics" contains an error message in that case -- which we want to display.
            lyricsTextEdit->setPlainText(lyrics);
            Q_UNUSED(result);
        });

        manualDownloaderWindow->exec();
    });

    lyricsMenu->addAction("&Lyric downloader", this, [&] {
        if (lyricDownloaderWindow == nullptr)
            lyricDownloaderWindow = new LyricDownloaderWindow(this);
        lyricDownloaderWindow->show();
        lyricDownloaderWindow->setFocus();
    });
    lyricsMenu->addAction("&Reverse lyric search", this, [&] {
        if (reverseSearchWindow == nullptr) {
            reverseSearchWindow = new ReverseSearchWindow(reverseSearchModel, this);
            reverseSearchModel = new ReverseSearchModel;
        }
        reverseSearchWindow->show();
        reverseSearchWindow->setFocus();
    });

    lyricsTextEdit = new QPlainTextEdit;
    setCentralWidget(lyricsTextEdit);
    lyricsTextEdit->setReadOnly(true);

    resize(300, 500);

    QDesktopWidget dw;
    move(dw.width() / 2 - width()/2, dw.height() / 2 - height()/2);
    setWindowTitle("Lyricus");
}
