#-------------------------------------------------
#
# Project created by QtCreator 2015-08-03T19:57:49
#
#-------------------------------------------------

QT       += core gui sql
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lyricusqt
TEMPLATE = app

LIBS += $$PWD/taglib-1.9.1/taglib-release/taglib.dll

SOURCES += main.cpp\
    datamodel.cpp \
    track.cpp \
    reversesearchwindow.cpp \
    mainwindow.cpp

HEADERS  += \
    datamodel.h \
    track.h \
    reversesearchwindow.h \
    mainwindow.h

INCLUDEPATH += taglib-1.9.1
INCLUDEPATH += taglib-1.9.1/taglib
INCLUDEPATH += taglib-1.9.1/taglib/ape
INCLUDEPATH += taglib-1.9.1/taglib/asf
INCLUDEPATH += taglib-1.9.1/taglib/flac
INCLUDEPATH += taglib-1.9.1/taglib/it
INCLUDEPATH += taglib-1.9.1/taglib/mp4
INCLUDEPATH += taglib-1.9.1/taglib/mod
INCLUDEPATH += taglib-1.9.1/taglib/mpc
INCLUDEPATH += taglib-1.9.1/taglib/mpeg
INCLUDEPATH += taglib-1.9.1/taglib/ogg
INCLUDEPATH += taglib-1.9.1/taglib/riff
INCLUDEPATH += taglib-1.9.1/taglib/s3m
INCLUDEPATH += taglib-1.9.1/taglib/toolkit
INCLUDEPATH += taglib-1.9.1/taglib/trueaudio
INCLUDEPATH += taglib-1.9.1/taglib/wavpack
INCLUDEPATH += taglib-1.9.1/taglib/xm
INCLUDEPATH += taglib-1.9.1/taglib/mpeg/id3v1
INCLUDEPATH += taglib-1.9.1/taglib/mpeg/id3v2
INCLUDEPATH += taglib-1.9.1/taglib/mpeg/id3v2/frames
INCLUDEPATH += taglib-1.9.1/taglib/ogg/flac
INCLUDEPATH += taglib-1.9.1/taglib/ogg/speex
INCLUDEPATH += taglib-1.9.1/taglib/ogg/vorbis
INCLUDEPATH += taglib-1.9.1/taglib/ogg/opus
INCLUDEPATH += taglib-1.9.1/taglib/riff/aiff
INCLUDEPATH += taglib-1.9.1/taglib/riff/wav
