#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UI/reversesearchwindow.h"
#include "UI/lyricdownloaderwindow.h"
#include "UI/manualdownloaderwindow.h"
#include "UI/configdialog.h"
#include <QMenuBar>
#include <QPlainTextEdit>

class ReverseSearchModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    ReverseSearchModel *reverseSearchModel = nullptr;
    ReverseSearchWindow *reverseSearchWindow = nullptr;
    LyricDownloaderWindow *lyricDownloaderWindow = nullptr;
    QPlainTextEdit *lyricsTextEdit = nullptr;
    ManualDownloaderWindow *manualDownloaderWindow = nullptr;
    ConfigDialog *configDialog = nullptr;

    QMenuBar *menuBar = nullptr;
    QMenu *fileMenu = nullptr;
    QMenu *editMenu = nullptr;
    QMenu *lyricsMenu = nullptr;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() {}

signals:

public slots:
};

#endif // MAINWINDOW_H
