#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

#include <QHash>
#include <QVariant>
#include <QVector>
#include <QPair>

#include <QGroupBox>
#include <QTreeWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

class ConfigDialog : public QDialog
{
    Q_OBJECT

    QGroupBox *siteTreeGroup = nullptr;
    QLabel *siteHintLabel = nullptr;
    QVBoxLayout *siteTreeVbox = nullptr;
    QTreeWidget *siteTreeWidget = nullptr;

    QGroupBox *pathsToIndexGroup = nullptr;
    QVBoxLayout *pathsToIndexVbox = nullptr;
    QTreeWidget *pathsToIndex = nullptr;
    QPushButton *addButton = nullptr;
    QPushButton *editButton = nullptr;
    QPushButton *removeButton = nullptr;

    QVBoxLayout *vbox = nullptr;
    QHBoxLayout *hbox = nullptr;
    QPushButton *okButton = nullptr;
    QPushButton *applyButton = nullptr;
    QPushButton *cancelButton = nullptr;

    QHash<QString, QVariant> _settings;
    QHash<QString, QVariant> _defaultSettings;

public:
    ConfigDialog(QWidget *parent);
    void loadSettings();
    bool saveSettings();

protected:
    void addPath();
    void editPath();
    void removePath();
    void selectionChanged(QTreeWidgetItem *cur, QTreeWidgetItem *prev);
    void itemEdited(QTreeWidgetItem *item, int column);
    void validatePath(QTreeWidgetItem *item);

    void ok();
    void cancel();
    bool eventFilter(QObject *obj, QEvent *event);
    Q_INVOKABLE void updateSiteList();
};

#endif // CONFIGDIALOG_H
