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

#include <QString>
#include <QTextDocument>
#include <QPlainTextDocumentLayout>
#include <QFileInfo>

#include "markdowndocument.h"

namespace ghostwriter
{
MarkdownDocument::MarkdownDocument(QObject *parent)
    : QTextDocument(parent), ast(nullptr)
{
    initializeUntitledDocument();
}

MarkdownDocument::MarkdownDocument(const QString &text, QObject *parent)
    : QTextDocument(text, parent), ast(nullptr)
{
    initializeUntitledDocument();
}

MarkdownDocument::~MarkdownDocument()
{
    QPlainTextDocumentLayout *documentLayout =
        new QPlainTextDocumentLayout(this);
    this->setDocumentLayout(documentLayout);

    m_filePath = QString();
    readOnlyFlag = false;
    m_displayName = tr("untitled");
    m_timestamp = QDateTime::currentDateTime();

    if (nullptr != ast) {
        delete ast;
        ast = nullptr;
    }
}

QString MarkdownDocument::displayName() const
{
    return m_displayName;
}

QString MarkdownDocument::filePath() const
{
    return m_filePath;
}

void MarkdownDocument::setFilePath(const QString &path)
{
    if (!path.isNull() && !path.isEmpty()) {
        QFileInfo fileInfo(path);
        m_filePath = fileInfo.absoluteFilePath();
        m_displayName = fileInfo.fileName();
    } else {
        m_filePath = QString();
        this->setReadOnly(false);
        this->setModified(false);
        m_displayName = tr("untitled");
    }

    emit filePathChanged();
}

bool MarkdownDocument::isNew() const
{
    return m_filePath.isNull() || m_filePath.isEmpty();
}

bool MarkdownDocument::isReadOnly() const
{
    return readOnlyFlag;
}

void MarkdownDocument::setReadOnly(bool readOnly)
{
    readOnlyFlag = readOnly;
}

QDateTime MarkdownDocument::timestamp() const
{
    return m_timestamp;
}

void MarkdownDocument::setTimestamp(const QDateTime &timestamp)
{
    this->m_timestamp = timestamp;
}


MarkdownAST *MarkdownDocument::markdownAST() const
{
    return ast;
}

void MarkdownDocument::setMarkdownAST(MarkdownAST *ast)
{
    this->ast = ast;
}

void MarkdownDocument::notifyTextBlockRemoved(const QTextBlock &block)
{
    emit textBlockRemoved(block.position());
    emit textBlockRemoved(block);
}

void MarkdownDocument::initializeUntitledDocument()
{
    QPlainTextDocumentLayout *documentLayout =
        new QPlainTextDocumentLayout(this);
    this->setDocumentLayout(documentLayout);

    m_filePath = QString();
    readOnlyFlag = false;
    m_displayName = tr("untitled");
    m_timestamp = QDateTime::currentDateTime();
}
} // namespace ghostwriter
