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
 * This class stores and retrieves recent file history using QSettings.
 * It is reentrant, and different instances can be used from anywhere to
 * access the same file history.
 */
class LibraryPrivate;
class Library
{
public:
    /**
     * Constructor.
     */
    Library();

    /**
     * Destructor. Will store data to disk.
     */
    ~Library();

    /**
     * Finds the bookmark for the given file path.  If none is found,
     * this method returns a null bookmark (i.e., the bookmark's
     * isNull() method returns true).
     */
    Bookmark lookup(const QString &filePath) const;

    /**
     * Returns the list of recent files, up to the maximum number specified.
     * Specify a value of -1 to get the entire history.
     */
    Bookmarks recentFiles(int max = -1);

    /**
     * Adds the given bookmark to the history.
     */
    void addRecent(const Bookmark &bookmark);

    /**
     * Adds the given bookmark to the history.
     */
    void addRecent(const QString &filePath, int position);

    /**
     * Removes the bookmark with the given file path from the history.
     */
    void removeRecent(const QString &filePath);

    /**
     * Removes the given bookmark from the history.
     */
    void removeRecent(const Bookmark &bookmark);

    /**
     * Wipes all saved bookmarks.
     */
    void clear();

    /**
     * Save data to disk.
     */
    void sync();

private:
    QScopedPointer<LibraryPrivate> d;
};
} // namespace ghostwriter

#endif // LIBRARY_H
