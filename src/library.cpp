/*
 * SPDX-FileCopyrightText: 2014-2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <QDebug>
#include <QFileInfo>
#include <QHash>
#include <QSettings>
#include <QString>

#include "library.h"

#define MAX_FILE_HISTORY_SIZE 100
#define LAST_OPENED_FILE_KEY "LastOpenedFile"
#define FILE_HISTORY_KEY "FileHistory"
#define FILE_PATH_KEY "filePath"
#define CURSOR_POSITION_KEY "cursorPosition"

namespace ghostwriter
{

class LibraryPrivate
{
public:
    LibraryPrivate() { }
    ~LibraryPrivate() { }

    bool dirty;
    Bookmark lastOpened;
    BookmarkList recentFiles;
};

const Bookmark Library::UNTITLED = Bookmark();

Library::Library()
    : d(new LibraryPrivate())
{
    QString lastOpenedPath;
    int lastOpenedPos;

    QSettings settings;

    // Load the last opened file path and cursor position.
    d->lastOpened = UNTITLED;

    settings.beginGroup(LAST_OPENED_FILE_KEY);
    lastOpenedPath = settings.value(FILE_PATH_KEY, QString()).toString();
    lastOpenedPos = settings.value(CURSOR_POSITION_KEY, 0).toInt();
    settings.endGroup();

    if (!lastOpenedPath.isEmpty()) {
        if (lastOpenedPos < 0) {
            lastOpenedPos = 0;
        }

        d->lastOpened = Bookmark(lastOpenedPath, lastOpenedPos);

        // If the file no longer exists, default to an untitled file.
        if (!d->lastOpened.isValid()) {
            d->lastOpened = UNTITLED;
        }
    }

    // Load the recent file history.
    int size = settings.beginReadArray(FILE_HISTORY_KEY);

    for (int i = 0; i < size && i < MAX_FILE_HISTORY_SIZE; i++) {
        settings.setArrayIndex(i);

        QString filePath = settings.value(FILE_PATH_KEY).toString();
        int position = settings.value(CURSOR_POSITION_KEY, 0).toInt();

        Bookmark bookmark(filePath, position);

        if (bookmark.isValid()) {
            d->recentFiles.append(bookmark);
        }
    }

    settings.endArray();

    d->dirty = false;
}

Library::~Library()
{
    // Store updates to bookmarks on scope exit.
    sync();
}

const Bookmark &Library::lastOpened() const
{
    return d->lastOpened;
}

void Library::setLastOpened(const Bookmark &bookmark, bool remember)
{
    if (bookmark.isNull() || bookmark.isValid()) {
        if (remember && d->lastOpened.isValid() && (bookmark != d->lastOpened)) {
            addRecent(d->lastOpened);
        }

        d->lastOpened = bookmark;
        removeRecent(d->lastOpened);
        d->dirty = true;
    }
}

void Library::updateLastOpened(const Bookmark &bookmark)
{
    if (bookmark.isNull() || bookmark.isValid()) {
        removeRecent(d->lastOpened);
        removeRecent(bookmark);
        d->lastOpened = bookmark;
        d->dirty = true;
    }
}

Bookmark Library::lookup(const Bookmark &bookmark) const
{
    if (d->lastOpened == bookmark) {
        return d->lastOpened;
    }

    int index = d->recentFiles.indexOf(bookmark);

    if (index >= 0) {
        return d->recentFiles.at(index);
    }

    return Bookmark(); // Not found
}

BookmarkList Library::recentFiles(int max)
{
    int size = d->recentFiles.size();

    if ((max < 0) || (max > size)) {
        max = size;
    }

    BookmarkList recentList;

    for (int i = 0; i < max; i++) {
        recentList.append(d->recentFiles.at(i));
    }

    return recentList;
}

void Library::addRecent
(
    const Bookmark &bookmark
)
{
    if (bookmark.isValid()) {
        d->recentFiles.removeAll(bookmark);
        d->recentFiles.prepend(bookmark);
        d->dirty = true;
    }
}

void Library::addRecent(const QString &filePath, int position)
{
    addRecent(Bookmark(filePath, position));
}

void Library::removeRecent(const Bookmark &bookmark)
{
    d->recentFiles.removeAll(bookmark);
    d->dirty = true;
}

void Library::clearHistory()
{
    d->recentFiles.clear();
    d->dirty = true;
}

void Library::sync()
{
    if (!d->dirty) {
        return;
    }

    // Store updates to bookmarks on scope exit.
    QSettings settings;

    if (!d->lastOpened.isValid()) {
        settings.remove(LAST_OPENED_FILE_KEY);
    } else {
        settings.beginGroup(LAST_OPENED_FILE_KEY);
        settings.setValue(FILE_PATH_KEY, d->lastOpened.filePath());
        settings.setValue(CURSOR_POSITION_KEY, d->lastOpened.cursorPosition());
        settings.endGroup();
    }

    if (d->recentFiles.isEmpty()) {
        settings.remove(FILE_HISTORY_KEY);
    } else {
        settings.beginWriteArray(FILE_HISTORY_KEY, d->recentFiles.size());

        for (int i = 0; (i < d->recentFiles.size()) && (i < MAX_FILE_HISTORY_SIZE); i++) {
            Bookmark bookmark = d->recentFiles.at(i);

            settings.setArrayIndex(i);
            settings.setValue(FILE_PATH_KEY, bookmark.filePath());
            settings.setValue(CURSOR_POSITION_KEY, bookmark.cursorPosition());
        }

        settings.endArray();
    }
}

} // namespace ghostwriter
