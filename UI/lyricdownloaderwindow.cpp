#include "UI/lyricdownloaderwindow.h"
#include <QMessageBox>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QAction>

LyricDownloaderWindow::LyricDownloaderWindow(QWidget *parent) : QMainWindow(parent) {
    qRegisterMetaType<LyricStatus>("LyricStatus");

    fileList = new QTreeWidget;
    fileList->setIndentation(0);
    fileList->setColumnCount(2);
    fileList->setHeaderLabels({ "Lyrics?", "File" });
    fileList->header()->setStretchLastSection(true);
    fileList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    fileList->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    fileList->header()->setStretchLastSection(true);
//  fileList->model()->setHeaderData(0, Qt::Horizontal, Qt::AlignCenter, Qt::TextAlignmentRole);
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    fileList->installEventFilter(this);

    vbox = new QVBoxLayout;
    bottomHbox = new QHBoxLayout;

    addFolderButton = new QPushButton("Add folder...");
    addFilesButton = new QPushButton("Add file(s)...");
    startDownloadButton = new QPushButton("Start download");
    startDownloadButton->setEnabled(false);
    overwriteLyricsCheckBox = new QCheckBox("Download and overwrite lyrics for files that already have them");
    progressBar = new QProgressBar;
    progressBar->setMinimum(0);
    progressBar->setMaximum(10);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);

    bottomHbox->addStretch();
    bottomHbox->addWidget(addFolderButton);
    bottomHbox->addWidget(addFilesButton);
    bottomHbox->addWidget(startDownloadButton);

    QWidget *central = new QWidget;
    central->setLayout(vbox);
    vbox->addWidget(fileList);
    vbox->addWidget(progressBar);
    vbox->addWidget(overwriteLyricsCheckBox);
    vbox->addLayout(bottomHbox);
    setCentralWidget(central);

    connect(addFolderButton, &QPushButton::clicked, [&] {
        QFileDialog fileDialog;
        fileDialog.setFileMode(QFileDialog::Directory);
        fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
        if (fileDialog.exec()) {
            if (fileDialog.selectedFiles().length() > 0)
                startDownloadButton->setEnabled(true);
            // Only one directory is supposed to be selectable, but just in case...
            for (const QString &dir : fileDialog.selectedFiles()) {
                addFilesRecursively(dir, 15);
            }
        }

        if (fileList->topLevelItemCount() > 0)
            startDownloadButton->setEnabled(true);
    });

    connect(addFilesButton, &QPushButton::clicked, [&] {
        QFileDialog fileDialog;
        fileDialog.setNameFilter("Supported files (*.mp3 *.m4a);;MP3 files (*.mp3);;M4A files (*.m4a)");
        fileDialog.setFileMode(QFileDialog::ExistingFiles);
        if (fileDialog.exec()) {
            if (fileDialog.selectedFiles().length() > 0)
                startDownloadButton->setEnabled(true);
            for (const QString &file : fileDialog.selectedFiles()) {
                addFile(file);
            }
        }

        if (fileList->topLevelItemCount() > 0)
            startDownloadButton->setEnabled(true);
    });

    connect(startDownloadButton, &QPushButton::clicked, this, &LyricDownloaderWindow::startButtonClicked);

    resize(900, 600);

    setAcceptDrops(true);
}

void LyricDownloaderWindow::startButtonClicked() {
    if (workerThread) {
        Q_ASSERT(!workerThread->isRunning());
        delete workerThread;
        workerThread = nullptr;
    }
    if (worker) {
        delete worker;
        worker = nullptr;
    }

    // <index, path> -- this is used because the index into filesToProcess may not equal the index of the fileList.
    // They only match if every track is NotProcessed, so this would fail if a previous operation had been aborted
    // and then resumed.
    // The worker needs to know the index into the fileList (for status updates), so we include it here.
    QList<QPair<int, QString>> filesToProcess;
    for (int i = 0; i < fileList->topLevelItemCount(); i++) {
        auto *item = fileList->topLevelItem(i);
        LyricStatus status = item->data(0, Qt::UserRole).value<LyricStatus>();
        if (status == LyricStatus::NotProcessed || overwriteLyricsCheckBox->isChecked())
            filesToProcess.append({ i, item->data(1, Qt::DisplayRole).toString() });
    }

    if (filesToProcess.length() == 0) {
        QMessageBox::information(this, "Done", "Nothing to do -- all tracks are already processed!", QMessageBox::Ok);
        return;
    }

    progressBar->setValue(0);
    progressBar->setMaximum(filesToProcess.length());
    progressBar->setTextVisible(true);

    worker = new LyricDownloaderWorker(filesToProcess, overwriteLyricsCheckBox->isChecked()); // Copy the data to the worker
    workerThread = new QThread;
    worker->moveToThread(workerThread);

    connect(workerThread, &QThread::started, worker, &LyricDownloaderWorker::process);
    connect(worker, &LyricDownloaderWorker::finished, workerThread, &QThread::quit);

    connect(worker, &LyricDownloaderWorker::updateProgress, this, &LyricDownloaderWindow::progressUpdate);
    connect(worker, &LyricDownloaderWorker::finished, this, [this] {
        progressBar->setValue(0);
        progressBar->setTextVisible(false);

        disconnect(startDownloadButton, &QPushButton::clicked, 0, 0);
        connect(startDownloadButton, &QPushButton::clicked, this, &LyricDownloaderWindow::startButtonClicked);
        startDownloadButton->setText("Start download");
        startDownloadButton->setEnabled(true);

        addFilesButton->setEnabled(true);
        addFolderButton->setEnabled(true);
        overwriteLyricsCheckBox->setEnabled(true);
    }, Qt::QueuedConnection);

    fileList->topLevelItem(0)->setBackgroundColor(0, QColor(133, 137, 255)); // Highlight the current item in blue

    workerThread->start();

    connect(worker, &LyricDownloaderWorker::aborted, this, [this](int lastProcessedIndex) {
        auto *item = fileList->topLevelItem(lastProcessedIndex + 1);
        if (item)
            item->setBackgroundColor(0, QColor(Qt::red));
    }, Qt::QueuedConnection);

    disconnect(startDownloadButton, &QPushButton::clicked, 0, 0);
    connect(startDownloadButton, &QPushButton::clicked, worker, &LyricDownloaderWorker::abort);
    connect(startDownloadButton, &QPushButton::clicked, [this] {
        startDownloadButton->setText("Stopping...");
        startDownloadButton->setEnabled(false);
    });
    startDownloadButton->setText("Stop download");
    startDownloadButton->setEnabled(true);

    addFilesButton->setEnabled(false);
    addFolderButton->setEnabled(false);
    overwriteLyricsCheckBox->setEnabled(false);
}

void LyricDownloaderWindow::progressUpdate(int index, LyricStatus status) {
    Q_ASSERT(index < fileList->topLevelItemCount());
    auto *item = fileList->topLevelItem(index);

    item->setData(0, Qt::UserRole, qVariantFromValue(status));

    if (status == LyricStatus::DownloadFailed) {
        item->setBackgroundColor(0, QColor(Qt::red));
        item->setData(0, Qt::DisplayRole, "Error");
    }
    else if (status == LyricStatus::NoLyricsFound) {
        item->setBackgroundColor(0, QColor(Qt::yellow));
        item->setData(0, Qt::DisplayRole, "No");
    }
    else if (status == LyricStatus::HadLyrics || status == LyricStatus::LyricsDownloaded) {
        item->setBackground(0, QColor(Qt::green));
        item->setData(0, Qt::DisplayRole, "Yes");
    }

    auto *nextItem = fileList->topLevelItem(index + 1);
    if (nextItem) {
        nextItem->setBackgroundColor(0, QColor(133, 137, 255)); // Highlight the current item in blue
        fileList->scrollToItem(nextItem);
    }
    else
        fileList->scrollToItem(item);

    progressBar->setValue(progressBar->value() + 1);

    fileList->repaint();
}

bool LyricDownloaderWindow::eventFilter(QObject *target, QEvent *event) {
    Q_ASSERT(target == fileList);
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Delete && (workerThread == nullptr || !workerThread->isRunning())) {
            for (const auto &item : fileList->selectedItems()) {
                delete item;
            }

            if (fileList->topLevelItemCount() == 0)
                startDownloadButton->setEnabled(false);
        }
    }

    return QMainWindow::eventFilter(target, event);
}

void LyricDownloaderWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (workerThread && workerThread->isRunning())
        return;

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
            addFile(local);
        }
        else if (fileInfo.isDir()) {
            addFilesRecursively(local, 15);
        }
        else
            continue;
    }

    if (fileList->topLevelItemCount() > 0)
        startDownloadButton->setEnabled(true);
}

void LyricDownloaderWindow::addFile(const QString &file) {
    auto *item = new QTreeWidgetItem({"?", file});
    item->setData(0, Qt::UserRole, qVariantFromValue(LyricStatus::NotProcessed));
    fileList->addTopLevelItem(item);
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
            addFile(absPath);
        }

        if (max_depth > 0 && info.isDir() && !info.isSymLink()) {
            addFilesRecursively(sFilePath, max_depth - 1);
        }
    }
}

