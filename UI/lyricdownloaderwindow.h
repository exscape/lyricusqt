#ifndef LYRICDOWNLOADERWINDOW_H
#define LYRICDOWNLOADERWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTreeWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QCheckBox>
#include <QProgressBar>
#include <QThread>
#include "Models/lyricdownloaderworker.h"

class ReverseSearchModel;

class LyricDownloaderWindow : public QWidget
{
    Q_OBJECT
    
    QTreeWidget *fileList = nullptr;
    QVBoxLayout *vbox = nullptr;
    QHBoxLayout *bottomHbox = nullptr;

    QProgressBar *progressBar = nullptr;
    QCheckBox *overwriteLyricsCheckBox = nullptr;
    QPushButton *addFolderButton = nullptr;
    QPushButton *addFilesButton = nullptr;
    QPushButton *startDownloadButton = nullptr;

    LyricDownloaderWorker *worker = nullptr;
    QThread *workerThread = nullptr;
    
public:
    explicit LyricDownloaderWindow(QWidget *parent = 0);
    bool eventFilter(QObject *target, QEvent *event);

    void progressUpdate(int index, LyricStatus status);
protected:
    void startButtonClicked();
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void closeEvent(QCloseEvent *e) override;
    void addFilesRecursively(const QString &sDir, int max_depth);
    void addFile(const QString &file);
};

#endif // LYRICDOWNLOADERWINDOW_H
