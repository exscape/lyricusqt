#ifndef REVERSESEARCHWINDOW_H
#define REVERSESEARCHWINDOW_H

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTreeWidget>
#include <QPlainTextEdit>
#include "reversesearchmodel.h"
#include "UI/reverseindexprogressdialog.h"

class ReverseSearchWindow : public QWidget
{
    Q_OBJECT

    ReverseSearchModel *reverseSearchModel = nullptr;

    ReverseIndexProgressDialog *reverseIndexProgressDialog = nullptr;

    QPushButton *indexButton = nullptr;
    QVBoxLayout *vbox = nullptr;
    QLineEdit *searchString = nullptr;
    QTreeWidget *results = nullptr;
    QPlainTextEdit *lyricDisplay = nullptr;

public:
    ReverseSearchWindow(QWidget *parent = 0);
    void checkIndex();
    ~ReverseSearchWindow() {}
protected:
    void searchStringUpdated(QString newString);
};

#endif // REVERSESEARCHWINDOW_H
