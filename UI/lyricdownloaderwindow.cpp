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
    
    QWidget *central = new QWidget;
    central->setLayout(gridLayout);
    setCentralWidget(central);

    resize(QSize(280, 90));
    
    gridLayout->addWidget(artistLabel, 0, 0, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(artistLineEdit, 0, 1, 1, 1);
    gridLayout->addWidget(titleLabel, 1, 0, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(titleLineEdit, 1, 1, 1, 1);
    gridLayout->addWidget(searchButton, 2, 0, 1, 2);

    connect(searchButton, &QPushButton::clicked, [&] {
        lyricFetcher->fetchLyrics(artistLineEdit->text(), titleLineEdit->text(), [&](const QString &lyrics, FetchResult result) {
            if (result == FetchResult::Success)
                QMessageBox::information(this, artistLineEdit->text() + " - " + titleLineEdit->text(), lyrics, QMessageBox::Ok);
            else
                QMessageBox::critical(this, artistLineEdit->text() + " - " + titleLineEdit->text(), "Unable to fetch lyrics!", QMessageBox::Ok);
        });
    });

    artistLineEdit->setText("Dream Theater");
    titleLineEdit->setText("Octavarium");
}
