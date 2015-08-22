/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
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

/**
 * This class stores and retrieves recent file history using QSettings.
 * It is reentrant, and different instances can be used from anywhere to
 * access the same file history.
 */
class DocumentHistory
{
    public:
        DocumentHistory();
        ~DocumentHistory();

        QStringList getRecentFiles(int max = -1);
        void add(const QString& filePath, int cursorPosition);
        int getCursorPosition(const QString& filePath);
        void clear();

    signals:
        void recentFilesChanged();

    private:
        class RecentFile
        {
            public:
                QString filePath;
                int position;

                inline bool operator==(const RecentFile& other)
                {
                    return (other.filePath == filePath);
                }
        };

        typedef QList<RecentFile> RecentFilesList;

        RecentFilesList loadFromSettings(int max = -1);
        void storeToSettings(const RecentFilesList& recentFiles);
        void cleanUpHistory(RecentFilesList& recentFiles);
};

#endif // DOCUMENTHISTORY_H
