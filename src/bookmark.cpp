/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
