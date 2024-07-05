/*
 * SPDX-FileCopyrightText: 2014-2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#include <QDateTime>
#include <QFileInfo>
#include <QList>
#include <QScopedPointer>
#include <QString>

#include "bookmark.h"

namespace ghostwriter
{
/**
 * This class stores and retrieves the application's active (i.e., currently
 * opened) file and the recent file history using QSettings. It is reentrant,
 * and different instances can be used from anywhere to access the same file
 * data.
 */
class LibraryPrivate;
class Library
{
public:
    /**
     * A null bookmark (isNull() returns true) that represents an empty,
     * untitled document (but NOT a draft with a temporary path).
     */
    static const Bookmark UNTITLED;

    /**
     * Constructor.
     */
    Library();

    /**
     * Destructor. Will store data to disk.
     */
    ~Library();

    /**
     * Returns the last opened file's path and cursor position as a bookmark.
     * Note that if the last opened file was an untitled, empty file, then
     * this method will return UNTITLED (i.e., the Bookmark will be null, with
     * isNull() returning true).
     */
    const Bookmark &lastOpened() const;

    /**
     * Sets the last opened file. The prior file (if not untitled) will be
     * added to the recent file history by default, unless remember is set
     * to false.
     *
     * Set to UNTITLED to specify that a new, untitled file should be
     * restored for the next session.
     */
    void setLastOpened(const Bookmark &bookmark, bool remember = true);

    /**
     * Replaces the last opened file with the given bookmark.  Call this method
     * if the file was renamed or the cursor position changed.
     */
    void updateLastOpened(const Bookmark &bookmark);

    /**
     * Finds the bookmark for the bookmark at the given file path, regardless
     * of cursor position. If none is found, this method returns a null bookmark
     * (i.e., the bookmark's isNull() method returns true).
     */
    Bookmark lookup(const Bookmark &bookmark) const;

    /**
     * Returns the list of recent files, up to the maximum number specified.
     * Specify a value of -1 to get the entire history.
     */
    BookmarkList recentFiles(int max = -1);

    /**
     * Adds the given bookmark to the history.
     */
    void addRecent(const Bookmark &bookmark);

    /**
     * Adds the given bookmark to the history.
     */
    void addRecent(const QString &filePath, int position);

    /**
     * Removes the given bookmark from the history.
     */
    void removeRecent(const Bookmark &bookmark);

    /**
     * Wipes all saved bookmarks in the recent files history,
     * including the last opened file bookmark.
     */
    void clearHistory();

    /**
     * Save data to disk.
     */
    void sync();

private:
    QScopedPointer<LibraryPrivate> d;
};
} // namespace ghostwriter

#endif // LIBRARY_H
