This repository contains the source and binaries for the "e" text editor.

= Links & Information =
Homepage: http://www.e-texteditor.com/
IRC: irc://freenode/etexteditor

= Git notes =
Windows users in particulary should take note of the Line Endings section.

== Line Endings ==
This is a Liux/Windows cross-platform project. Because of this and git's behavior,
file line-endings are important to get right and keep consistent.

The default for all new files in this repo is to use UNIX-style "LF" endings.
The only time Windows-style line endings "CRLF" should be used is if some 
Windows-specific file requires them to work properly.

=== Git settings ===
For Windows users, edit .git/config and under [core] add the setting:
	autocrlf = false
	
This will preserve line-endings as they exist in the repo.

=== Visual Studio ===

Visual Studio does *not* have a setting for "always use LF"; to save a file 
with different line ends you must use the "File | Advanced Save Options"
dialog and then re-save the file.

Always `git diff` before committing; if every line in a file is marked as
changed then you probably converted CRLF to LF accidentally. Resave the file
and re-diff before committing.


= Building e =
See `windows-notes.txt` or `linux-notes.txt` as appropriate.

= Additional dependencies and add-ons =
 * e Support folder & built-in themes and bundles: http://code.google.com/p/ebundles/
 * wxCocoaDialog: http://github.com/adamv/wxcocoadialog/tree/master
 * e-find-in-files: http://github.com/adamv/e-find-in-files/tree/master

= Bugs, Build Problems and Feature Requests =
GitHub now has an issue tracking feature. e itself already has several
avenues for bug reports and feature requests, as can be found from the
e homepage.

Issues related to building and packaging e itself, or suggested code changes/
refactorings may be placed in the GitHub issue tracker:
 * http://github.com/etexteditor/e/issues
