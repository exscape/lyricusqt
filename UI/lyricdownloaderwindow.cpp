#include "UI/lyricdownloaderwindow.h"
#include <QMessageBox>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>

LyricDownloaderWindow::LyricDownloaderWindow(QWidget *parent) : QMainWindow(parent) {
    fileList = new QTreeWidget;
    fileList->setIndentation(0);
    fileList->setColumnCount(2);
    fileList->setHeaderLabels({ "Lyrics?", "File" });
    fileList->header()->setStretchLastSection(true);
    fileList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    fileList->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    fileList->header()->setStretchLastSection(true);

    fileList->addTopLevelItem(new QTreeWidgetItem(QStringList({"No", "C:\\Music\\Test.mp3"})));
    fileList->model()->setHeaderData(0, Qt::Horizontal, Qt::AlignCenter, Qt::TextAlignmentRole);

    vbox = new QVBoxLayout;
    bottomHbox = new QHBoxLayout;

    QWidget *central = new QWidget;
    central->setLayout(vbox);
    vbox->addWidget(fileList);
    vbox->addLayout(bottomHbox);
    setCentralWidget(central);

    addFolderButton = new QPushButton("Add folder...");
    addFilesButton = new QPushButton("Add file(s)...");
    startDownloadButton = new QPushButton("Start download");
    startDownloadButton->setEnabled(false);

    bottomHbox->addStretch();
    bottomHbox->addWidget(addFolderButton);
    bottomHbox->addWidget(addFilesButton);
    bottomHbox->addWidget(startDownloadButton);

    connect(addFolderButton, &QPushButton::clicked, [&] {
        QFileDialog fileDialog;
        fileDialog.setFileMode(QFileDialog::Directory);
        fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
        if (fileDialog.exec()) {
            // Only one directory is supposed to be selectable, but just in case...
            for (const QString &dir : fileDialog.selectedFiles()) {
                addFilesRecursively(dir, 15);
            }
        }
    });

    connect(addFilesButton, &QPushButton::clicked, [&] {
        QFileDialog fileDialog;
        fileDialog.setNameFilter("Supported files (*.mp3 *.m4a);;MP3 files (*.mp3);;M4A files (*.m4a)");
        fileDialog.setFileMode(QFileDialog::ExistingFiles);
        if (fileDialog.exec()) {
            for (const QString &file : fileDialog.selectedFiles()) {
                fileList->addTopLevelItem(new QTreeWidgetItem({"?", file}));
            }
        }
    });

    resize(900, 600);

    setAcceptDrops(true);
}

void LyricDownloaderWindow::dragEnterEvent(QDragEnterEvent *e) {
    // If at least one of the files dragged inside is an MP3 file, an M4A file, or a folder, accept the drop.
    // If not, deny it.
    // This means we might get a folder full of JPEG files, but we can't recursively check everything here,
    // or the UI will be laggy like all hell when big folders are dragged.
    // Careful checking will need to happen on drop.
    for (const QUrl &url : e->mimeData()->urls()) {
        QString local = url.toLocalFile();
        QFileInfo fileInfo(local);
        if (fileInfo.isDir() || fileInfo.suffix() == "mp3" || fileInfo.suffix() == "m4a") {
            e->acceptProposedAction();
            return;
        }
    }
}

void LyricDownloaderWindow::dropEvent(QDropEvent *e) {
    QStringList types = { "mp3", "m4a" };
    for (const QUrl &url : e->mimeData()->urls()) {
        QString local = url.toLocalFile();
        QFileInfo fileInfo(local);
        if (fileInfo.suffix() == "mp3" || fileInfo.suffix() == "m4a") {
            fileList->addTopLevelItem(new QTreeWidgetItem({"?", local}));
        }
        else if (fileInfo.isDir()) {
            addFilesRecursively(local, 15);
        }
        else
            continue;
    }
}

void LyricDownloaderWindow::addFilesRecursively(const QString &sDir, int max_depth) {
    QDir dir(sDir);
    QFileInfoList list = dir.entryInfoList(dir.nameFilters(), QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo info : list) {
//        if (Application::shouldTerminate)
//            return {};
        QString sFilePath = info.filePath();
        QString absPath = info.absoluteFilePath();

        if (absPath.endsWith(".mp3", Qt::CaseInsensitive)|| absPath.endsWith(".m4a", Qt::CaseInsensitive)) {
            fileList->addTopLevelItem(new QTreeWidgetItem({"?", absPath}));
        }

        if (max_depth > 0 && info.isDir() && !info.isSymLink()) {
            addFilesRecursively(sFilePath, max_depth - 1);
        }
    }
}

