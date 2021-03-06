#-------------------------------------------------
#
# Project created by QtCreator 2015-08-03T19:57:49
#
#-------------------------------------------------

QT       += core gui sql network
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lyricusqt
TEMPLATE = app

win32 {
    LIBS += $$PWD/taglib-1.11.1/taglib-release/taglib.dll
    SOURCES += Misc/foobarnowplayingannouncer.cpp
    HEADERS += Misc/foobarnowplayingannouncer.h
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
    CONFIG += console # fix qDebug output
}

SOURCES +=\
    UI/reversesearchwindow.cpp \
    UI/mainwindow.cpp \
    UI/lyricdownloaderwindow.cpp \
    Sites/lyricsite.cpp \
    Sites/darklyricssite.cpp \
    Sites/azlyricssite.cpp \
    Sites/songmeaningssite.cpp \
    UI/manualdownloaderwindow.cpp \
    UI/configdialog.cpp \
    UI/reverseindexprogressdialog.cpp \
    Misc/application.cpp \
    Misc/main.cpp \
    Misc/shared.cpp \
    Misc/track.cpp \
    Models/lyricdownloaderworker.cpp \
    Models/lyricfetcher.cpp \
    Models/reversesearchmodel.cpp

HEADERS  += \
    UI/reversesearchwindow.h \
    UI/mainwindow.h \
    Sites/lyricsite.h \
    Sites/darklyricssite.h \
    Sites/azlyricssite.h \
    Sites/songmeaningssite.h \
    UI/manualdownloaderwindow.h \
    UI/lyricdownloaderwindow.h \
    UI/configdialog.h \
    UI/reverseindexprogressdialog.h \
    Misc/shared.h \
    Misc/track.h \
    Misc/application.h \
    Models/lyricdownloaderworker.h \
    Models/lyricfetcher.h \
    Models/reversesearchmodel.h

INCLUDEPATH += taglib-1.11.1
INCLUDEPATH += taglib-1.11.1/taglib
INCLUDEPATH += taglib-1.11.1/taglib/ape
INCLUDEPATH += taglib-1.11.1/taglib/asf
INCLUDEPATH += taglib-1.11.1/taglib/flac
INCLUDEPATH += taglib-1.11.1/taglib/it
INCLUDEPATH += taglib-1.11.1/taglib/mp4
INCLUDEPATH += taglib-1.11.1/taglib/mod
INCLUDEPATH += taglib-1.11.1/taglib/mpc
INCLUDEPATH += taglib-1.11.1/taglib/mpeg
INCLUDEPATH += taglib-1.11.1/taglib/ogg
INCLUDEPATH += taglib-1.11.1/taglib/riff
INCLUDEPATH += taglib-1.11.1/taglib/s3m
INCLUDEPATH += taglib-1.11.1/taglib/toolkit
INCLUDEPATH += taglib-1.11.1/taglib/trueaudio
INCLUDEPATH += taglib-1.11.1/taglib/wavpack
INCLUDEPATH += taglib-1.11.1/taglib/xm
INCLUDEPATH += taglib-1.11.1/taglib/mpeg/id3v1
INCLUDEPATH += taglib-1.11.1/taglib/mpeg/id3v2
INCLUDEPATH += taglib-1.11.1/taglib/mpeg/id3v2/frames
INCLUDEPATH += taglib-1.11.1/taglib/ogg/flac
INCLUDEPATH += taglib-1.11.1/taglib/ogg/speex
INCLUDEPATH += taglib-1.11.1/taglib/ogg/vorbis
INCLUDEPATH += taglib-1.11.1/taglib/ogg/opus
INCLUDEPATH += taglib-1.11.1/taglib/riff/aiff
INCLUDEPATH += taglib-1.11.1/taglib/riff/wav

SUBDIRS += \
    taglib-1.11.1/taglib/taglib.pro
