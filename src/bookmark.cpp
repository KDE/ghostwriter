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

#include <QFileInfo>

#include "bookmark.h"

namespace ghostwriter
{
class BookmarkPrivate
{
public:
    BookmarkPrivate() { }
    ~BookmarkPrivate() { }

    QString filePath;
    int position;
};

Bookmark::Bookmark()
    : d(new BookmarkPrivate())
{
    d->filePath = QString();
    d->position = 0;
}

Bookmark::Bookmark(const QString &filePath, int position)
    : d(new BookmarkPrivate())
{
    if (filePath.isNull()) {
        d->filePath = QString();
        d->position = 0;
        return;
    }

    d->filePath = QFileInfo(filePath).absoluteFilePath();
    
    if (position >= 0) {
        d->position = position;
    } else {
        d->position = 0;
    }
}

Bookmark::Bookmark(const Bookmark &other)
    : d(new BookmarkPrivate())
{
    d->filePath = other.d->filePath;
    d->position = other.d->position;
}

Bookmark::~Bookmark()
{
    ;
}

QString Bookmark::filePath() const
{
    return d->filePath;
}

int Bookmark::cursorPosition() const
{
    return d->position;
}

QDateTime Bookmark::lastRead() const
{
    return QFileInfo(d->filePath).lastRead();
}

bool Bookmark::isValid() const
{
    if (isNull()) {
        return false;
    }

    QFileInfo fileInfo(d->filePath);

    return (fileInfo.exists() && fileInfo.isFile());
}

bool Bookmark::isNull() const
{
    return (d->filePath.isNull());
}

Bookmark& Bookmark::operator=(const Bookmark &other)
{
    d->filePath = other.d->filePath;
    d->position = other.d->position;
    return *this;
}

bool Bookmark::operator==(const Bookmark &other) const
{
    return (other.d->filePath == this->d->filePath);
}

bool Bookmark::operator!=(const Bookmark &other) const
{
    return (other.d->filePath != this->d->filePath);
}

} // namespace ghostwriter
