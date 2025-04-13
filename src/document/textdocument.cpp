/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "textdocument.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QPlainTextDocumentLayout>
#include <QString>
#include <QTemporaryFile>
#include <QTextDocument>

#include <KFileItem>
#include <KIO/FileCopyJob>
#include <KIO/StatJob>
#include <KJobWidgets>
#include <KNetworkMounts>
#include <KProtocolInfo>

#include "asynctextwriter.h"

namespace ghostwriter
{
using IOError = TextDocument::IOError;
using IOErrorCode = TextDocument::IOErrorCode;
using Result = TextDocument::Result;
using LoadResult = TextDocument::Result;

class TextDocumentPrivate
{
public:
    static IOError mapFileError(QFileDevice::FileError &&err);

    TextDocumentPrivate(TextDocument *q);
    ~TextDocumentPrivate();
    Result loadFile(const QString &path, bool draft, const QString &displayName = QString());
    QString createDisplayName();

    void createDraftBackingFile();
    void initializeWriter(const QString &path);
    void watchFile(const QString &path);
    void removeWatchedFile(const QString &path);

    /*
     * Handles the event where a file as been modified externally on disk.
     */
    void onFileChangedExternally(const QString &path);

    static QFileSystemWatcher *s_fileWatcher;
    static bool s_autoLoadEnabled;
    static QString s_draftLocation;

    TextDocument *q;

    bool m_valid;
    bool m_draft;
    QString m_displayName;
    bool m_backed;
    bool m_modified;
    bool m_writable;
    bool m_saveInProgress;
    QString m_filePath;
    QDateTime m_lastModified;
    QFileInfo m_info;
    AsyncTextWriter *m_writer;
};

QFileSystemWatcher *TextDocumentPrivate::s_fileWatcher = nullptr;
bool TextDocumentPrivate::s_autoLoadEnabled = false;
QString TextDocumentPrivate::s_draftLocation = QString();

bool TextDocument::autoLoadEnabled()
{
    return TextDocumentPrivate::s_autoLoadEnabled;
}

void TextDocument::setAutoLoadEnabled(bool enabled)
{
    TextDocumentPrivate::s_autoLoadEnabled = enabled;
    // TODO: Check if file on disk is out of sync and should be reloaded.
}

TextDocument::Result TextDocument::setDraftLocation(const QString &path)
{
    if (path.isEmpty()) {
        return IOErrorCode(TextDocument::InvalidPathError, tr("Cannot use empty directory path as the draft location."));
    }

    TextDocumentPrivate::s_draftLocation = path;
}

// QString draftLocation();

TextDocument::TextDocument(QObject *parent = nullptr)
    : QTextDocument(parent)
    , d(new TextDocumentPrivate(this))
{
    setDocumentLayout(new QPlainTextDocumentLayout(this));
}

TextDocument::Result TextDocument::create(const QString &draftName, QObject *parent)
{
    auto doc = new TextDocument(parent);
    doc->d->m_displayName = draftName;
    return doc;
}

TextDocument::Result TextDocument::open(const QString &path, QObject *parent)
{
    auto doc = new TextDocument(parent);
    return doc->d->loadFile(path, false);
}

TextDocument::Result TextDocument::openDraft(const QString &draftName, const QString &path, QObject *parent)
{
    auto doc = new TextDocument(parent);
    return doc->d->loadFile(path, true, draftName);
}

TextDocument::~TextDocument()
{
    ;
}

QString TextDocument::fileName() const
{
    return QFileInfo(d->m_filePath).fileName();
}

QString TextDocument::filePath() const
{
    return d->m_filePath;
}

QString TextDocument::absoluteFilePath() const
{
    return QFileInfo(d->m_filePath).absoluteFilePath();
}

QString TextDocument::displayName() const
{
    if (d->m_draft) {
        return d->m_displayName;
    }
}

bool TextDocument::isDraft() const
{
    return d->m_draft;
}

bool TextDocument::backed() const
{
    return d->m_backed;
}

bool TextDocument::modified() const
{
    return d->m_modified;
}

/**
 * Returns true if the document is writable (i.e., not having read only
 * permissions), false otherwise.
 */
bool TextDocument::isWritable() const
{
    return d->m_writable;
}

/**
 * Returns the last in-memory modification time of the document, which is useful when
 * comparing the last modified time of the file represented on disk.
 */
const QDateTime &TextDocument::lastModified() const
{
    return d->m_lastModified;
}

Result TextDocument::reload()
{
    // m_backed = true
    // m_lastModified = QFileInfo time
    // m_draft = DO NOT CHANGE
    // m_filePath = DO NOT CHANGE
    // m_displayName = DO NOT CHANGE
    // m_modified = false;
    // m_writable = derived from QFileInfo;
    // m_saveInProgress = false;
    // m_writer = DO NOT CHANGE
    return this;
}

TextDocument::TextDocument(QObject *parent)
    : QTextDocument(parent)
    , d(new TextDocumentPrivate(this))

{
    setDocumentLayout(new QPlainTextDocumentLayout(this));
}

TextDocumentPrivate::TextDocumentPrivate(TextDocument *q)
    : q(q)
    , m_valid(true)
    , m_draft(true)
    , m_displayName(TextDocument::tr("untitled"))
    , m_backed(false)
    , m_modified(false)
    , m_writable(true)
    , m_saveInProgress(false)
    , m_filePath(QString())
    , m_lastModified(QDateTime())
    , m_writer(nullptr)
{
    if (nullptr == s_fileWatcher) {
        s_fileWatcher = new QFileSystemWatcher();
    }
}

TextDocumentPrivate::~TextDocumentPrivate()
{
    if (nullptr != m_writer) {
        delete m_writer;
        m_writer = nullptr;
    }
}

Result TextDocumentPrivate::loadFile(const QString &path, bool draft, const QString &displayName)
{
    if (path.isEmpty()) {
        return IOErrorCode(TextDocument::InvalidPathError, TextDocument::tr("Cannot open empty file path."));
    }

    QFile file(path);

    if (!file.open(QIODevice::ReadWrite)) {
        return IOErrorCode(mapFileError(file.error()), QTextDocument::tr("Could not open %1: %2").arg(path).arg(file.errorString()));
    }

    // Markdown files need to be in UTF-8 format, so assume that is
    // what the user is opening by default.  Enable auto-detection
    // of of UTF-16 or UTF-32 BOM in case the file isn't UTF-8 encoded.
    //
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream.setAutoDetectUnicode(true);

    QString text = stream.readAll();
    auto ec = mapFileError(file.error());

    if (IOError::NoError != ec) {
        file.close();
        return IOErrorCode(ec, QTextDocument::tr("Could not read %1: %2").arg(path).arg(file.errorString()));
    }

    file.close();
    q->setPlainText(text);

    // If this is the first time a document has been opened/loaded,
    // get the file information for it and begin watching it.
    if (m_info.filePath().isEmpty()) {
        m_info.setCaching(false);
        m_info.setFile(path);

        if (!draft) {
            m_displayName = createDisplayName();
        }

        watchFile(m_info.filePath());
    }

    if (draft) {
        if (displayName.isEmpty()) {
            m_displayName = TextDocument::tr("untitled");
        } else {
            m_displayName = displayName;
        }
    }

    m_filePath = m_info.filePath();
    m_draft = draft;
    m_backed = true;
    m_modified = false;
    m_saveInProgress = false;
    m_writable = m_info.isWritable();
    m_lastModified = m_info.lastModified();

    if (nullptr == m_writer) {
        initializeWriter(m_filePath);
    }

    return q;
}

QString TextDocumentPrivate::createDisplayName()
{
    // SPDX-SnippetBegin
    // SPDX-License-Identifier: MIT
    // SPDX-License-Identifier: GPL-3.0-or-later
    // SPDX-SnippetCopyrightText: 2022 Christoph Cullmann <cullmann@kde.org>
    // SPDX-SnippetCopyrightText: 2022 Waqar Ahmed <waqar.17a@gmail.com>
    // SPDX-SnippetCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
    // SDPX—SnippetName: Utils::niceFileNameWithPath from Kate
    // SPDX-SnippetComment: Function Utils::niceFileNameWithPath that was lifted
    //                      from Kate's ktexteditor_utils.cpp file and modified.

    // we want some filename @ folder output to have chance to keep important stuff even on elide
    // perhaps shorten the path
    const QString homePath = QDir::homePath();

    QString path = m_info.absoluteFilePath();

    // Remove trailing '/', if any.
    if (path.endsWith('/')) {
        path = path.removeLast();
    }

    if (path.startsWith(homePath)) {
        path = QLatin1String("~") + path.right(path.length() - homePath.length());
    }

    return m_info.fileName() + QStringLiteral(" @ ") + path;

    // SPDX-SnippetEnd;
}

void TextDocumentPrivate::createDraftBackingFile()
{
    QFileInfo draftDirInfo(q->draftLocation());

    if (draftDirInfo.isFile()) {
        raiseError(TextDocument::tr("Draft directory location is a file: '%1'").arg(q->draftLocation()));
        return;
    }

    QDir draftDir = draftDirInfo.dir();

    if (!draftDir.mkpath(q->draftLocation())) {
        raiseError(TextDocument::tr("Could not create draft directory '%1'.\n"
                                    "Possible permissions error.")
                       .arg(q->draftLocation()));
        return;
    }

    QString draftPath;
    QFileInfo draftInfo;

    // Make sure draft file name is unique.
    for (int i = 1;; i++) {
        draftPath = draftDir.filePath(q->draftNamePattern().arg(i));
        draftInfo = QFileInfo(draftPath);

        if (!draftInfo.exists()) {
            break;
        }
    }

    // Create the backing draft file.
    QFile draftFile = QFile(draftPath);
    draftPath = draftInfo.fileName();

    if (!draftFile.open(QIODevice::WriteOnly)) {
        raiseError(TextDocument::tr("Could not create draft file '%1'").arg(draftFile.errorString()));
        return;
    }

    draftFile.close();

    if (!draftFile.isWritable()) {
        raiseError(TextDocument::tr("Possible permissions error in making draft file writable: %1").arg(draftPath));

        if (!draftFile.remove()) {
            qCritical() << "Error removing" << draftPath;
            qCritical() << "Could not remove read-only file:" << draftFile.errorString();
        }

        return;
    }

    m_filePath = draftPath;
    m_writable = draftFile.isWritable();
    initializeWriter(draftPath);
    watchFile(draftPath);
}

void TextDocumentPrivate::initializeWriter(const QString &path)
{
    m_writer = new AsyncTextWriter(path, q);

    q->connect(m_writer, &AsyncTextWriter::writeComplete, q, [this]() {
        m_saveInProgress = false;
        m_lastModified = QDateTime::currentDateTime();

        if (!s_fileWatcher->files().contains(m_writer->fileName())) {
            s_fileWatcher->addPath(m_writer->fileName());
        }
    });

    q->connect(m_writer, &AsyncTextWriter::writeError, [this](const QString &err) {
        m_saveInProgress = false;
        emit q->saveError(IOErrorCode(TextDocument::WriteError, err));
    });
}

void TextDocumentPrivate::watchFile(const QString &path)
{
    if (nullptr == s_fileWatcher) {
        s_fileWatcher = new QFileSystemWatcher();
        q->connect(s_fileWatcher, &QFileSystemWatcher::fileChanged, [this](const QString &path) {
            onFileChangedExternally(path);
        });
    }

    s_fileWatcher->addPath(QFileInfo(path).absoluteFilePath());
}

static TextDocument::IOError mapFileError(QFileDevice::FileError &&err)
{
    switch ((int)err) {
    case QFileDevice::NoError:
        return TextDocument::NoError;
    case QFileDevice::ReadError:
        return TextDocument::ReadError;
    case QFileDevice::WriteError:
        return TextDocument::WriteError;
    case QFileDevice::FatalError:
        return TextDocument::FatalError;
    case QFileDevice::ResourceError:
        return TextDocument::ResourceError;
    case QFileDevice::OpenError:
        return TextDocument::OpenError;
    case QFileDevice::AbortError:
        return TextDocument::AbortError;
    case QFileDevice::TimeOutError:
        return TextDocument::TimeOutError;
    case QFileDevice::UnspecifiedError:
        return TextDocument::UnspecifiedError;
    case QFileDevice::RemoveError:
        return TextDocument::RemoveError;
    case QFileDevice::RenameError:
        return TextDocument::RenameError;
    case QFileDevice::PositionError:
        return TextDocument::PositionError;
    case QFileDevice::ResizeError:
        return TextDocument::ResizeError;
    case QFileDevice::PermissionsError:
        return TextDocument::PermissionsError;
    case QFileDevice::CopyError:
        return TextDocument::CopyError;
    default:
        return TextDocument::UnspecifiedError;
    }

} // namespace ghostwriter
