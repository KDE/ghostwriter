/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
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

#include <QString>
#include <QTextDocument>
#include <QPlainTextDocumentLayout>
#include <QFileInfo>

#include "TextDocument.h"

TextDocument::TextDocument(QObject* parent)
    : QTextDocument(parent)
{
    initializeUntitledDocument();
}

TextDocument::TextDocument(const QString& text, QObject* parent)
    : QTextDocument(text, parent)
{
    initializeUntitledDocument();
}

TextDocument::~TextDocument()
{
    QPlainTextDocumentLayout* documentLayout =
        new QPlainTextDocumentLayout(this);
    this->setDocumentLayout(documentLayout);

    filePath = QString();
    readOnlyFlag = false;
    displayName = tr("untitled");
    timestamp = QDateTime::currentDateTime();
}

QString TextDocument::getDisplayName() const
{
    return displayName;
}

QString TextDocument::getFilePath() const
{
    return filePath;
}

void TextDocument::setFilePath(const QString& path)
{
    if (!path.isNull() && !path.isEmpty())
    {
        QFileInfo fileInfo(path);
        filePath = fileInfo.absoluteFilePath();
        displayName = fileInfo.fileName();
    }
    else
    {
        filePath = QString();
        this->setReadOnly(false);
        this->setModified(false);
        displayName = tr("untitled");
    }

    emit filePathChanged();
}

bool TextDocument::isNew() const
{
    return filePath.isNull() || filePath.isEmpty();
}

bool TextDocument::isReadOnly() const
{
    return readOnlyFlag;
}

void TextDocument::setReadOnly(bool readOnly)
{
    readOnlyFlag = readOnly;
}

QDateTime TextDocument::getTimestamp() const
{
    return timestamp;
}

void TextDocument::setTimestamp(const QDateTime& timestamp)
{
    this->timestamp = timestamp;
}

void TextDocument::notifyTextBlockRemoved(const QTextBlock& block)
{
    emit textBlockRemoved(block.position());
    emit textBlockRemoved(block);
}

void TextDocument::initializeUntitledDocument()
{
    QPlainTextDocumentLayout* documentLayout =
        new QPlainTextDocumentLayout(this);
    this->setDocumentLayout(documentLayout);

    filePath = QString();
    readOnlyFlag = false;
    displayName = tr("untitled");
    timestamp = QDateTime::currentDateTime();
}
