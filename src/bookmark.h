/*
 * SPDX-FileCopyrightText: 2014-2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QDateTime>
#include <QList>
#include <QScopedPointer>
#include <QString>

namespace ghostwriter
{
/**
 * This class represents a bookmark of a file and its last known cursor
 * position.  It is used with the Library class to store recent file history.
 */
class BookmarkPrivate;
class Bookmark
{
public:
    /**
     * Constructor for a null bookmark.
     */
    Bookmark();

    /**
     * Constructor for given file path and cursor position.
     * Note that if a negative cursor position value is specified,
     * the cursor position for this bookmark will be set to 0.
     */
    Bookmark(const QString &filePath, int position = 0);

    /**
     * Copy constructor.
     */
    Bookmark(const Bookmark &other);

    /**
     * Destructor.
     */
    ~Bookmark();

    /**
     * Returns the absolute file path of this bookmark.
     */
    QString filePath() const;

    /**
     * Returns the cursor position of this bookmark.
     */
    int cursorPosition() const;

    /**
     * Returns the time last read or last modified.  See QFileInfo::lastRead()
     * for details.
     */
    QDateTime lastRead() const;

    /**
     * Returns true if the bookmark's file permissions is readable, false
     * otherwise.
     */
    bool isReadable() const;

    /**
     * Returns true if the bookmark's file permissions is writeable, false
     * otherwise.
     */
    bool isWriteable() const;

    /**
     * Returns true if the bookmark's file path exists, false otherwise.
     */
    bool isValid() const;

    /**
     * Returns true if this bookmark is null (i.e., has no file path).
     */
    bool isNull() const;

    /**
     * Assignment operator overload.
     */
    Bookmark& operator=(const Bookmark &other);

    /**
     * Equal operator overload. Two bookmarks are considered equal if they
     * share the same file path, regardless of cursor position.
     */
    bool operator==(const Bookmark &other) const;
    
    /**
     * Not equal operator overload.
     */
    bool operator!=(const Bookmark &other) const;

    /**
     * Returns a pretty-print formatted string containing both the bookmark's
     * file path and its cursor position.
     */
    QString toString() const;

private:
    QScopedPointer<BookmarkPrivate> d;
};

typedef QList<Bookmark> BookmarkList;

} // namespace ghostwriter

#endif
