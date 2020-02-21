1.8.1
=====

Fixes
-----

* Issue #443: MathJax 3.0 is now built into the HTML preview.  Note that this feature works best with Pandoc and requires an Internet connection.  Other Markdown processors are not currently supported.
* Issue #447: Cursor is no longer shown when editor is not in focus.
* Issue #461: Fixed Cheat Sheet to have correct image example.
* Issue #471: HUD window styling is fixed for Qt 5.13.
* Issue #472: Fixed typing paused timers for session statistics and for hiding HUD windows.
* Discount is not listed as an available Markdown processor if MultiMarkdown is installed.
* New application translations are available for the following languages:
    - Arabic
    - Dutch

1.8.0
=====

New Features
------------

* HTML preview has been ported from QtWebKit to QtWebEngine (Chromium).

Fixes
-----

* Issue #206: ghostwriter now has right-to-left language support.
* Issue #391: mathml option for pandoc is now used to render math equations for preview and export.
* Issue #411: Fixed crash when adding a new theme with no theme selected.
* Issue #412: Fixed export to PDF (wkhtmltopdf) failure with Pandoc.
* Issue #430: Fixed issue extra spaces appearing between `<br/>` tags in the HTML preview.
* Removed print support, since it has some odd bugs on some platforms, and most users would want to print their exported content anyway.
* Removed graphics fade effect for the bottom of the editor, as it is a risky feature that has caused bugs in the past and does not work on HiDPI screens.

1.7.4
=====

Fixes
-----

* Issue #178: Only use --quiet option for Pandoc version 2 or greater.
* Fixed words-per-minute calculations to not be jumpy or overly high.
* Fixed issue where creating a new theme resulted in inconsistent selection and display of themes in the Theme Selection dialog.
* Fixed broken icon when running under Wayland.
* Fixed screenshots in appdata.
* Issue #385: Updated resource files license to CC BY-SA 4.0.

1.7.3
=====

Fixes
-----

* Issue #369: Fixed segfault that occurred when changing the theme or interface style after opening the Preview Options dialog.

1.7.2
=====

Fixes
-----

* Fixed issue where the `--quiet` option was mistakenly used with older Pandoc versions that did not support its use.
* Worked around Qt 5.11 qinstall bug where running qmake and make would fail when installing the translation files

1.7.1
======

Fixes
-----

* Fixed regression in full screen mode where menu bar was not showing on mouse-over.
* Fixed regression where wrong theme was shown as selected on opening the Theme Selection dialog.
* Fixed issue #366 where warnings from Pandoc that were resulting in error dialogs on successful export.
* Fixed issue #292 where saving a document under Windows resulted in end-of-line characters being written as `\n` instead of `\r\n`.
* Fixed issue #360 where a graphical glitch under MacOS disabled the Theme Selection dialog's remove theme button.

1.7.0
=====

New Features
------------

* Added light and dark built-in themes based on the community Ulysses III theme Plainstraction.
* Live preview has been optimized to show the preview for typed characters immediately on smaller documents.
* Added ODT and EPUB 3 as export options for MultiMarkdown 6.
* Theme selection dialog now displays a grid instead of a list.

Fixes
-----

* The menu bar is now hidden again in full screen mode when the user presses `Alt+Tab` to switch to a different application.
* Fixed issue #354 to disable hard line breaks for Pandoc's Github-flavored Markdown.
* Worked around issue in Qt 5.11 where changing the theme causes the app to crash.
* Worked around issue in Qt 5.10 where opening a new document with a document already loaded can cause the app to crash.

1.6.2
=====

Fixes
-----

* Issue #315: Worked around slow load of QPrinter constructor in Fedora 27 by lazy loading the printer settings only when needed for printing or print preview rather than during application startup.
* Fixed line break tokenizing to not include paragraph breaks.

1.6.1
=====

Fixes
-----

* Worked around an issue with Qt version 5.10 that causes the application to occasionally crash when reloading the currently opened document or when opening a new document due to the text cursor not being set to the beginning of the document before clearing the editor and loading the new document.

1.6.0
=====

New Features
------------

* HTML live preview is now in a split pane view rather than in a separate window.
* New options were added to hide HUD windows while typing, in full screen mode, or in live preview mode.
* A new button was added to the status bar and a new shortcut was added to the View menu in the menu bar to toggle hiding/showing the open HUDs.
* Github's cmark-gfm can now be detected and used as a Markdown processor.
* Block quotes and code blocks now have an inobtrusive background color to differentiate them.
* Buttons have been added to the status for existing features.
* HUD windows and scrollbars can be stylized to have square or rounded corners in the Settings, and are no longer theme dependent.
* New "Typewriter" focus mode was added by @hirschenberger.
* The style sheet and Markdown processor to use with the HTML live preview can now be selected in their own dialog box accessible both from the status bar at the bottom of the window and from the menu bar in the Settings menu.
* The "Copy HTML" button has been moved to the "status bar" at the bottom of the main window.
* The "Copy HTML" button / menu bar option can copy just the selected Markdown text as HTML (using the currently selected Markdown processor). It will still copy the entire Markdown text document and convert it to HTML in the system clipboard if no text is selected.
* Themes now support options for emphasis color and block text color. Note that emphasis color will also affect the color of headings, and block text color will affect the color of block quotes and code blocks.

Fixes
-----

* Issue #302: ghostwriter will now look for default Qt translations on the standard Qt translations path (for standard buttons/text). These default Qt translations have been included in the Windows installer translations folder and portable Windows Zip, and will be loaded automatically based on the currently selected application language.
* Some tweaks have been made to the parsing/highlighting of emphasized text to handle nested emphasis cases better. As part of this effort, ghostwriter uses a Qt 5-specific regular expression class that is not available in Qt 4. As such, ghostwriter can no longer be built with Qt and must be built with Qt 5.
* Issue #272: Fixed issue with Focus mode where inserting matching characters around a selection was causing the application to crash.
* Issue #260: Added compatability with Pandoc version 2 and MultiMarkdown version 6.
* Fixed issue where HTML preview scrolls to the top if the new HTML from a modified document matches the old HTML, even if the markup used in the source document did change.
* Fixed issue where current word is not live spell checked if the user switches the text cursor to a different line.
* Fixed issue where prompt to reload a file that has been modified externally was being displayed multiple times per file change. This seems to have been triggered by some editors like Sublime Text that perform multiple file saves for a single file modification.
* Issue #228: Selected text can now be dragged and dropped to a different location within the editor.
* Fixed issue where if setext heading markup line is selected and deleted with the cursor position beginning at the end of the previous line, the heading is not removed from the Outline HUD.
* Fixed issue where Preview HTML menu item and button were not in sync to appear checked or unchecked.
* ghostwriter now clears the Outline HUD when loading or reloading a document to prevent a crash in case bad data is in the HUD.
* Fixed copy HTML issue to ensure Markdown text has proper formatting (i.e., greater than signs aren't converted to `&gt;`).
* ghostwriter will now search in the mozilla-dicts installation location for dictionaries on OpenBSD.

1.5.0
=====

New Features
------------

* HUD windows can now be closed using the Esc key when they have focus.
* A new option has been added to highlight double spaces at the end of a line.
* Github-style task lists can now be made using the `*` and `+` bullet point characters, in addition to the `-` character.
* Support for ConTeXt and wkhtmltopdf when exporting to PDF with Pandoc has been added.
* Auto-matching characters for selected text now respects the existing preferences for the matching done while typing without text selected.
* Typing a single quote (`'`) will no longer result in a second quote being automatically inserted after the cursor if the cursor is positioned in the middle of a word.
* Most settings have been moved into a new Preferences dialog to accommodate the ever increasing number of application options.
* Font size can now be increased by pressing `Ctrl+=` or decreased by pressing `Ctrl+-`. It can also be changed by using `Ctrl` and the mouse wheel.
* When exporting to other file formats, the output file will be opened after export with its default application.
* E-books created using Pandoc will now also be generated with a table of contents.
* When passing in a file path via the command line, a new file will be created with that path if it does not already exist.
* When typing a `*` character, ghostwriter would auto-insert a second `*` character in anticipation of emphasized text. The auto-inserted second `*` will now be removed if a space is typed to accommodate a bullet list instead.
* Rudimentary support for HiDPI has been added. See notes below.
* Various performance enhancements and tweaks have been made under the hood.

Fixes
-----

* Issue #128: Hyphenated words are now considered one word for spell checking.
* Issue #131: ghostwriter can now be built from its source tarball without needing git to be installed.
* Issue #132: Linux desktop file now specifies that ghostwriter can open only one file from the command line rather than multiple files.
* Issue #170: `Ctrl+M` now opens the HTML Preview instead of `Ctrl+W`. This is to avoid a conflict with closing windows in Linux.
* Issue #219: Copy and paste no longer makes the word count jump.
* Sundry minor fixes have been added.

1.4.2
=====

Fixes
-----

* Issue #113: Full screen clock now displays time based on locale.
* Issue #119: Null characters, which had been breaking the HTML preview, are no longer inserted when typing over selecting text.
* Issue #121: Code fences are now correctly highlighted when they contain blank lines.
* Issue #123: Statistics HUDs now have proper padding when alternating row colors is enabled.
* Issue #125: The translations folder has been moved up one folder for the portable distribution of ghostwriter.
* Sundry minor fixes have been added.

1.4.1
=====

Fixes
-----

* Issue #115: Pressing backspace at the end of a line no longer deletes adjacent line breaks.
* Issue #116: Renaming a theme no longer removes its background image.
* Issue #107: Main window is now rendered correctly when transitioning between full screen mode and maximized. Main window also remembers its normal size upon being restored from maximized after having been in full screen mode previously.

1.4.0
=====

New Features
------------

* The time of day optionally displays in full screen mode.
* The recent file history size has been increased to 10.
* The menu bar will optionally auto-hide in full screen mode when the mouse is not hovering over it.
* All new Hemingway mode button will disable backspace and delete keys.
* Two new HUDs display statistical information on the current writing session and document.
* Auto-matched characters can be customized.
* High DPI scaling has been enabled for Qt 5.6 and above.
* Classic Light and Classic Dark themes have new color palettes.
* Various aesthetics have been added to the main window.
* New application translations are available for the following languages:
    - German
    - Italian
    - Japanese
    - Portuguese (Brazil)
    - Russian

Fixes
-----

* Pandoc PDF export now has better defaults for margins and hyperlinks.
* Shortcut keys using `Alt` now use `Ctrl` to avoid conflicts with OS shortcut keys and to better accommodate non-USA keyboards.
* Dragging and dropping an image into the editor will now insert the image link at the mouse location.
* Sundry other fixes have been added.

1.3.1
=====

New Features
------------

* Added a new Markdown cheat sheet HUD window.
* Added proper English translation ts and qm files to support plurals.
* Updated ghostwriter to select application translation to use based on OS locale.

Fixes
-----

* Fixed DPI scaling for HTML Preview and Quick Reference Guide.
* Fixed Qt 5 API call that was causing ghostwriter to fail compilation under Qt 4.

1.2.5
=====

Fixes
-----

* Fixed an issue where export of Markdown document using Pandoc or other command line tools failed when spaces were in the output file path.
* Fixed an issue where numbered lists that were sublists to a bullet point list were not highlighted properly.

1.2.4
=====

Fixes
-----

* Fixed an issue where anchor tags for headings inserted for HTML Preview were conflicting with Pandoc's heading anchor tags.

1.2.3
=====

Fixes
-----

* Fixed an issue on Windows where UTF-8 encoding was not being detected on opening a file unless there was a BOM present. The application now defaults to UTF-8 when opening a file.

1.2.2
=====

Fixes
-----

* Fixed an issue where spelling suggestions were not always displayed in the context menu if the menu key was pressed on the keyboard.

1.2.1
=====

Fixes
-----

* Corrected a regression where the application stopped going to the last known cursor position upon opening a file.
* Fixed an issue where the context menu was showing up in the wrong position on the screen when the user pressed the keyboard's menu key for a misspelled word.
* Fixed an issue where spelling suggestions were not always being displayed in the context menu depending on which line in the text block the mouse was clicked.

1.2.0
=====

New Features
------------

* More intelligent list handling
* New Format menu
* Pipe table highlighting
* GitHub mentions highlighting

Fixes
-----

* Sundry fixes have been added

1.1.0
=====

New Features
------------

* Export dialog now categorizes export options by Markdown processor.
* Export dialog displays a checkbox option for smart typography.
* Support for the latest Pandoc version's CommonMark input format option has been added both to the export dialog and to the HTML preview window.
* Support for cmark has been added to the export dialog and HTML preview window. ghostwriter will automatically detect if cmark is installed (i.e., if its location is in the system's PATH environment variable).
* Font dialog's combo box now shows fonts WYSIWYG-style.

Fixes
--------

* Word counting is more accurate and does not regard markup text as separate words.

1.0.1
=====

Fixes
-----

* Internal enhancements to Markdown parsing class names, with comments added to the code.
* Updated the .pro file to be able to get the software version number through git even if there are only unannotated tags.

1.0.0
=====

Initial release.  Happy birthday, ghostwriter!
