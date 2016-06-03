#ifndef LYRICDOWNLOADERWINDOW_H
#define LYRICDOWNLOADERWINDOW_H

#include <QMainWindow>
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
#include "lyricfetcher.h"

class ReverseSearchModel;

class LyricDownloaderWindow : public QMainWindow
{
    Q_OBJECT
    
    LyricFetcher *lyricFetcher = nullptr;
    QTreeWidget *fileList = nullptr;
    QVBoxLayout *vbox = nullptr;
    QHBoxLayout *bottomHbox = nullptr;

    QPushButton *addFolderButton = nullptr;
    QPushButton *addFilesButton = nullptr;
    QPushButton *startDownloadButton = nullptr;
    
public:
    explicit LyricDownloaderWindow(QWidget *parent = 0);
    bool eventFilter(QObject *target, QEvent *event);

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void addFilesRecursively(const QString &sDir, int max_depth);
    void addFile(const QString &file);
};

#endif // LYRICDOWNLOADERWINDOW_H
