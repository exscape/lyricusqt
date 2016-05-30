#include "lyricfetcher.h"

LyricFetcher::LyricFetcher() {
}

void LyricFetcher::fetchLyrics(const QString &artist, const QString &title, std::function<void(const QString &, FetchResult)> callback) {
    /*	if (theTrack && ([theTrack containsString:@"(live" ignoringCaseAndDiacritics:YES] || [theTrack containsString:@"(demo" ignoringCaseAndDiacritics:YES])) {
        theTrack = [theTrack stringByReplacingOccurrencesOfRegex:@"(?i)(.*?)\\s*\\(live.*" withString:@"$1"];
        theTrack = [theTrack stringByReplacingOccurrencesOfRegex:@"(?i)(.*?)\\s*\\(demo.*" withString:@"$1"];
    }
    */

    DarkLyricsSite darkLyrics;
    darkLyrics.fetchLyrics(artist, title, callback);
}

/*
 * Single fetch case:
 * 1) Button is pushed
 * 2) LyricDownloaderWindow calls LyricFetcher::fetchLyrics with an attached lambda to call on completion
 * 3) fetchLyrics calls DarkLyricsSite::fetchLyrics, forwarding the lambda
 * 4) DarkLyricsSite::fetchLyrics uses the async Qt API to fetch the HTML, with a signal to itself to completion
 * 5) On completion, it parses the HTML, and then calls the lambda
 * 6) The lambda is executed, and updates the UI, and so on.
 *
 * Multi fetch case:
 * 1) A list of tracks is selected, and a button is pushed
 * 2) QtConcurrent or similar is used to run several requests simultaneously
 * 3) Each thread/task calls fetchLyrics with its own callback, etc.
 * 4) ...
 */
