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

LIBS += $$PWD/taglib-1.11/taglib-release/taglib.dll

SOURCES += main.cpp\
    datamodel.cpp \
    track.cpp \
    UI/reversesearchwindow.cpp \
    UI/mainwindow.cpp \
    UI/lyricdownloaderwindow.cpp \
    lyricfetcher.cpp \
    Sites/lyricsite.cpp \
    Sites/darklyricssite.cpp \
    Sites/azlyricssite.cpp \
    Sites/songmeaningssite.cpp

HEADERS  += \
    datamodel.h \
    track.h \
    UI/reversesearchwindow.h \
    UI/mainwindow.h \
    UI/lyricdownloaderwindow.h \
    lyricfetcher.h \
    Sites/lyricsite.h \
    Sites/darklyricssite.h \
    Sites/azlyricssite.h \
    Sites/songmeaningssite.h

INCLUDEPATH += taglib-1.11
INCLUDEPATH += taglib-1.11/taglib
INCLUDEPATH += taglib-1.11/taglib/ape
INCLUDEPATH += taglib-1.11/taglib/asf
INCLUDEPATH += taglib-1.11/taglib/flac
INCLUDEPATH += taglib-1.11/taglib/it
INCLUDEPATH += taglib-1.11/taglib/mp4
INCLUDEPATH += taglib-1.11/taglib/mod
INCLUDEPATH += taglib-1.11/taglib/mpc
INCLUDEPATH += taglib-1.11/taglib/mpeg
INCLUDEPATH += taglib-1.11/taglib/ogg
INCLUDEPATH += taglib-1.11/taglib/riff
INCLUDEPATH += taglib-1.11/taglib/s3m
INCLUDEPATH += taglib-1.11/taglib/toolkit
INCLUDEPATH += taglib-1.11/taglib/trueaudio
INCLUDEPATH += taglib-1.11/taglib/wavpack
INCLUDEPATH += taglib-1.11/taglib/xm
INCLUDEPATH += taglib-1.11/taglib/mpeg/id3v1
INCLUDEPATH += taglib-1.11/taglib/mpeg/id3v2
INCLUDEPATH += taglib-1.11/taglib/mpeg/id3v2/frames
INCLUDEPATH += taglib-1.11/taglib/ogg/flac
INCLUDEPATH += taglib-1.11/taglib/ogg/speex
INCLUDEPATH += taglib-1.11/taglib/ogg/vorbis
INCLUDEPATH += taglib-1.11/taglib/ogg/opus
INCLUDEPATH += taglib-1.11/taglib/riff/aiff
INCLUDEPATH += taglib-1.11/taglib/riff/wav

SUBDIRS += \
    taglib-1.11/taglib/taglib.pro
