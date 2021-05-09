#-------------------------------------------------
#
# Project created by QtCreator 2015-08-03T19:57:49
#
#-------------------------------------------------

QT       += core gui sql network
CONFIG += c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lyricusqt
TEMPLATE = app

win32 {
    LIBS += -L$$PWD/taglib_from_vcpkg -ltag
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

INCLUDEPATH += $$PWD/taglib_from_vcpkg
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/ape
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/asf
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/flac
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/it
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/mp4
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/mod
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/mpc
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/mpeg
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/ogg
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/riff
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/s3m
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/toolkit
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/trueaudio
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/wavpack
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/xm
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/mpeg/id3v1
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/mpeg/id3v2
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/mpeg/id3v2/frames
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/ogg/flac
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/ogg/speex
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/ogg/vorbis
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/ogg/opus
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/riff/aiff
INCLUDEPATH += $$PWD/taglib_from_vcpkg/taglib/riff/wav
