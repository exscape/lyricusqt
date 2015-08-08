#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "reversesearchwindow.h"

class DataModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    DataModel *dataModel = nullptr;
    QPushButton *reverseSearchButton = nullptr;
    ReverseSearchWindow *reverseSearchWindow = nullptr;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() {}

signals:

public slots:
};

#endif // MAINWINDOW_H
