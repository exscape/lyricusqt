#ifndef REVERSESEARCHWINDOW_H
#define REVERSESEARCHWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTreeWidget>
#include <QPlainTextEdit>

class DataModel;

class ReverseSearchWindow : public QMainWindow
{
    Q_OBJECT

    DataModel *dataModel = nullptr;

    QPushButton *indexButton = nullptr;
    QVBoxLayout *vbox = nullptr;
    QLineEdit *searchString = nullptr;
    QTreeWidget *results = nullptr;
    QPlainTextEdit *lyricDisplay = nullptr;

public:
    ReverseSearchWindow(DataModel *model, QWidget *parent);
    ~ReverseSearchWindow() {}
protected:
    void searchStringUpdated(QString newString);
};

#endif // REVERSESEARCHWINDOW_H
