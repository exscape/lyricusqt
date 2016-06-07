#include "configdialog.h"
#include <QHeaderView>
#include <QEvent>
#include <QDropEvent>
#include <QDebug>
#include "application.h"
#include "shared.h"
#include <QMessageBox>
#include <QDir>

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent) {
    // Remove the "?" button next to the close button
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    siteTreeWidget = new QTreeWidget;

    siteHintLabel = new QLabel("Drag/drop sites to change priority");
    siteTreeGroup = new QGroupBox("Site priority");
    siteTreeVbox = new QVBoxLayout;
    siteTreeWidget = new QTreeWidget(this);

    vbox = new QVBoxLayout;
    hbox = new QHBoxLayout;
    okButton = new QPushButton("OK");
    applyButton = new QPushButton("Apply");
    cancelButton = new QPushButton("Cancel");

    siteTreeVbox->addWidget(siteHintLabel);
    siteTreeVbox->addWidget(siteTreeWidget);
    siteTreeGroup->setLayout(siteTreeVbox);

    hbox->addStretch();
    hbox->addWidget(okButton);
    hbox->addWidget(applyButton);
    hbox->addWidget(cancelButton);

    vbox->addWidget(siteTreeGroup);
    vbox->addLayout(hbox);

    siteTreeWidget->clear();
    siteTreeWidget->setIndentation(0);
    siteTreeWidget->setColumnCount(2);
    siteTreeWidget->setHeaderLabels({ "Priority", "Site" });
    siteTreeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    siteTreeWidget->header()->setStretchLastSection(true);

    siteTreeWidget->installEventFilter(this);
    siteTreeWidget->viewport()->installEventFilter(this);

    siteTreeWidget->setDragDropMode(QAbstractItemView::DragDrop);
    siteTreeWidget->setDefaultDropAction(Qt::MoveAction);

    setLayout(vbox);

    connect(okButton, &QPushButton::clicked, this, &ConfigDialog::ok);
    connect(applyButton, &QPushButton::clicked, this, &ConfigDialog::saveSettings);
    connect(cancelButton, &QPushButton::clicked, this, &ConfigDialog::cancel);

    resize(300, 200);

    connect(siteTreeWidget, &QTreeWidget::itemChanged, [&] {
        updateSiteList();
    });
}

void ConfigDialog::updateSiteList() {
    // Called when the list is re-ordered, in order to update the priority numbers.
    int priority = 1;
    for (int i = 0; i < siteTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = siteTreeWidget->topLevelItem(i);
        if (item->checkState(0) == Qt::Checked)
            item->setData(0, Qt::DisplayRole, QString::number(priority++));
        else
            item->setData(0, Qt::DisplayRole, "-");
    }

    siteTreeWidget->update();
}

bool ConfigDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Drop && (obj == siteTreeWidget || obj == siteTreeWidget->viewport())) {
        // The order of the sites might've changed, so we need to re-number them,
        // to make sure that the top site is shown as #1, the second one as #2, etc.
        // This is done as a queued call so that it happens *after* the drop. If we call this now, it has no effect, as
        // the current state of the widget reflects how it was BEFORE the drop.
        QMetaObject::invokeMethod(this, "updateSiteList", Qt::QueuedConnection);
    }

    return QDialog::eventFilter(obj, event);
}

void ConfigDialog::loadSettings() {
    // Set up the site priority list, from the settings
    siteTreeWidget->clear();
    QVector<QPair<bool, QString>> sitePriority = Application::getSetting("sitePriority").value<QVector<QPair<bool, QString>>>();

    int priority = 1;
    for (const QPair<bool, QString> &site : sitePriority) {
        QTreeWidgetItem *item = new QTreeWidgetItem({ QString::number(priority++), site.second });
        item->setCheckState(0, site.first ? Qt::Checked : Qt::Unchecked);
        item->setFlags(item->flags() & ~Qt::ItemIsDropEnabled);
        siteTreeWidget->addTopLevelItem(item);
    }

    updateSiteList();
}

bool ConfigDialog::saveSettings() {
    QVector<QPair<bool, QString>> sitePriority;

    int numSites = 0;
    for (int i = 0; i < siteTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = siteTreeWidget->topLevelItem(i);
        sitePriority.append(QPair<bool ,QString>(item->checkState(0) == Qt::Checked, item->data(1, Qt::DisplayRole).toString()));
        if (item->checkState(0) == Qt::Checked)
            numSites++;
    }

    if (numSites == 0) {
        QMessageBox::critical(this, "No sites selected", "You have no sites enabled in the site priority section. Please enable at least one (using the checkmark next to the site priority).", QMessageBox::Ok);
        return false;
    }

    Application::setSetting("sitePriority", QVariant::fromValue(sitePriority));

    const QString home = QDir::homePath();
    QVector<Path> pathsToIndex = {
                                   { joinPath(home, "Music"), 10 },
                                 };
    Application::setSetting("pathsToIndex", QVariant::fromValue(pathsToIndex));

    Application::writeSettings();

    return true;
}

void ConfigDialog::ok() {
    if (saveSettings())
        this->accept();
}

void ConfigDialog::cancel() {
    this->reject();
}
