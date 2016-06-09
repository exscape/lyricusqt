#ifndef FOOBARNOWPLAYINGANNOUNCER_H
#define FOOBARNOWPLAYINGANNOUNCER_H

#include <QObject>
#include <windows.h>

class FoobarNowPlayingAnnouncer : public QObject
{
    Q_OBJECT

public:
    explicit FoobarNowPlayingAnnouncer(QObject *parent = 0);
    void run();
signals:
    void newTrack(QString artist, QString title);
};

#endif // FOOBARNOWPLAYINGANNOUNCER_H
