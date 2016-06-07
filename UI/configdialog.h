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

    void ok();
    void cancel();
    bool eventFilter(QObject *obj, QEvent *event);
    Q_INVOKABLE void updateSiteList();
};

#endif // CONFIGDIALOG_H
