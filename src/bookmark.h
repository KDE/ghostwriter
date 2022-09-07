/***********************************************************************
 *
 * Copyright (C) 2014-2022 wereturtle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

private:
    QScopedPointer<BookmarkPrivate> d;
};

typedef QList<Bookmark> Bookmarks;

} // namespace ghostwriter

#endif