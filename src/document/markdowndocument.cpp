/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "markdowndocument.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QPlainTextDocumentLayout>
#include <QString>
#include <QTextDocument>

#include <KBackup>

#include "asynctextwriter.h"

namespace ghostwriter
{
class MarkdownDocumentPrivate
{
public:
    MarkdownDocumentPrivate(MarkdownDocument *q)
        : q(q)
    {
    }
    ~MarkdownDocumentPrivate()
    {
    }

    static QFileSystemWatcher *s_fileWatcher;
    static bool s_autoLoadEnabled;

    bool valid;
    bool draft;
    QUrl url;
    bool modified;
    bool writable;
    QDateTime timestamp;
    QString errorString;
    MarkdownAST *ast;

    AsyncTextWriter *writer;

    MarkdownDocument *q;
};

QFileSystemWatcher *MarkdownDocumentPrivate::s_fileWatcher = nullptr;

bool MarkdownDocument::autoLoadEnabled()
{
    return MarkdownDocumentPrivate::s_autoLoadEnabled;
}

void MarkdownDocument::setAutoLoadEnabled(bool enabled)
{
    MarkdownDocumentPrivate::s_autoLoadEnabled = enabled;
}

MarkdownDocument::MarkdownDocument()
{
    d->draft = true;
    d->timestamp = QDateTime::currentDateTime();
    d->writable = true;
    d->valid = true;
    d->modified = false;
}

bool MarkdownDocument::openFile(const QUrl &url)
{
    if (url.isEmpty()) {
        d->errorString = tr("File path is empty");
        d->valid = false;
        return;
    }

    if (!url.isValid()) {
        d->errorString = tr("Invalid file path");
        d->valid = false;
        return;
    }

    QFile file(url.path());

    if (!file.open(QIODevice::ReadWrite)) {
        d->errorString = file.errorString();
        emit loadError(d->errorString);
        return;
    }

    d->url = url;

    // Markdown files need to be in UTF-8 format, so assume that is
    // what the user is opening by default.  Enable auto-detection
    // of of UTF-16 or UTF-32 BOM in case the file isn't UTF-8 encoded.
    //
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream.setAutoDetectUnicode(true);

    QString text = stream.readAll();

    if (QFile::NoError != file.error()) {
        emit loadError(tr("Could not open %1: %2"));
        d->errorString = file.errorString();
        file.close();
        return false;
    }

    file.close();

    setPlainText(text);
    d->modified = false;

    QFileInfo fileInfo(d->url.path());
    d->writable = fileInfo.isWritable();
    d->timestamp = fileInfo.lastModified();

    if (url.isEmpty()) {
        errorString = MarkdownDocument::tr("File path is empty");
        valid = false;
        return;
    }

    if (!url.isValid()) {
        errorString = MarkdownDocument::tr("Invalid file path");
        valid = false;
        return;
    }

    if (MarkdownDocument::backupsEnabled() && url.isLocalFile()) {
        bool success = KBackup::simpleBackupFile(url.path(), MarkdownDocument::backupDir(), MarkdownDocument::backupExtension());

        if (!success) {
            emit q->backupError();
        }

        QFile file(url.path());

        if (!file.open(QIODevice::ReadWrite)) {
            errorString = file.errorString();
            valid = false;
            return;
        }

        this->url = url;

        // Markdown files need to be in UTF-8 format, so assume that is
        // what the user is opening by default.  Enable auto-detection
        // of of UTF-16 or UTF-32 BOM in case the file isn't UTF-8 encoded.
        //
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream.setAutoDetectUnicode(true);

        QString text = stream.readAll();

        if (QFile::NoError != file.error()) {
            errorString = file.errorString();
            file.close();
            valid = false;
            return;
        }

        file.close();

        q->setPlainText(text);
        modified = false;

        QFileInfo fileInfo(url.path());
        writable = fileInfo.isWritable();
        timestamp = fileInfo.lastModified();
    }

    bool MarkdownDocument::openDraft(const QUrl &url)
    {
        if (draft && !url.isLocalFile()) {
            document->d->errorString = tr("Cannot load remote untitled draft");
            document->d->valid = false;
            return;
        }
    }

    MarkdownDocument::MarkdownDocument()
        : QTextDocument(nullptr)
    {
        QPlainTextDocumentLayout *documentLayout = new QPlainTextDocumentLayout(this);

        setDocumentLayout(documentLayout);

        d->valid = true;
        d->draft = true;
        d->url = QUrl();
        d->modified = false;
        d->writable = true;
        d->timestamp = QDateTime::currentDateTime();
        d->errorString = QString();
        d->writer = nullptr;
    }

    MarkdownDocument::~MarkdownDocument()
    {
        ;
    }

    bool MarkdownDocument::isDraft() const;

    QUrl MarkdownDocument::url() const
    {
        return d->url;
    }

    bool MarkdownDocument::modified() const;

    /**s
     * Returns true if there is a conflict with the file contents in memory
     * vs. the contents on disk, false otherwise. This method should be
     * called to see if the file should be reloaded from disk or else
     * overwritten by what is in memory.
     */
    bool MarkdownDocument::hasConflict() const;

    /**
     * Returns true if the document is writeable (i.e., not having read only
     * permissions), false otherwise.
     */
    bool MarkdownDocument::isWritable() const;

    /**
     * Gets the last modification time of the document, which is useful when
     * comparing the last modified time of the file represented on disk.
     */
    QDateTime MarkdownDocument::lastModifiedTime() const;

    /**
     * Renames the file to the given name (without changing its directory).
     */
    void MarkdownDocument::rename(const QString &name);

    /**
     * Saves the document to disk.
     */
    void MarkdownDocument::save();

    /**
     * Saves the document to disk at the given location, changing the document's
     * URL to the one given.
     */
    void MarkdownDocument::saveAs(const QUrl &url);

    /**
     * Saves the document to disk to the given location as a copy without
     * changing the current URL of the document.
     */
    void MarkdownDocument::saveCopyAs(const QUrl &url) const;

    /**
     * Reverts to the backup file on disk that was created prior to opening the
     * document and editing/auto-saving it.
     */
    void MarkdownDocument::revert();

    MarkdownAST *MarkdownDocument::markdownAST() const
    {
        return d->ast;
    }

    void MarkdownDocument::setMarkdownAST(MarkdownAST * ast)
    {
        d->ast = ast;
    }

    QString MarkdownDocument::errorString() const
    {
        return d->errorString;
    }

    MarkdownDocument::MarkdownDocument(const QUrl &url, bool draft)
    {
        MarkdownDocument();
    }

} // namespace ghostwriter
