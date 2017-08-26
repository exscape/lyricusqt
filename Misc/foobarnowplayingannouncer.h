#ifndef FOOBARNOWPLAYINGANNOUNCER_H
#define FOOBARNOWPLAYINGANNOUNCER_H

#include <QObject>
#include <QString>
#include <windows.h>

class FoobarNowPlayingAnnouncer : public QObject
{
    Q_OBJECT

public:
    explicit FoobarNowPlayingAnnouncer(QObject *parent = 0);
    void run();
    void reEmitLastTrack();
signals:
    void newTrack(QString artist, QString title, QString path);
protected:
    QString lastArtist;
    QString lastTitle;
    QString lastPath;
};

#endif // FOOBARNOWPLAYINGANNOUNCER_H
