#include "reversesearchwindow.h"
#include "datamodel.h"
#include "track.h"
#include <QHeaderView>
#include <QDebug>
#include <QScrollbar>
#include <QPalette>

enum CustomRoles {
    LyricsRole = Qt::UserRole + 1,
};

void ReverseSearchWindow::searchStringUpdated(QString newString) {
    if (newString.length() <= 2)
        return;

    QList<Track> data = dataModel->tracksMatchingLyrics(newString);
    qDebug() << data.length() << "tracks matched";

    results->clear();

    QList<QTreeWidgetItem*> items;
    for (const Track &track : data) {
        auto *item = new QTreeWidgetItem({ track.artist/*, track.album*/, track.title });
//        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(0, LyricsRole, track.lyrics);
        items.append(item);
    }

    results->addTopLevelItems(items);
}

ReverseSearchWindow::ReverseSearchWindow(DataModel *model, QWidget *parent) : QMainWindow(parent) {
    this->dataModel = model;

    QWidget *window = new QWidget;
    vbox = new QVBoxLayout;
    indexButton = new QPushButton("Update index");
    connect(indexButton, &QPushButton::clicked, [&] (bool checked) {
        Q_UNUSED(checked);
        dataModel->updateIndex();
    });

    searchString = new QLineEdit;
    searchString->setPlaceholderText("Enter search string...");

    results = new QTreeWidget;
    lyricDisplay = new QPlainTextEdit;

    vbox->addWidget(indexButton);
    vbox->addWidget(searchString);
    vbox->addWidget(results);
    vbox->addWidget(lyricDisplay);

    vbox->setStretch(2, 2);
    vbox->setStretch(3, 5);

    window->setLayout(vbox);
    setCentralWidget(window);

    results->setIndentation(0);
    results->setColumnCount(2);
    results->setHeaderLabels({ "Artist"/*, "Album"*/, "Title" });
    results->header()->setStretchLastSection(true);
    results->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    results->header()->setMinimumSectionSize(75);
    results->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    results->header()->setStretchLastSection(true);
//    results->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
//    results->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    connect(results, &QTreeWidget::currentItemChanged, [&](QTreeWidgetItem *current, QTreeWidgetItem *previous) {
        Q_UNUSED(previous);
        if (!current)
            return;
        lyricDisplay->setPlainText(current->data(0, LyricsRole).toString());

        // Highlight the search string
        lyricDisplay->setFocus();
        lyricDisplay->find(searchString->text());

        lyricDisplay->centerCursor();
    });

    QPalette pal = lyricDisplay->palette();
    pal.setColor(QPalette::Highlight, QColor(200, 0, 0, 200));
    pal.setColor(QPalette::HighlightedText, QColor(Qt::white));
    lyricDisplay->setPalette(pal);

    lyricDisplay->setReadOnly(true);

    connect(searchString, &QLineEdit::textEdited, this, &ReverseSearchWindow::searchStringUpdated);

    resize(600, 700);
    setWindowTitle("Lyricus - Reverse Lyric Search");
}
