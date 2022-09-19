/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
    this->displayName = MarkdownDocument::tr("untitled");
    this->timestamp = QDateTime::currentDateTime();
    this->ast = nullptr;
}
} // namespace ghostwriter
