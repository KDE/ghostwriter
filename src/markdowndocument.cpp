/***********************************************************************
 *
 * Copyright (C) 2014-2021 wereturtle
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
class MarkdownDocumentPrivate
{
    Q_DECLARE_PUBLIC(MarkdownDocument)

public:
    MarkdownDocumentPrivate(MarkdownDocument *q_ptr) 
        : q_ptr(q_ptr){ }
    ~MarkdownDocumentPrivate() { }

    QString displayName;
    QString filePath;
    bool readOnlyFlag;
    QDateTime timestamp;
    MarkdownAST *ast;

    MarkdownDocument *q_ptr;

    /*
    * Initializes the class for an untitled document.
    */
    void initializeUntitledDocument();
};

MarkdownDocument::MarkdownDocument(QObject *parent)
    : QTextDocument(parent), d_ptr(new MarkdownDocumentPrivate(this))
{
    Q_D(MarkdownDocument);

    d->initializeUntitledDocument();
}

MarkdownDocument::MarkdownDocument(const QString &text, QObject *parent)
    : QTextDocument(text, parent), d_ptr(new MarkdownDocumentPrivate(this))
{
    Q_D(MarkdownDocument);

    d->initializeUntitledDocument();
}

MarkdownDocument::~MarkdownDocument()
{
    Q_D(MarkdownDocument);

    QPlainTextDocumentLayout *documentLayout =
        new QPlainTextDocumentLayout(this);
    this->setDocumentLayout(documentLayout);

    if (nullptr != d->ast) {
        delete d->ast;
        d->ast = nullptr;
    }
}

QString MarkdownDocument::displayName() const
{
    Q_D(const MarkdownDocument);

    return d->displayName;
}

QString MarkdownDocument::filePath() const
{
    Q_D(const MarkdownDocument);

    return d->filePath;
}

void MarkdownDocument::setFilePath(const QString &path)
{
    Q_D(MarkdownDocument);

    if (!path.isNull() && !path.isEmpty()) {
        QFileInfo fileInfo(path);
        d->filePath = fileInfo.absoluteFilePath();
        d->displayName = fileInfo.fileName();
    } else {
        d->filePath = QString();
        this->setReadOnly(false);
        this->setModified(false);
        d->displayName = tr("untitled");
    }

    emit filePathChanged();
}

bool MarkdownDocument::isNew() const
{
    Q_D(const MarkdownDocument);

    return d->filePath.isNull() || d->filePath.isEmpty();
}

bool MarkdownDocument::isReadOnly() const
{
    Q_D(const MarkdownDocument);

    return d->readOnlyFlag;
}

void MarkdownDocument::setReadOnly(bool readOnly)
{
    Q_D(MarkdownDocument);

    d->readOnlyFlag = readOnly;
}

QDateTime MarkdownDocument::timestamp() const
{
    Q_D(const MarkdownDocument);

    return d->timestamp;
}

void MarkdownDocument::setTimestamp(const QDateTime &timestamp)
{
    Q_D(MarkdownDocument);

    d->timestamp = timestamp;
}


MarkdownAST *MarkdownDocument::markdownAST() const
{
    Q_D(const MarkdownDocument);

    return d->ast;
}

void MarkdownDocument::setMarkdownAST(MarkdownAST *ast)
{
    Q_D(MarkdownDocument);

    d->ast = ast;
}

void MarkdownDocument::notifyTextBlockRemoved(const QTextBlock &block)
{
    emit textBlockRemoved(block.position());
    emit textBlockRemoved(block);
}

void MarkdownDocument::clear()
{
    QTextDocument::clear();
    emit cleared();
}

void MarkdownDocumentPrivate::initializeUntitledDocument()
{
    Q_Q(MarkdownDocument);
    
    QPlainTextDocumentLayout *documentLayout =
        new QPlainTextDocumentLayout(q);
    q->setDocumentLayout(documentLayout);

    this->filePath = QString();
    this->readOnlyFlag = false;
    this->displayName = QObject::tr("untitled");
    this->timestamp = QDateTime::currentDateTime();
    this->ast = nullptr;
}
} // namespace ghostwriter
