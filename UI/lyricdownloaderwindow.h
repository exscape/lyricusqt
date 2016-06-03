#ifndef LYRICDOWNLOADERWINDOW_H
#define LYRICDOWNLOADERWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include "lyricfetcher.h"

class ReverseSearchModel;

class LyricDownloaderWindow : public QMainWindow
{
    Q_OBJECT
    
    ReverseSearchModel *dataModel = nullptr;
    LyricFetcher *lyricFetcher = nullptr;
    QGridLayout *gridLayout = nullptr;
    QLabel *artistLabel = nullptr;
    QLabel *titleLabel = nullptr;
    QLineEdit *artistLineEdit = nullptr;
    QLineEdit *titleLineEdit = nullptr;
    QPushButton *searchButton = nullptr;
    QPlainTextEdit *lyricsTextEdit = nullptr;

    
public:
    explicit LyricDownloaderWindow(ReverseSearchModel *model = 0, QWidget *parent = 0);

signals:

public slots:
};

#endif // LYRICDOWNLOADERWINDOW_H
