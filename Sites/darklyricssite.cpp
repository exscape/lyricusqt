#include "darklyricssite.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

DarkLyricsSite::DarkLyricsSite() {
}

void DarkLyricsSite::fetchLyrics(const QString &artist, const QString &title, std::function<void (const QString &, FetchResult)> callback) {
    qWarning() << artist << "-" << title;
    QString artistURL;
    FetchResult result;
    std::tie(artistURL, result) = getArtistURL(artist);
    if (artistURL.length() < 1 || result != FetchResult::Success) {
        callback({}, result);
        return;
    }

    QString trackURL;
    std::tie(trackURL, result) = getTrackURL(title, artistURL);
    if (trackURL.length() < 1 || result != FetchResult::Success) {
        callback({}, result);
        return;
    }

    qWarning() << "Track URL:" << trackURL;

    callback("lyrics", FetchResult::Success);
}

std::tuple<QString, FetchResult> DarkLyricsSite::getArtistURL(const QString &_artist) const {
    // These URLs have a simple format, so we can figure them out instead of having to
    // fetch and parse HTML. Nice and speedy!
    //
    // We begin with stripping everything but regular characters, including whitespace, and convert to lowercase.
    // E.g. "Dream Theater" -> "dreamtheater"
    QString artist = _artist;
    artist = artist.toLower().replace(QRegularExpression("[^A-Za-z0-9]"), "");

    if (artist.length() < 1)
        return std::make_tuple(QString(), FetchResult::InvalidRequest);

    if (artist[0].isDigit())
        return std::make_tuple(QString("http://www.darklyrics.com/19/%1.html").arg(artist), FetchResult::Success);
    else
        return std::make_tuple(QString("http://www.darklyrics.com/%1/%2.html").arg(QString(artist[0]), artist), FetchResult::Success);
}

std::tuple<QString, FetchResult> DarkLyricsSite::getTrackURL(const QString &title, const QString &artistURL) const {

   // return std::make_tuple("testurl",  FetchResult::Success);

    QNetworkAccessManager qnam;
    QNetworkRequest req(QUrl(artistURL));
    QNetworkReply reply = qnam.get(req);
    connect(reply, &QNetworkReply::finished, [&] {

    })



#if 0
    //
    // Looks through an artist page (i.e. "http://www.darklyrics.com/d/darktranquillity.html") for the track link
    //
    NSError *err = nil;
    NSString *html = [TBUtil getHTMLFromURLUsingUTF8:artistURL error:&err];
    if (html == nil) {
        if (err != nil) {
            NSMutableDictionary *errorDetail = [NSMutableDictionary dictionary];
            [errorDetail setValue:@"Unable to download lyrics. This could be a problem with your internet connection or the site(s) used." forKey:NSLocalizedDescriptionKey];
            if (error != nil) {
                *error = [NSError errorWithDomain:@"org.exscape.Lyricus" code:LyricusHTMLFetchError userInfo:errorDetail];
            }
        }
        return nil;
    }

    NSString *regex =
    @"<a href=\"../(lyrics/[^#]*?)\\#\\d+\">([^<]*?)</a><br[^>]*>";

    NSArray *matchArray = [html arrayOfDictionariesByMatchingRegex:regex withKeysAndCaptures:@"url", 1, @"title", 2, nil];
    for (NSDictionary *match in matchArray) {
        if ([TBUtil string:title isEqualToString:[match objectForKey:@"title"]]) // we ignore all non-chars, so that "The Serpent's chalice" matches "the Serpents chalice" and so on.
            return [[match objectForKey:@"url"] stringByReplacingOccurrencesOfRegex:@"^lyrics/" withString:@"http://www.darklyrics.com/lyrics/"];
    }
    return nil;
#endif
}

#if 0
-(NSString *) fetchLyricsForTrack:(NSString *)title byArtist:(NSString *)artist withBatch:(BOOL)batch error:(NSError **)error {
    //
    // The only method called from the outside
    //
    if (!batch) [self sendStatusUpdate:@"Trying darklyrics..." ofType:LyricusNoteHeader];

    if (!batch) [self sendStatusUpdate:@"Searching for artist page..." ofType:LyricusNoteStartedWorking];
    NSURL *artistURL = [self getURLForArtist:artist];
    if (artistURL == nil) {
        if (!batch) [self sendStatusUpdate:@"Searching for artist page..." ofType:LyricusNoteFailure];
        return nil;
    }
    else
        if (!batch) [self sendStatusUpdate:@"Searching for artist page..." ofType:LyricusNoteSuccess];

    if (!batch) [self sendStatusUpdate:@"Searching for lyric page..." ofType:LyricusNoteStartedWorking];
    NSString *trackURL = [self getLyricURLForTrack:title fromArtistURL: artistURL error:error];
    if (trackURL == nil) {
        if (!batch) [self sendStatusUpdate:@"Searching for lyric page..." ofType:LyricusNoteFailure];
        return nil;
    }
    else
        if (!batch) [self sendStatusUpdate:@"Searching for lyric page..." ofType:LyricusNoteSuccess];

    if (!batch) [self sendStatusUpdate:@"Downloading lyrics..." ofType:LyricusNoteStartedWorking];
    NSString *lyrics = [self extractLyricsFromURL:trackURL forTrack:title error:error];

    if (lyrics == nil || [lyrics length] < 5) {
        if (!batch) [self sendStatusUpdate:@"Downloading lyrics..." ofType:LyricusNoteFailure];
        return nil;
    }
    else {
        if (!batch) [self sendStatusUpdate:@"Downloading lyrics..." ofType:LyricusNoteSuccess];
        return lyrics;
    }
}

#pragma mark -
#pragma mark Internal/private

-(NSString *)getLyricURLForTrack:(NSString *)title fromArtistURL:(NSURL *)artistURL error:(NSError **)error {
    //
    // Looks through an artist page (i.e. "http://www.darklyrics.com/d/darktranquillity.html") for the track link
    //
    NSError *err = nil;
    NSString *html = [TBUtil getHTMLFromURLUsingUTF8:artistURL error:&err];
    if (html == nil) {
        if (err != nil) {
            NSMutableDictionary *errorDetail = [NSMutableDictionary dictionary];
            [errorDetail setValue:@"Unable to download lyrics. This could be a problem with your internet connection or the site(s) used." forKey:NSLocalizedDescriptionKey];
            if (error != nil) {
                *error = [NSError errorWithDomain:@"org.exscape.Lyricus" code:LyricusHTMLFetchError userInfo:errorDetail];
            }
        }
        return nil;
    }

    NSString *regex =
    @"<a href=\"../(lyrics/[^#]*?)\\#\\d+\">([^<]*?)</a><br[^>]*>";

    NSArray *matchArray = [html arrayOfDictionariesByMatchingRegex:regex withKeysAndCaptures:@"url", 1, @"title", 2, nil];
    for (NSDictionary *match in matchArray) {
        if ([TBUtil string:title isEqualToString:[match objectForKey:@"title"]]) // we ignore all non-chars, so that "The Serpent's chalice" matches "the Serpents chalice" and so on.
            return [[match objectForKey:@"url"] stringByReplacingOccurrencesOfRegex:@"^lyrics/" withString:@"http://www.darklyrics.com/lyrics/"];
    }
    return nil;
}

-(NSString *)extractLyricsFromURL:(NSString *)url forTrack:(NSString *)trackName error:(NSError **)error {
    //
    // Extracts and returns the lyrics from a given URL and trackname.
    //
    if (url == nil)
        return nil;

    // This cache is only used for the session, and means that instead of re-downloading the lyrics for an album
    // 10 times for 10 tracks, we'll only download it once. The cache is NOT saved between sessions, which also shouldn't be necessary.
    // (Darklyrics displays the lyrics for ALL tracks an on album on the same page, so fetching it 10 times for 10 tracks is just stupid.)
    NSString *source;
    NSError *err = nil;
    if ([albumCache objectForKey:url] != nil) {
        source = [albumCache objectForKey:url];
    }
    else {
        source = [TBUtil getHTMLFromURLUsingUTF8:[NSURL URLWithString:url] error:&err];
        if (source)
            [albumCache setObject:source forKey:url];
    }

    if (source == nil) {
        if (err != nil) {
            NSMutableDictionary *errorDetail = [NSMutableDictionary dictionary];
            [errorDetail setValue:@"Unable to download lyrics. This could be a problem with your internet connection or the site(s) used." forKey:NSLocalizedDescriptionKey];
            if (error != nil) {
                *error = [NSError errorWithDomain:@"org.exscape.Lyricus" code:LyricusHTMLFetchError userInfo:errorDetail];
            }
        }
        return nil;
    }

    NSMutableString *lyrics;
    NSString *regex =
    @"(?i)<h3><a name=\"\\d+\">\\d+\\. ([^<]*?)</a></h3><br />\\s*([\\s\\S]+?)(?=<br /><br />)";
    // Ah, the beauty of regular expressions.

    NSArray *matchesArray = [source arrayOfDictionariesByMatchingRegex:regex withKeysAndCaptures:@"title", 1, @"lyrics", 2, nil];

    for (NSDictionary *match in matchesArray) {
        if ([TBUtil string:[match objectForKey:@"title"] isEqualToString:trackName]) {
            lyrics = [[match objectForKey:@"lyrics"] mutableCopy];
            [lyrics replaceOccurrencesOfRegex:@"<[^>]*>" withString:@""];

            return [lyrics stringByTrimmingWhitespace];
        }
    }

    // If we've checked all titles, something is wrong, since darklyrics provides a list of the lyrics supported to be at this URL. This is likely because the regular expression doesn't match due to site updates.

    NSMutableDictionary *errorDetail = [NSMutableDictionary dictionary];
    [errorDetail setValue:@"Unable to parse lyrics at darklyrics. Please report this to the developer at serenity@exscape.org!" forKey:NSLocalizedDescriptionKey];
    if (error != nil) {
        *error = [NSError errorWithDomain:@"org.exscape.Lyricus" code:LyricusLyricParseError userInfo:errorDetail];
    }
    return nil;
}
#endif
