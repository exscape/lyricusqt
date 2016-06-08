#include "UI/manualdownloaderwindow.h"

ManualDownloaderWindow::ManualDownloaderWindow() {
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
        emit fetchStarted(artistLineEdit->text(), titleLineEdit->text());
        lyricFetcher->fetchLyrics(artistLineEdit->text(), titleLineEdit->text(), [=](const QString &lyrics, FetchResult result) {
            if (result == FetchResult::Success)
                emit fetchComplete(lyrics, result);
            else if (result == FetchResult::NoMatch)
                emit fetchComplete(QString("No results for %1 - %2").arg(artistLineEdit->text(), titleLineEdit->text()), result);
            else if (result == FetchResult::ConnectionFailed || result == FetchResult::RequestFailed)
                emit fetchComplete("A connection error/site error occured.", result);
            else if (result == FetchResult::ParsingFailed)
                emit fetchComplete("A parse error occurred. This is probably because of a site change; this software needs to be updated.", result);
            else
                emit fetchComplete("An error occured.", result);
        });
    });

    searchButton->setEnabled(false);
    connect(artistLineEdit, &QLineEdit::textEdited, this, &ManualDownloaderWindow::validateFields);
    connect(titleLineEdit, &QLineEdit::textEdited, this, &ManualDownloaderWindow::validateFields);
}

void ManualDownloaderWindow::validateFields() {
    if (artistLineEdit->text().length() > 0 && titleLineEdit->text().length() > 0)
        searchButton->setEnabled(true);
    else
        searchButton->setEnabled(false);
}
