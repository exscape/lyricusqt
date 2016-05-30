#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "UI/reversesearchwindow.h"
#include "UI/lyricdownloaderwindow.h"
#include <QHBoxLayout>

class DataModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    DataModel *dataModel = nullptr;
    QPushButton *reverseSearchButton = nullptr;
    QPushButton *lyricDownloaderButton = nullptr;
    ReverseSearchWindow *reverseSearchWindow = nullptr;
    LyricDownloaderWindow *lyricDownloaderWindow = nullptr;
    QHBoxLayout *hbox = nullptr;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() {}

signals:

public slots:
};

#endif // MAINWINDOW_H
