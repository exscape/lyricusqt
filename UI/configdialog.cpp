#include "UI/configdialog.h"
#include "Misc/application.h"
#include "Misc/shared.h"
#include <QHeaderView>
#include <QEvent>
#include <QDropEvent>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>

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

    connect(siteTreeWidget, &QTreeWidget::itemChanged, [&] {
        updateSiteList();
    });

    pathsToIndexGroup = new QGroupBox("Paths to index (reverse lyric search)");
    pathsToIndexVbox = new QVBoxLayout;
    pathsToIndex = new QTreeWidget;

    connect(pathsToIndex, &QTreeWidget::currentItemChanged, this, &ConfigDialog::selectionChanged);
    connect(pathsToIndex, &QTreeWidget::itemChanged, this, &ConfigDialog::itemEdited);

    // Create buttons; edit/remove are disabled until we are sure something is selected (see selectionChanged)
    addButton = new QPushButton("A&dd...");
    editButton = new QPushButton("&Edit...");
    removeButton = new QPushButton("&Remove");
    editButton->setDisabled(true);
    removeButton->setDisabled(true);

    connect(addButton, &QPushButton::released, this, &ConfigDialog::addPath);
    connect(editButton, &QPushButton::released, this, &ConfigDialog::editPath);
    connect(removeButton, &QPushButton::released, this, &ConfigDialog::removePath);

    QHBoxLayout *buttonsHBox = new QHBoxLayout;
    buttonsHBox->addStretch(1);
    buttonsHBox->addWidget(addButton);
    buttonsHBox->addWidget(editButton);
    buttonsHBox->addWidget(removeButton);

    pathsToIndexVbox->addWidget(pathsToIndex);
    pathsToIndexVbox->addLayout(buttonsHBox);
    pathsToIndexGroup->setLayout(pathsToIndexVbox);

    connect(okButton, &QPushButton::clicked, this, &ConfigDialog::ok);
    connect(applyButton, &QPushButton::clicked, this, &ConfigDialog::saveSettings);
    connect(cancelButton, &QPushButton::clicked, this, &ConfigDialog::cancel);

    siteTreeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    siteTreeWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    pathsToIndex->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    vbox->addWidget(siteTreeGroup);
    vbox->addWidget(pathsToIndexGroup);
    vbox->addLayout(hbox);

    setLayout(vbox);
    resize(400, 400);

    setWindowTitle("Lyricus Preferences");
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

    editButton->setDisabled(true);
    removeButton->setDisabled(true);
    pathsToIndex->clear();
    pathsToIndex->setIndentation(0);
    pathsToIndex->setColumnCount(2);
    pathsToIndex->setHeaderLabels({ "Path", "Depth" });
    pathsToIndex->header()->setStretchLastSection(false);
    pathsToIndex->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    pathsToIndex->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    QList<QTreeWidgetItem*> items;
    for (const Path &path : Application::getSetting("pathsToIndex").value<QVector<Path>>()) {
        QString displayPath = path.path;
#ifdef Q_OS_WIN
        displayPath.replace('/', '\\');
#endif
        auto *item = new QTreeWidgetItem({ displayPath, QString::number(path.depth) });
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(0, PathRole, path.path);
        validatePath(item);
        items.append(item);
    }

    pathsToIndex->addTopLevelItems(items);
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
    QVector<Path> pathsToIndexVector = {
                                   { joinPath(home, "Music"), 10 },
                                 };
    Application::setSetting("pathsToIndex", QVariant::fromValue(pathsToIndexVector));

    Application::writeSettings();

    QVector<Path> paths;
    int num = pathsToIndex->topLevelItemCount();
    for (int i = 0; i < num; i++) {
        const QTreeWidgetItem *item = pathsToIndex->topLevelItem(i);
        QString path = item->data(0, PathRole).toString();
        int depth = item->data(1, Qt::DisplayRole).toInt();
        paths.append({ path, depth });
    }

    Application::setSetting("pathsToIndex", QVariant::fromValue(paths));

    return true;
}

void ConfigDialog::ok() {
    if (saveSettings())
        this->accept();
}

void ConfigDialog::cancel() {
    this->reject();
}

void ConfigDialog::addPath() {
    QString selectedDir = QFileDialog::getExistingDirectory(this, "Select a directory", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (selectedDir.length() > 0) {
        QString displayPath = selectedDir;
#ifdef Q_OS_WIN
        displayPath.replace('/', '\\');
#endif
        QTreeWidgetItem *item = new QTreeWidgetItem({ displayPath, QString::number(10) });
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(0, PathRole, selectedDir);
        pathsToIndex->addTopLevelItem(item);
        validatePath(item);
    }

    pathsToIndex->setCurrentItem(nullptr);
}

void ConfigDialog::editPath() {
    QTreeWidgetItem *item = pathsToIndex->currentItem();
    Q_ASSERT(item != nullptr);

    QDir dir(item->data(0, PathRole).toString());
    dir.cdUp();
    const QString dirToDisplay = dir.absolutePath();

    QString selectedDir = QFileDialog::getExistingDirectory(this, "Select a directory", dirToDisplay, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (selectedDir.length() > 0) {
        item->setData(0, PathRole, selectedDir);
#ifdef Q_OS_WIN
        selectedDir.replace('/', '\\');
#endif
        item->setData(0, Qt::DisplayRole, selectedDir);
        validatePath(item);
    }

    pathsToIndex->setCurrentItem(nullptr);
}

void ConfigDialog::removePath() {
    // This feels a bit unorthodox, but it does (and should!) work.
    // Note that deleting nullptr is safe, so we don't need to check the pointer first.
    // (Also, the remove button is only enabled when an item is actually selected.)
    delete pathsToIndex->currentItem();
}

void ConfigDialog::selectionChanged(QTreeWidgetItem *cur, QTreeWidgetItem *prev) {
    Q_UNUSED(prev);

    if (cur) {
        this->editButton->setEnabled(true);
        this->removeButton->setEnabled(true);
    }
    else {
        this->editButton->setEnabled(false);
        this->removeButton->setEnabled(false);
    }
}

void ConfigDialog::itemEdited(QTreeWidgetItem *item, int column) {
    // When an item is edited by the user, ensure the path they entered is valid.
    Q_UNUSED(column);
    qDebug() << "itemEdited";
    if (column == 0)  {
        QString path = item->data(0, Qt::DisplayRole).toString();
        path.replace('\\', '/');
        item->setData(0, PathRole, path);
        validatePath(item);
        pathsToIndex->setCurrentItem(nullptr);
    }
}

void ConfigDialog::validatePath(QTreeWidgetItem *item) {
    // Ensure that a displayed item's path exists. If it does not, display the name in red.
    QPalette palette;
    if (!QDir(item->data(0, PathRole).toString()).exists()) {
        item->setForeground(0, QBrush(Qt::red));
        item->setToolTip(0, "This path does not exist!");
    }
    else {
        item->setForeground(0, palette.text());
        item->setToolTip(0, "");
    }
}
