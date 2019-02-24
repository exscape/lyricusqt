#include "UI/reverseindexprogressdialog.h"
#include "Misc/application.h"
#include <QDebug>

ReverseIndexProgressDialog::ReverseIndexProgressDialog(QWidget *parent) : QDialog(parent) {
    // Remove the window buttons
    setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) & ~Qt::WindowCloseButtonHint);

    progressBar = new QProgressBar;
    hbox = new QHBoxLayout;
    abortButton = new QPushButton("Abort");
    progressBar->setTextVisible(false);

    estimatedRemainingLabel = new QLabel;
    vbox = new QVBoxLayout;

    // This makes it an indeterminate progress bar until the files have been scanned,
    // and the total number of songs to index is known.
    progressBar->setValue(0);
    progressBar->setMaximum(0);

    hbox->addWidget(progressBar);
    hbox->addWidget(abortButton);

    vbox->addLayout(hbox);
    vbox->addWidget(estimatedRemainingLabel);

    setLayout(vbox);

    connect(abortButton, &QPushButton::clicked, [this] {
        abortClicked();
        Application::processEvents();
    });

    resize(500, 1);

    time = new QTime();

    setWindowTitle("Indexing lyrics, please wait...");
}

void ReverseIndexProgressDialog::setProgress(int finished, int total) {
    if (finished == 0) {
        time->restart();
        lastUpdateTime = 0;
    }

    QString estimatedRemaining = "Estimated time remaining: ";

    if (time->elapsed() < 5000 || finished == 0) {
        estimatedRemaining += "Calculating...";
    }
    else if (finished > 0) {
        int elapsed = time->elapsed();
        int est = static_cast<int>(elapsed * (static_cast<double>(total)/static_cast<double>(finished) - 1.0));
        est /= 1000;

        if (est < 10)
            estimatedRemaining += "less than 10 seconds";
        else if (est < 60) {
            est /= 5;
            est *= 5;
            estimatedRemaining += QString("about %1 seconds").arg(est);
        }
        else {
            est /= 10;
            est *= 10;
            if (est % 60 == 0)
                estimatedRemaining += QString("%1 minute%2").arg(est/60).arg(est/60 == 1 ? "" : "s");
            else
                estimatedRemaining += QString("%1 minute%2, %3 seconds").arg(est/60).arg(est/60 == 1 ? "" : "s").arg(est%60);
        }
    }

    if (time->elapsed() >= lastUpdateTime + 2000 || lastUpdateTime == 0) {
        estimatedRemainingLabel->setText(estimatedRemaining);
        lastUpdateTime = time->elapsed();
    }

    progressBar->setMaximum(total);
    progressBar->setValue(finished);
    progressBar->repaint();
}
