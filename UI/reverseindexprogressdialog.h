#ifndef REVERSEINDEXPROGRESSDIALOG_H
#define REVERSEINDEXPROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTime>

class ReverseIndexProgressDialog : public QDialog
{
    Q_OBJECT

    QProgressBar *progressBar = nullptr;
    QHBoxLayout *hbox = nullptr;
    QVBoxLayout *vbox = nullptr;
    QLabel *estimatedRemainingLabel = nullptr;
    QPushButton *abortButton = nullptr;
    QTime *time = nullptr;
    int lastUpdateTime = 0;

public:
    ReverseIndexProgressDialog(QWidget *parent);
    void setProgress(int finished, int total);

signals:
    void abortClicked();
};

#endif // REVERSEINDEXPROGRESSDIALOG_H
