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

    // This makes it an indeterminate progress bar until the files have been scanned,
    // and the total number of songs to index is known.
    progressBar->setValue(0);
    progressBar->setMaximum(0);

    hbox->addWidget(progressBar);
    hbox->addWidget(abortButton);

    setLayout(hbox);

    connect(abortButton, &QPushButton::clicked, [this] {
        abortClicked();
        Application::processEvents();
    });

    resize(500, 1);

    setWindowTitle("Indexing lyrics, please wait...");
}

void ReverseIndexProgressDialog::setProgress(int finished, int total) {
    progressBar->setMaximum(total);
    progressBar->setValue(finished);
    progressBar->repaint();
}
