#include "UI/manualdownloaderwindow.h"

ManualDownloaderWindow::ManualDownloaderWindow(QWidget *parent) : QDialog(parent) {
    lyricFetcher = new LyricFetcher;
    gridLayout = new QGridLayout;
    artistLabel = new QLabel("Artist:");
    titleLabel = new QLabel("Title:");
    artistLineEdit = new QLineEdit;
    titleLineEdit = new QLineEdit;
    searchButton = new QPushButton("Search");

    // Remove the "?" button next to the close button
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setLayout(gridLayout);

    resize(QSize(250, 90));

    gridLayout->addWidget(artistLabel, 0, 0, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(artistLineEdit, 0, 1, 1, 1);

    gridLayout->addWidget(titleLabel, 1, 0, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(titleLineEdit, 1, 1, 1, 1);
    gridLayout->addWidget(searchButton, 2, 1, 1, 1);

    connect(searchButton, &QPushButton::clicked, [=] {
        fetchLyrics(artistLineEdit->text(), titleLineEdit->text());
    });

    searchButton->setEnabled(false);
    connect(artistLineEdit, &QLineEdit::textChanged, this, &ManualDownloaderWindow::validateFields);
    connect(titleLineEdit, &QLineEdit::textChanged, this, &ManualDownloaderWindow::validateFields);


    setWindowTitle("Lyricus - Manual Download");
}

void ManualDownloaderWindow::setArtistAndTitle(QString artist, QString title) {
    artistLineEdit->setText(artist);
    titleLineEdit->setText(title);

    artistLineEdit->selectAll();
    artistLineEdit->setFocus();
}

void ManualDownloaderWindow::fetchLyrics(QString artist, QString title) {
    if (artist.length() == 0 || title.length() == 0)
        return;

    emit fetchStarted(artist, title);

    lyricFetcher->fetchLyrics(artist, title, [=](const QString &lyrics, FetchResult result) {
        if (result == FetchResult::Success)
            emit fetchComplete(artist, title, lyrics, result);
        else if (result == FetchResult::NoMatch)
            emit fetchComplete(artist, title, QString("No results for %1 - %2").arg(artist, title), result);
        else if (result == FetchResult::ConnectionFailed || result == FetchResult::RequestFailed)
            emit fetchComplete(artist, title, "A connection error/site error occured.", result);
        else if (result == FetchResult::ParsingFailed)
            emit fetchComplete(artist, title, "A parse error occurred. This is probably because of a site change; this software needs to be updated.", result);
        else
            emit fetchComplete(artist, title, "An error occured.", result);
    });

}

void ManualDownloaderWindow::validateFields() {
    if (artistLineEdit->text().length() > 0 && titleLineEdit->text().length() > 0)
        searchButton->setEnabled(true);
    else
        searchButton->setEnabled(false);
}
