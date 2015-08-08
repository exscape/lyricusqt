#include "mainwindow.h"
#include <QDesktopWidget>
#include "datamodel.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    dataModel = new DataModel;
    reverseSearchButton = new QPushButton("Reverse lyric search");

    QDesktopWidget dw;

    connect(reverseSearchButton, &QPushButton::clicked, [&]  {
        if (reverseSearchWindow == nullptr)
            reverseSearchWindow = new ReverseSearchWindow(dataModel, this);
        reverseSearchWindow->show();
        reverseSearchWindow->setFocus();
    });

    setCentralWidget(reverseSearchButton);
    move(dw.width() / 2 - width()/2, dw.height() / 2 - height()/2);
    setWindowTitle("Lyricus");
}
