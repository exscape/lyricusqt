#ifndef MANUALDOWNLOADERWINDOW_H
#define MANUALDOWNLOADERWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include "Models/lyricfetcher.h"

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
    ManualDownloaderWindow(QWidget *parent = 0);
    void fetchLyrics(QString artist, QString title);
    void setArtistAndTitle(QString artist, QString title);

signals:
    void fetchStarted(QString artist, QString title);
    void fetchComplete(QString artist, QString title, QString lyrics, FetchResult result);

protected:
    void validateFields();
};

#endif // MANUALDOWNLOADERWINDOW_H
