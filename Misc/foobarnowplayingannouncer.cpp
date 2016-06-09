#include "foobarnowplayingannouncer.h"
#include <QDebug>

// TODO: REMOVE, DEBUGGING ONLY
#include <QWaitCondition>
#include <QMutex>

// This should be more than enough, since the struct is smaller
#define BUFSIZE 2048

struct songInfo {
    // These are in UTF-8; use QString::fromUtf8() to create a QString
    char artist[512];
    char title[512];
};

FoobarNowPlayingAnnouncer::FoobarNowPlayingAnnouncer(QObject *parent) : QObject(parent) {
}

void FoobarNowPlayingAnnouncer::run() {
    // TODO
    // TODO: error handling, retrying etc!
    // TODO

    HANDLE hPipe = CreateFile(TEXT("\\\\.\\pipe\\foo_simpleserver_announce"), GENERIC_READ | FILE_WRITE_ATTRIBUTES /* NO GENERIC_WRITE */, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        qWarning() << "Unable to connect to foo_simpleserver_announce pipe";
        return;
    }

    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
        qWarning() << "Unable to set PIPE_READMODE_MESSAGE for foo_simpleserver_announce pipe";
        return;
    }

    char *responseData = (char *)HeapAlloc(GetProcessHeap(), 0, BUFSIZE);
    if (!responseData) {
        CloseHandle(hPipe);
        qWarning() << "Unable to allocate memory for foo_simpleserver_announce response data";
        return;
    }

    while (true) {
        DWORD bytes_read = 0;
        bool success = ReadFile(hPipe, responseData, BUFSIZE, &bytes_read, NULL);

        if (!success) {
            qWarning() << "ReadFile failed; GetLastError() ==" << GetLastError();
            CloseHandle(hPipe);
            HeapFree(GetProcessHeap(), 0, responseData);
            return;
        }

        if (bytes_read != sizeof(struct songInfo)) {
            qWarning() << "foo_simpleserver_announce read failed: unexpected data size" << bytes_read << "bytes";
            CloseHandle(hPipe);
            HeapFree(GetProcessHeap(), 0, responseData);
            return;
        }


        struct songInfo *info = reinterpret_cast<struct songInfo *>(responseData);

        qDebug() << "New song:" << info->artist << "-" << info->title;
    }

//  FlushFileBuffers(hPipe);
    HeapFree(GetProcessHeap(), 0, responseData);

    /*
    int i = 0;
    while (true) {
        qDebug() << "FoobarNowPlayingAnnouncer still here...";
        QMutex mutex;
        mutex.lock();
        QWaitCondition waitCondition;
        waitCondition.wait(&mutex, 1000);
        mutex.unlock();

        if (i++ % 5 == 0)
            emit newTrack("Some artist", QString("Some track #%1").arg(i));
    }
    */
}
