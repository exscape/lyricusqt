#include "shared.h"
#include <taglib/fileref.h>
#include <taglib/mpeg/mpegfile.h>
#include <taglib/mpeg/id3v2/frames/unsynchronizedlyricsframe.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mp4/mp4file.h>
#include <QFile>
#include <QDebug>
#include <QPair>

QString simplifiedRepresentation(const QString &source) {
    return source.toLower().replace(QRegularExpression("[^A-Za-z0-9 ]"), "");
}

QPair<QString, QString> artistAndTitleForFile(const QString &path) {
    TagLib::FileRef file(QFile::encodeName(path).constData());
    if (file.isNull())
        return {};

    return { TStringToQString(file.tag()->artist()), TStringToQString(file.tag()->title()) };
}

QString lyricsForFile(const QString &path) {
    if (path.endsWith(".mp3")) {
        TagLib::MPEG::File mpf(QFile::encodeName(path).constData());
        if (!mpf.isOpen() || !mpf.isValid())
            return {};

        auto *id3v2tag = mpf.ID3v2Tag();
        if (!id3v2tag)
            return {};

        TagLib::ID3v2::FrameList frames = id3v2tag->frameListMap()["USLT"];
        if (frames.isEmpty())
            return {};

        auto *frame = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(frames.front());
        if (!frame)
            return {};

        return frame->text().toCString(true);
    }
    else if (path.endsWith(".m4a")) {
        TagLib::MP4::File file(QFile::encodeName(path).constData());
        if (!file.isOpen() || !file.isValid())
            return {};

        TagLib::MP4::Item item = file.tag()->itemListMap()["\xa9lyr"];
        TagLib::StringList strings = item.toStringList();
        if (!strings.isEmpty()) {
            return strings.front().toCString(true);
        }
        else
            return {};
    }

    return {};
}

bool setLyricsForFile(const QString &path, const QString &lyrics) {
    if (path.endsWith(".mp3")) {
        TagLib::MPEG::File mpf(QFile::encodeName(path).constData());
        if (!mpf.isOpen() || !mpf.isValid())
            return false;

        auto *id3v2tag = mpf.ID3v2Tag();
        if (!id3v2tag)
            return false;

        TagLib::ID3v2::FrameList frames = id3v2tag->frameListMap()["USLT"];
        if (frames.isEmpty()) {
            qDebug() << "No USLT frame found; creating one";
            auto *frame = new TagLib::ID3v2::UnsynchronizedLyricsFrame;
            frame->setLanguage("eng");
            id3v2tag->addFrame(frame);
            frames = id3v2tag->frameListMap()["USLT"];
        }

        if (frames.isEmpty()) {
            qWarning() << "Unable to read/create USLT frame for" << path;
            return false;
        }

        auto *frame = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(frames.front());
        if (!frame)
            return false;

        frame->setText(QStringToTString(lyrics));

        bool success = mpf.save();
        if (!success)
            qWarning() << "Unable to save lyrics to" << path;

        return success;

    }
    else if (path.endsWith(".m4a")) {
        TagLib::MP4::File file(QFile::encodeName(path).constData());
        if (!file.isOpen() || !file.isValid())
            return {};

        file.tag()->itemListMap()["\xa9lyr"] = TagLib::StringList(QStringToTString(lyrics));

        return file.save();
    }

    return false;
}
