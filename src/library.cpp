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

#include <QHash>
#include <QFileInfo>
#include <QSettings>
#include <QString>

#include "library.h"

#define MAX_FILE_HISTORY_SIZE 20
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

    Bookmarks recentFiles;
};

Library::Library() :
    d(new LibraryPrivate())
{
    // Load bookmarks.
    
    QSettings settings;
    int size = settings.beginReadArray(FILE_HISTORY_KEY);

    int bookmarksAdded = 0;

    for (int i = 0; i < size && i < MAX_FILE_HISTORY_SIZE; i++) {
        settings.setArrayIndex(i);

        QString filePath = settings.value(FILE_PATH_KEY).toString();
        int position = settings.value(CURSOR_POSITION_KEY, 0).toInt();

        Bookmark bookmark(filePath, position);

        if (bookmark.isValid()) {
            d->recentFiles.append(bookmark);
            bookmarksAdded++;
        }
    }

    settings.endArray();
}

Library::~Library()
{
    // Store updates to bookmarks on scope exit.
    this->sync();
}

Bookmark Library::lookup(const QString &filePath) const
{
    int index = d->recentFiles.indexOf(Bookmark(filePath));

    if (index >= 0) {
        return d->recentFiles.at(index);
    }

    return Bookmark(); // Not found
}

Bookmarks Library::recentFiles(int max)
{
    int size = d->recentFiles.size();

    if ((max < 0) || (max > size)) {
        max = size;
    }

    Bookmarks recentList;

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
    }
}

void Library::addRecent(const QString &filePath, int position)
{
    addRecent(Bookmark(filePath, position));
}

void Library::removeRecent(const QString &filePath)
{
    Bookmark bookmark(filePath, 0);
    d->recentFiles.removeAll(bookmark);
}

void Library::removeRecent(const Bookmark &bookmark)
{
    d->recentFiles.removeAll(bookmark);
}

void Library::clear()
{
    d->recentFiles.clear();
}

void Library::sync()
{
    // Store updates to bookmarks on scope exit.
    QSettings settings;

    if (d->recentFiles.isEmpty()) {
        settings.remove(FILE_HISTORY_KEY);
        return;
    }

    settings.beginWriteArray(FILE_HISTORY_KEY, d->recentFiles.size());

    for (int i = 0;
            (i < d->recentFiles.size()) && (i < MAX_FILE_HISTORY_SIZE);
            i++) {
        Bookmark bookmark = d->recentFiles.at(i);

        settings.setArrayIndex(i);
        settings.setValue(FILE_PATH_KEY, bookmark.filePath());
        settings.setValue
        (
            CURSOR_POSITION_KEY,
            bookmark.cursorPosition()
        );
    }

    settings.endArray();
}

} // namespace ghostwriter
