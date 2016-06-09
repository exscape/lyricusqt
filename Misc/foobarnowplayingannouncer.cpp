#include "foobarnowplayingannouncer.h"
#include <QDebug>
#include <QThread>

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
    char *responseData = (char *)HeapAlloc(GetProcessHeap(), 0, BUFSIZE);
    if (!responseData) {
        qWarning() << "Unable to allocate memory for foo_simpleserver_announce response data";
        return;
    }

    bool initial_run = true;

    // Loop retrying to connect to the pipe, in case it fails (or disconnects after initially succeeding)
    while (true) {
restart_loop:
        if (!initial_run) {
            QThread::sleep(4);
        }
        else
            initial_run = false;

        HANDLE hPipe = CreateFile(TEXT("\\\\.\\pipe\\foo_simpleserver_announce"), GENERIC_READ | FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe == INVALID_HANDLE_VALUE) {
            continue;
        }

        DWORD mode = PIPE_READMODE_MESSAGE;
        if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
            qWarning() << "Unable to set PIPE_READMODE_MESSAGE for foo_simpleserver_announce pipe";
            CloseHandle(hPipe);
            continue;
        }

        // Loop while connected to this pipe
        while (true) {
            DWORD bytes_read = 0;
            bool success = ReadFile(hPipe, responseData, BUFSIZE, &bytes_read, NULL);

            if (!success) {
                qWarning() << "foo_simpleserver_announce ReadFile failed; GetLastError() ==" << GetLastError();
                CloseHandle(hPipe);
                goto restart_loop; // Weee!
            }

            if (bytes_read != sizeof(struct songInfo)) {
                qWarning() << "foo_simpleserver_announce read failed: unexpected data size" << bytes_read << "bytes";
                CloseHandle(hPipe);
                goto restart_loop; // Weee!
            }

            struct songInfo *info = reinterpret_cast<struct songInfo *>(responseData);

            QString artist = QString::fromUtf8(info->artist);
            QString title = QString::fromUtf8(info->title);

            if (artist.length() > 0 && title.length() > 0)
                emit newTrack(artist, title);
        }
    }

    // This won't ever be reached... oh well.
    HeapFree(GetProcessHeap(), 0, responseData);
}
