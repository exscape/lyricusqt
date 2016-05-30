#ifndef LYRICDOWNLOADERWINDOW_H
#define LYRICDOWNLOADERWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "lyricfetcher.h"

class DataModel;

class LyricDownloaderWindow : public QMainWindow
{
    Q_OBJECT
    
    DataModel *dataModel = nullptr;
    LyricFetcher *lyricFetcher = nullptr;
    QGridLayout *gridLayout = nullptr;
    QLabel *artistLabel = nullptr;
    QLabel *titleLabel = nullptr;
    QLineEdit *artistLineEdit = nullptr;
    QLineEdit *titleLineEdit = nullptr;
    QPushButton *searchButton = nullptr;
    
    
public:
    explicit LyricDownloaderWindow(DataModel *model = 0, QWidget *parent = 0);

signals:

public slots:
};

#endif // LYRICDOWNLOADERWINDOW_H
