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

#include "io/localtextio.h"
#include "io/netsharetextio.h"
#include "io/textio.h"

#include "textdocument_p.h"

namespace ghostwriter
{
class TextDocumentPrivate
{
public:
    TextDocumentPrivate(TextDocument *q);
    ~TextDocumentPrivate();

    TextDocument::IOResult
    initialize(const QUrl &url, const QStringConverter::Encoding encoding, const QString &draftName, TextDocument::TextIOFactoryFunction textIOFactory);
    TextDocument::IOResult loadFile(const QUrl &url);

    TextDocument::IOResult createDraftBackingFile(const QUrl &draftUrl);
    void watchFile(const QString &path);
    void removeWatchedFile(const QString &path);

    /*
     * Handles the event where a file has been modified externally on disk.
     */
    void onFileChangedExternally(const QString &path);

    static QFileSystemWatcher *s_fileWatcher;

    TextDocument *q;

    bool m_draft;
    QString m_displayName;
    bool m_modified;
    bool m_writable;
    bool m_saveInProgress;
    QUrl m_fileUrl;
    QDateTime m_lastModified;
    QStringConverter::Encoding m_encoding;
    QFileInfo m_fileInfo;
    std::unique_ptr<TextIO> m_textIO;
    TextDocument::TextIOFactoryFunction m_textIOFactory;
};

QFileSystemWatcher *TextDocumentPrivate::s_fileWatcher = nullptr;

// TextDocument::TextDocument(const QUrl &url, const QString &contents, const std::unique_ptr<TextIO> &textIO, const QString &draftName = QString(), QObject
// *parent = nullptr)
//     : QTextDocument(parent)
//     , d(new TextDocumentPrivate(this))
// {
//     setDocumentLayout(new QPlainTextDocumentLayout(this));
`
    //     d->m_textIO = textIO;
    //     setPlainText(contents);
    //     d->m_filePath = url.toLocalFile();
    //     d->m_draft = draftName.isEmpty() ? true : false;
    //     d->m_displayName =  d->m_draft ? draftName : QFileInfo(d->m_filePath).baseName();
    // }

    KResult<TextDocument *, TextIOError> TextDocument::create(const QString &draftName,
                                                              const QUrl &draftUrl,
                                                              QStringConverter::Encoding encoding,
                                                              QObject *parent,
                                                              TextIOFactoryFunction textIOFactory)
{
    TextDocument *doc = new TextDocument(parent);

    if (!doc) {
        return {TextIOError::ResourceError, TextDocument::tr("Out of memory")};
    }

    // TODO: handle logic for URL already existing (should error!)
    // TODO: create emtpy temp draft document
    doc->d->initialize(draftUrl, encoding, draftName, textIOFactory);
}

KResult<TextDocument *, TextIOError>
TextDocument::open(const QUrl &url, QStringConverter::Encoding encoding, QObject *parent, TextIOFactoryFunction textIOFactory)
{
    TextDocument *doc = new TextDocument(parent);

    if (!doc) {
        return {TextIOError::ResourceError, TextDocument::tr("Out of memory")};
    }

    doc->d->initialize(url, encoding, QString(), textIOFactory);
}

KResult<TextDocument *, TextIOError>
TextDocument::openDraft(const QString &draftName, const QUrl &url, QStringConverter::Encoding encoding, QObject *parent, TextIOFactoryFunction textIOFactory)
{
    TextDocument *doc = new TextDocument(parent);

    if (!doc) {
        return {TextIOError::ResourceError, TextDocument::tr("Out of memory")};
    }

    doc->d->initialize(url, encoding, draftName, textIOFactory);
}

TextDocument::~TextDocument()
{
    ;
}

QUrl TextDocument::url() const
{
    return d->m_fileUrl;
}

QString TextDocument::filePath() const
{
    return d->m_fileInfo.filePath();
}

QString TextDocument::absoluteFilePath() const
{
    return d->m_fileInfo.absoluteFilePath();
}

QString TextDocument::displayName() const
{
    return d->m_displayName;
}

bool TextDocument::isDraft() const
{
    return d->m_draft;
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

std::unique_ptr<TextIO> TextDocument::defaultTextIOFactory(const QUrl &fileUrl, QStringConverter::Encoding encoding)
{
    if (NetShareTextIO::canHandle(fileUrl)) {
        return std::make_unique<NetShareTextIO>(fileUrl, encoding);
    } else if (LocalTextIO::canHandle(fileUrl)) {
        return std::make_unique<LocalTextIO>(fileUrl, encoding);
    }
    // Add more handlers here as needed.
    return nullptr;
}

TextDocumentPrivate::TextDocumentPrivate(TextDocument *q)
    : q(q)
    , m_draft(true)
    , m_displayName(TextDocument::tr("untitled"))
    , m_modified(false)
    , m_writable(true)
    , m_saveInProgress(false)
    , m_fileUrl(QUrl())
    , m_lastModified(QDateTime())
    , m_textIO(nullptr)
{
    ;
}

TextDocumentPrivate::~TextDocumentPrivate()
{
    if (nullptr != m_writer) {
        delete m_writer;
        m_writer = nullptr;
    }
}

TextDocument::IOResult TextDocumentPrivate::initialize(const QUrl &url,
                                                       const QStringConverter::Encoding encoding,
                                                       const QString &draftName,
                                                       TextDocument::TextIOFactoryFunction textIOFactory)
{
    if (!url.isValid()) {
        return {TextIOError::FileNotFound, TextDocument::tr("Invalid file path")};
    }

    m_fileUrl = url;
    m_encoding = encoding;
    m_draft = !draftName.isEmpty();
    m_textIO = textIOFactory(url, encoding);

    if (nullptr == m_textIO) {
        return {TextIOError::FatalError, TextDocument::tr("Unsupported file system or URL")};
    }

    m_textIOFactory = textIOFactory;
    m_fileInfo = QFileInfo(m_fileUrl.toLocalFile());
    m_writable = m_fileInfo.isWritable();
    m_lastModified = m_fileInfo.lastModified();

    if (m_draft) {
        m_displayName = draftName;
    } else {
        m_displayName = m_fileInfo.fileName();
    }

    if (nullptr == s_fileWatcher) {
        s_fileWatcher = new QFileSystemWatcher();
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

// void TextDocumentPrivate::watchFile(const QString &path)
// {
//     if (nullptr == s_fileWatcher) {
//         s_fileWatcher = new QFileSystemWatcher();
//         q->connect(s_fileWatcher, &QFileSystemWatcher::fileChanged, [this](const QString &path) {
//             onFileChangedExternally(path);
//         });
//     }

//     s_fileWatcher->addPath(QFileInfo(path).absoluteFilePath());
// }

} // namespace ghostwriter
