#ifndef REVERSEINDEXPROGRESSDIALOG_H
#define REVERSEINDEXPROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class ReverseIndexProgressDialog : public QDialog
{
    Q_OBJECT

    QProgressBar *progressBar = nullptr;
    QHBoxLayout *hbox = nullptr;
    QPushButton *abortButton = nullptr;

public:
    ReverseIndexProgressDialog(QWidget *parent);
    void setProgress(int finished, int total);

signals:
    void abortClicked();
};

#endif // REVERSEINDEXPROGRESSDIALOG_H
