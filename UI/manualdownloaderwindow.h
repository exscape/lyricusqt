#ifndef MANUALDOWNLOADERWINDOW_H
#define MANUALDOWNLOADERWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include "lyricfetcher.h"

class ManualDownloaderWindow : public QDialog
{
    Q_OBJECT

    LyricFetcher *lyricFetcher = nullptr;
    QGridLayout *gridLayout = nullptr;
    QLabel *artistLabel = nullptr;
    QLabel *titleLabel = nullptr;
    QLineEdit *artistLineEdit = nullptr;
    QLineEdit *titleLineEdit = nullptr;
    QPushButton *searchButton = nullptr;

public:
    ManualDownloaderWindow();

signals:
    void fetchComplete(QString lyrics, FetchResult result);
};

#endif // MANUALDOWNLOADERWINDOW_H