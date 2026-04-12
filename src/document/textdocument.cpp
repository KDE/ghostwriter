/*
 * SPDX-FileCopyrightText: 2014-2026 Megan Conkle <megan.conkle@kdemail.net>
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

#include <QFutureWatcher>

#include "io/textiofactory.h"

namespace ghostwriter
{
class TextDocumentPrivate
{
public:
    enum class SaveKind {
        Save,
        SaveAs
    };

    explicit TextDocumentPrivate(TextDocument *q);
    ~TextDocumentPrivate();

    TextDocument::IOResult
    initialize(const QUrl &url, QStringConverter::Encoding encoding, const QString &draftName, const TextDocument::TextIOFactoryFunction &factory);

    TextDocument::IOResult setFileUrl(const QUrl &url, const QString &draftName, bool isDraft);
    TextDocument::IOResult loadFromUrl(const QUrl &url, const QString &draftName);
    TextDocument::IOResult saveInternal(SaveKind kind);
    void ensureWatcherConnected();
    void watchPath(const QString &path);
    void removeWatchedPath(const QString &path);
    void onFileChangedExternally(const QString &path);
    QString filePath() const;

    static QFileSystemWatcher *s_fileWatcher;

    TextDocument *q;
    bool draft;
    bool closed;
    QString displayName;
    bool readOnly;
    bool saveInProgress;
    bool conflict;
    QUrl fileUrl;
    QDateTime lastModified;
    QStringConverter::Encoding encoding;
    QFileInfo fileInfo;
    std::unique_ptr<TextIO> textIO;
    TextDocument::TextIOFactoryFunction textIOFactory;
    QFutureWatcher<TextIO::WriteResult> *saveWatcher;
    SaveKind saveKind;
};

QFileSystemWatcher *TextDocumentPrivate::s_fileWatcher = nullptr;

TextDocument::TextDocument(QObject *parent)
    : QTextDocument(parent)
    , d(new TextDocumentPrivate(this))
{
    setDocumentLayout(new QPlainTextDocumentLayout(this));
}

TextDocument::~TextDocument() = default;

KResult<TextDocument *, TextIOError>
TextDocument::create(const QString &draftName, const QUrl &draftUrl, QStringConverter::Encoding encoding, QObject *parent, TextIOFactoryFunction textIOFactory)
{
    auto *doc = new TextDocument(parent);

    if (nullptr == doc) {
        return {TextIOError::ResourceError, tr("Out of memory")};
    }

    const auto initResult = doc->d->initialize(draftUrl, encoding, draftName, textIOFactory);

    if (!initResult) {
        delete doc;
        return {initResult.errcode(), initResult.errmsg()};
    }

    const auto writeResult = doc->save();

    if (!writeResult) {
        delete doc;
        return {writeResult.errcode(), writeResult.errmsg()};
    }

    doc->QTextDocument::setModified(false);

    return doc;
}

KResult<TextDocument *, TextIOError>
TextDocument::open(const QUrl &url, QStringConverter::Encoding encoding, QObject *parent, TextIOFactoryFunction textIOFactory)
{
    auto *doc = new TextDocument(parent);

    if (nullptr == doc) {
        return {TextIOError::ResourceError, tr("Out of memory")};
    }

    const auto initResult = doc->d->initialize(url, encoding, QString(), textIOFactory);

    if (!initResult) {
        delete doc;
        return {initResult.errcode(), initResult.errmsg()};
    }

    const auto loadResult = doc->d->loadFromUrl(url, QString());

    if (!loadResult) {
        delete doc;
        return {loadResult.errcode(), loadResult.errmsg()};
    }

    return doc;
}

KResult<TextDocument *, TextIOError>
TextDocument::openDraft(const QString &draftName, const QUrl &url, QStringConverter::Encoding encoding, QObject *parent, TextIOFactoryFunction textIOFactory)
{
    auto *doc = new TextDocument(parent);

    if (nullptr == doc) {
        return {TextIOError::ResourceError, tr("Out of memory")};
    }

    const auto initResult = doc->d->initialize(url, encoding, draftName, textIOFactory);

    if (!initResult) {
        delete doc;
        return {initResult.errcode(), initResult.errmsg()};
    }

    const auto loadResult = doc->d->loadFromUrl(url, draftName);

    if (!loadResult) {
        delete doc;
        return {loadResult.errcode(), loadResult.errmsg()};
    }

    return doc;
}

TextDocument::IOResult TextDocument::close()
{
    if (d->closed) {
        return {};
    }

    if (d->saveInProgress) {
        d->saveWatcher->waitForFinished();
    }

    d->removeWatchedPath(filePath());

    QTextDocument::clear();
    clearUndoRedoStacks();

    d->closed = true;
    d->readOnly = true;
    d->conflict = false;
    d->textIO.reset();

    QTextDocument::setModified(false);

    emit cleared();

    return {};
}

QUrl TextDocument::url() const
{
    return d->fileUrl;
}

QString TextDocument::filePath() const
{
    return d->filePath();
}

QString TextDocument::absoluteFilePath() const
{
    return QFileInfo(filePath()).absoluteFilePath();
}

QString TextDocument::canonicalFilePath() const
{
    return QFileInfo(filePath()).canonicalFilePath();
}

QString TextDocument::displayName() const
{
    return d->displayName;
}

bool TextDocument::isClosed() const
{
    return d->closed;
}

bool TextDocument::isDraft() const
{
    return d->draft;
}

bool TextDocument::modified() const
{
    return QTextDocument::isModified();
}

bool TextDocument::isReadOnly() const
{
    return d->readOnly;
}

bool TextDocument::hasConflict() const
{
    return d->conflict;
}

const QDateTime &TextDocument::lastModified() const
{
    return d->lastModified;
}

TextDocument::IOResult TextDocument::rename(const QString &name) noexcept
{
    if (d->closed) {
        return {TextIOError::FatalError, tr("Document is closed")};
    }

    const QFileInfo currentInfo(filePath());
    const QString destinationPath = currentInfo.dir().absoluteFilePath(name);

    QFile sourceFile(filePath());

    if (!sourceFile.rename(destinationPath)) {
        return {TextIOError::RenameError, tr("Could not rename %1: %2").arg(filePath(), sourceFile.errorString())};
    }

    return d->setFileUrl(QUrl::fromLocalFile(destinationPath), QString(), false);
}

TextDocument::IOResult TextDocument::save()
{
    if (d->closed) {
        return {TextIOError::FatalError, tr("Document is closed")};
    }

    return d->saveInternal(TextDocumentPrivate::SaveKind::Save);
}

TextDocument::IOResult TextDocument::saveAs(const QUrl &url)
{
    if (d->closed) {
        return {TextIOError::FatalError, tr("Document is closed")};
    }

    const bool wasDraft = d->draft;
    const QUrl oldUrl = d->fileUrl;
    const QString oldDisplayName = d->displayName;

    const auto setUrlResult = d->setFileUrl(url, QString(), false);

    if (!setUrlResult) {
        return setUrlResult;
    }

    const auto saveResult = d->saveInternal(TextDocumentPrivate::SaveKind::SaveAs);

    if (!saveResult) {
        d->setFileUrl(oldUrl, oldDisplayName, wasDraft);
        return saveResult;
    }

    return {};
}

TextDocument::IOResult TextDocument::saveCopyAs(const QUrl &url)
{
    if (d->closed) {
        return {TextIOError::FatalError, tr("Document is closed")};
    }

    auto copyTextIO = d->textIOFactory ? d->textIOFactory(url, d->encoding) : nullptr;

    if (!copyTextIO) {
        return {TextIOError::FatalError, tr("Unsupported file URL or file system")};
    }

    const auto result = copyTextIO->write(toPlainText());

    if (!result) {
        const KErrorCode<TextIOError> err(result.errcode(), result.errmsg());
        emit saveCopyAsError(err);
        return {result.errcode(), result.errmsg()};
    }

    return {};
}

TextDocument::IOResult TextDocument::revert(const QUrl &snapshotURL)
{
    if (d->closed) {
        return {TextIOError::FatalError, tr("Document is closed")};
    }

    return d->loadFromUrl(snapshotURL, d->draft ? d->displayName : QString());
}

TextDocument::IOResult TextDocument::reload()
{
    if (d->closed) {
        return {TextIOError::FatalError, tr("Document is closed")};
    }

    return d->loadFromUrl(url(), d->draft ? d->displayName : QString());
}

void TextDocument::setEncoding(QStringConverter::Encoding encoding)
{
    if (d->encoding == encoding) {
        return;
    }

    d->encoding = encoding;

    if (d->textIO) {
        d->textIO->setEncoding(encoding);
    }

    emit encodingChanged(encoding);
}

QStringConverter::Encoding TextDocument::encoding() const
{
    return d->encoding;
}

std::unique_ptr<TextIO> TextDocument::defaultTextIOFactory(const QUrl &url, QStringConverter::Encoding encoding)
{
    TextIOFactory factory;
    return factory.createTextIO(url, encoding);
}

/**********  TextDocumentPrivate **********/

explicit TextDocumentPrivate::TextDocumentPrivate(TextDocument *q)
    : q(q)
    , draft(true)
    , closed(false)
    , displayName(TextDocument::tr("untitled"))
    , readOnly(false)
    , saveInProgress(false)
    , conflict(false)
    , fileUrl(QUrl())
    , lastModified(QDateTime::currentDateTime())
    , encoding(QStringConverter::Utf8)
    , saveKind(SaveKind::Save)
{
    saveWatcher = new QFutureWatcher<TextIO::WriteResult>(q);
}

TextDocumentPrivate::~TextDocumentPrivate() = default;

TextDocument::IOResult TextDocumentPrivate::initialize(const QUrl &url,
                                                       QStringConverter::Encoding encoding,
                                                       const QString &draftName,
                                                       const TextDocument::TextIOFactoryFunction &factory)
{
    if (!url.isValid()) {
        return {TextIOError::FileNotFound, TextDocument::tr("Invalid file URL")};
    }

    textIOFactory = factory;
    this->encoding = encoding;

    K_TRY(setFileUrl(url, draftName, !draftName.isEmpty()));
    ensureWatcherConnected();
    return {};
}

TextDocument::IOResult TextDocumentPrivate::setFileUrl(const QUrl &url, const QString &draftName, bool isDraft)
{
    if (!url.isValid()) {
        return {TextIOError::FileNotFound, TextDocument::tr("Invalid file URL")};
    }

    removeWatchedPath(fileUrl.toLocalFile());

    auto io = textIOFactory ? textIOFactory(url, encoding) : nullptr;

    if (!io) {
        return {TextIOError::FatalError, TextDocument::tr("Unsupported file URL or file system")};
    }

    fileUrl = url;
    textIO = std::move(io);
    fileInfo = QFileInfo(fileUrl.toLocalFile());
    draft = isDraft;

    if (draft) {
        displayName = draftName.isEmpty() ? TextDocument::tr("untitled") : draftName;
    } else if (fileInfo.fileName().isEmpty()) {
        displayName = TextDocument::tr("untitled");
    } else {
        displayName = fileInfo.fileName();
    }

    const bool oldReadOnly = readOnly;
    readOnly = (fileInfo.exists() && fileInfo.isWritable());

    if (fileInfo.exists()) {
        lastModified = fileInfo.lastModified();
    }

    watchPath(fileUrl.toLocalFile());

    emit q->fileUrlChanged(fileUrl);
    emit q->filePathChanged(filePath());
    emit q->displayNameChanged(displayName);

    if (oldReadOnly != readOnly) {
        emit q->permissionsChanged(readOnly);
    }

    return {};
}

TextDocument::IOResult TextDocumentPrivate::loadFromUrl(const QUrl &url, const QString &draftName)
{
    if (closed) {
        return {TextIOError::FatalError, TextDocument::tr("Document is closed")};
    }

    const bool draftMode = !draftName.isEmpty();
    const auto setUrlResult = setFileUrl(url, draftName, draftMode);

    if (!setUrlResult) {
        return setUrlResult;
    }

    const auto readResult = textIO->read();

    if (!readResult) {
        return {readResult.errcode(), readResult.errmsg()};
    }

    q->setPlainText(readResult.value());
    q->QTextDocument::setModified(false);
    q->clearUndoRedoStacks();

    conflict = false;

    if (fileInfo.exists()) {
        lastModified = fileInfo.lastModified();
    } else {
        lastModified = QDateTime::currentDateTime();
    }

    return {};
}

TextDocument::IOResult TextDocumentPrivate::saveInternal(SaveKind kind)
{
    if (closed) {
        return {TextIOError::FatalError, TextDocument::tr("Document is closed")};
    }

    if (readOnly && (SaveKind::Save == kind)) {
        return {TextIOError::PermissionDenied, TextDocument::tr("File is read only")};
    }

    if (!textIO) {
        return {TextIOError::OpenError, TextDocument::tr("No file URL specified")};
    }

    if (saveInProgress) {
        saveWatcher->waitForFinished();
    }

    saveKind = kind;
    saveInProgress = true;

    saveWatcher->setFuture(textIO->writeAsync(q->toPlainText()));
    return {};
}

void TextDocumentPrivate::ensureWatcherConnected()
{
    if (nullptr == s_fileWatcher) {
        s_fileWatcher = new QFileSystemWatcher();
    }

    QObject::connect(s_fileWatcher, &QFileSystemWatcher::fileChanged, q, [this](const QString &path) {
        onFileChangedExternally(path);
    });

    QObject::connect(saveWatcher, &QFutureWatcher<TextIO::WriteResult>::finished, q, [this]() {
        const auto result = saveWatcher->result();
        saveInProgress = false;

        if (!result) {
            const KErrorCode<TextIOError> err(result.errcode(), result.errmsg());

            if (SaveKind::SaveAs == saveKind) {
                emit q->saveAsError(err);
            } else {
                emit q->saveError(err);
            }

            return;
        }

        q->QTextDocument::setModified(false);
        conflict = false;

        fileInfo = QFileInfo(filePath());
        readOnly = (fileInfo.exists() && !fileInfo.isWritable());
        lastModified = fileInfo.lastModified();

        watchPath(filePath());
        emit q->permissionsChanged(!readOnly);
    });
}

void TextDocumentPrivate::watchPath(const QString &path)
{
    if (path.isEmpty() || (nullptr == s_fileWatcher)) {
        return;
    }

    if (!s_fileWatcher->files().contains(path)) {
        s_fileWatcher->addPath(path);
    }
}

void TextDocumentPrivate::removeWatchedPath(const QString &path)
{
    if (path.isEmpty() || (nullptr == s_fileWatcher)) {
        return;
    }

    if (s_fileWatcher->files().contains(path)) {
        s_fileWatcher->removePath(path);
    }
}

void TextDocumentPrivate::onFileChangedExternally(const QString &path)
{
    QFileInfo info(path);

    const bool isReadOnly = !info.isWritable();

    if (isReadOnly != readOnly) {
        readOnly = isReadOnly;
        emit q->permissionsChanged(readOnly);
    }

    // If the file path has changed since we last checked, or if we've already detected a conflict, don't do anything.
    if (path != filePath() || conflict) {
        return;
    }

    if (!info.exists()) {
        conflict = true;
        q->QTextDocument::setModified(true);
        emit q->conflictDetected();
        return;
    }

    if (!saveInProgress && (info.lastModified() > lastModified)) {
        conflict = true;
        emit q->conflictDetected();
    }

    watchPath(path);
}

QString TextDocumentPrivate::filePath() const
{
    return fileUrl.toLocalFile();
}

} // namespace ghostwriter
