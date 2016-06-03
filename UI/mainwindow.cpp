#include "UI/mainwindow.h"
#include <QDesktopWidget>
#include "reversesearchmodel.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    reverseSearchModel = new ReverseSearchModel;
    reverseSearchButton = new QPushButton("Reverse lyric search");
    lyricDownloaderButton = new QPushButton("Lyric downloader");
    hbox = new QHBoxLayout;

    QDesktopWidget dw;

    connect(reverseSearchButton, &QPushButton::clicked, [&]  {
        if (reverseSearchWindow == nullptr)
            reverseSearchWindow = new ReverseSearchWindow(reverseSearchModel, this);
        reverseSearchWindow->show();
        reverseSearchWindow->setFocus();
    });

    connect(lyricDownloaderButton, &QPushButton::clicked, [&]  {
        if (lyricDownloaderWindow == nullptr)
            lyricDownloaderWindow = new LyricDownloaderWindow(reverseSearchModel, this);
        lyricDownloaderWindow->show();
        lyricDownloaderWindow->setFocus();
    });

    hbox->addWidget(reverseSearchButton);
    hbox->addWidget(lyricDownloaderButton);

    QWidget *central = new QWidget;
    central->setLayout(hbox);
    setCentralWidget(central);

    move(dw.width() / 2 - width()/2, dw.height() / 2 - height()/2);
    setWindowTitle("Lyricus");
}
