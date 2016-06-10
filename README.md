About this project
==================
This project is the successor to Lyricus, "a minimalist lyric reader/editor for Mac OS X".
It is a cross-platform (but still Windows-focused) lyric reader/downloader.

It is still in its early stages, but the following basic functionality is there as of 2016-06-10:

* Display lyrics to the current track in foobar2000 (support for other players is planned)
* Download lyrics to any number of tracks at once
* Index and search by lyrics to find (local) tracks that contain them
* Support for .mp3, .m4a and FLAC file formats
* Support for AZLyrics, Darklyrics and Songmeanings


About foobar2000 support
------------------------

Unfortunately, a foobar2000 plugin (or "component" using their terminology) is required, as
there is no way for an external application to fetch information from foobar2000 natively.  
My foo_simpleserver plugin (found in a separate repository) takes care of that part.
