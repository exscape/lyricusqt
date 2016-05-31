#include "UI/lyricdownloaderwindow.h"
#include <QMessageBox>

LyricDownloaderWindow::LyricDownloaderWindow(DataModel *model, QWidget *parent) : QMainWindow(parent), dataModel(model)
{
    lyricFetcher = new LyricFetcher;
    gridLayout = new QGridLayout;
    artistLabel = new QLabel("Artist:");
    titleLabel = new QLabel("Title:");
    artistLineEdit = new QLineEdit;
    titleLineEdit = new QLineEdit;
    searchButton = new QPushButton("Search");
    lyricsTextEdit = new QPlainTextEdit;
    
    QWidget *central = new QWidget;
    central->setLayout(gridLayout);
    setCentralWidget(central);

    resize(QSize(580, 400));

    gridLayout->addWidget(artistLabel, 0, 0, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(artistLineEdit, 0, 1, 1, 1);

    gridLayout->addWidget(titleLabel, 1, 0, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(titleLineEdit, 1, 1, 1, 1);
    gridLayout->addWidget(searchButton, 2, 1, 1, 1);
    gridLayout->addWidget(lyricsTextEdit, 3, 1, 10, 1);

    connect(searchButton, &QPushButton::clicked, [&] {
        lyricFetcher->fetchLyrics(artistLineEdit->text(), titleLineEdit->text(), [&](const QString &lyrics, FetchResult result) {
            if (result == FetchResult::Success)
                lyricsTextEdit->setPlainText(lyrics);
            else if (result == FetchResult::NoMatch)
                lyricsTextEdit->setPlainText(QString("No results for %1 - %2").arg(artistLineEdit->text(), titleLineEdit->text()));
            else
                lyricsTextEdit->setPlainText("An error occured!");
        });
    });

    artistLineEdit->setText("Dream Theater");
    titleLineEdit->setText("Octavarium");
}
