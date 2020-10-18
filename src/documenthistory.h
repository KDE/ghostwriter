/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#ifndef DOCUMENTHISTORY_H
#define DOCUMENTHISTORY_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QStringList>

namespace ghostwriter
{
/**
 * This class stores and retrieves recent file history using QSettings.
 * It is reentrant, and different instances can be used from anywhere to
 * access the same file history.
 */
class DocumentHistory
{
public:
    /**
     * Constructor.
     */
    DocumentHistory();

    /**
     * Destructor.
     */
    ~DocumentHistory();

    /**
     * Returns the list of recent files, up to the maximum number specified.
     * Specify a value of -1 to get the entire history.
     */
    QStringList recentFiles(int max = -1);

    /**
     * Adds the given file path and cursor position to the history.
     */
    void add(const QString &filePath, int cursorPosition);

    /**
     * Gets the last-known cursor position for the given file path.  This
     * will return 0 (beginning of the file) if the last cursor position
     * is unknown.
     */
    int cursorPosition(const QString &filePath);

    /**
     * Wipes the document history clean.
     */
    void clear();

signals:
    /**
     * Emitted when a recent file is added/removed from the history.
     */
    void recentFilesChanged();
};
}

#endif // DOCUMENTHISTORY_H
