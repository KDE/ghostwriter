/*
 * SPDX-FileCopyrightText: 2014-2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTextCursor>
#include <QTimer>

#include <memory>

#include "export/exportdialog.h"

#include "documentmanager.h"
#include "library.h"
#include "messageboxhelper.h"

namespace ghostwriter
{
class DocumentManagerPrivate
{
    Q_DECLARE_PUBLIC(DocumentManager)

public:
    static const QString FILE_CHOOSER_FILTER;

    explicit DocumentManagerPrivate(DocumentManager *qptr)
        : q_ptr(qptr)
        , document(nullptr)
        , editor(nullptr)
        , fileHistoryEnabled(true)
        , restoreSessionEnabled(true)
        , createBackupOnSave(true)
        , autoSaveTimer(nullptr)
        , autoSaveEnabled(false)
        , documentModifiedNotifVisible(false)
    {
    }

    const QString draftName = DocumentManager::tr("untitled");

    QString draftLocation;
    QString backupLocation;

    DocumentManager *q_ptr;
    TextDocument *document;
    MarkdownEditor *editor;
    bool fileHistoryEnabled;
    bool restoreSessionEnabled;
    bool createBackupOnSave;
    QTimer *autoSaveTimer;
    bool autoSaveEnabled;
    bool documentModifiedNotifVisible;

    bool saveFile();
    bool loadFile(const Bookmark &location);
    bool checkSaveChanges();
    bool checkPermissionsBeforeSave();
    void backupFile(const QString &filePath) const;
    void autoSaveFile();
    bool documentIsDraft() const;
    void createDraft();
    void connectDocumentSignals();
    void disconnectDocumentSignals();
};

const QString DocumentManagerPrivate::FILE_CHOOSER_FILTER =
    QString("%1 (*.md *.markdown *.mdown *.mkdn *.mkd *.mdwn *.mdtxt *.mdtext *.text *.Rmd *.txt);;%2 (*.txt);;%3 (*)")
        .arg(DocumentManager::tr("Markdown"))
        .arg(DocumentManager::tr("Text"))
        .arg(DocumentManager::tr("All"));

DocumentManager::DocumentManager(MarkdownEditor *editor, QObject *parent)
    : QObject(parent)
    , d_ptr(new DocumentManagerPrivate(this))
{
    Q_D(DocumentManager);

    d->editor = editor;
    d->draftLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    d->document = qobject_cast<TextDocument *>(editor->document());

    if (nullptr == d->document) {
        d->document = static_cast<TextDocument *>(editor->document());
    }

    d->connectDocumentSignals();

    d->autoSaveTimer = new QTimer(this);
    d->autoSaveTimer->start(60000);

    connect(d->autoSaveTimer, &QTimer::timeout, this, [d]() {
        d->autoSaveFile();
    });
}

void DocumentManagerPrivate::connectDocumentSignals()
{
    Q_Q(DocumentManager);

    if (nullptr == document) {
        return;
    }

    QObject::connect(document, &QTextDocument::modificationChanged, q, [this, q](bool modified) {
        if (document->isReadOnly() || !autoSaveEnabled) {
            emit q->documentModifiedChanged(modified);
        }
    });

    QObject::connect(document, &TextDocument::displayNameChanged, q, &DocumentManager::documentDisplayNameChanged);

    QObject::connect(document, &TextDocument::saveError, q, [this](const KErrorCode<TextIOError> &err) {
        MessageBoxHelper::critical(editor, DocumentManager::tr("Error saving %1").arg(document->filePath()), err.message());
    });

    QObject::connect(document, &TextDocument::saveAsError, q, [this](const KErrorCode<TextIOError> &err) {
        MessageBoxHelper::critical(editor, DocumentManager::tr("Error saving %1").arg(document->filePath()), err.message());
    });

    QObject::connect(document, &TextDocument::conflictDetected, q, [this, q]() {
        if (documentModifiedNotifVisible) {
            return;
        }

        documentModifiedNotifVisible = true;

        int response = MessageBoxHelper::question(editor,
                                                  DocumentManager::tr("The document has been modified by another program."),
                                                  DocumentManager::tr("Would you like to reload the document?"),
                                                  QMessageBox::Yes | QMessageBox::No,
                                                  QMessageBox::Yes);

        documentModifiedNotifVisible = false;

        if (QMessageBox::Yes == response) {
            q->reload();
        }
    });
}

void DocumentManagerPrivate::disconnectDocumentSignals()
{
    if (nullptr == document) {
        return;
    }

    QObject::disconnect(document, nullptr, nullptr, nullptr);
}

DocumentManager::~DocumentManager() = default;

TextDocument *DocumentManager::document() const
{
    Q_D(const DocumentManager);
    return d->document;
}

bool DocumentManager::autoSaveEnabled() const
{
    Q_D(const DocumentManager);
    return d->autoSaveEnabled;
}

void DocumentManager::setAutoSaveEnabled(bool enabled)
{
    Q_D(DocumentManager);

    d->autoSaveEnabled = enabled;

    if (enabled) {
        emit documentModifiedChanged(false);
    } else if (d->document->isModified()) {
        d->document->setModified(false);
    }
}

bool DocumentManager::fileBackupEnabled() const
{
    Q_D(const DocumentManager);
    return d->createBackupOnSave;
}

void DocumentManager::setFileBackupEnabled(bool enabled)
{
    Q_D(DocumentManager);
    d->createBackupOnSave = enabled;
}

void DocumentManager::setDraftLocation(const QString &directory)
{
    Q_D(DocumentManager);

    QDir draftDir(directory);

    if (!draftDir.exists()) {
        draftDir.mkpath(draftDir.path());
    }

    d->draftLocation = draftDir.absolutePath();
}

void DocumentManager::setBackupLocation(const QString &directory)
{
    Q_D(DocumentManager);

    QDir backupDir(directory);

    if (!backupDir.exists()) {
        backupDir.mkpath(backupDir.path());
    }

    d->backupLocation = backupDir.absolutePath();
}

void DocumentManager::setFileHistoryEnabled(bool enabled)
{
    Q_D(DocumentManager);
    d->fileHistoryEnabled = enabled;
}

void DocumentManager::setRestoreSessionEnabled(bool enabled)
{
    Q_D(DocumentManager);
    d->restoreSessionEnabled = enabled;
}

void DocumentManager::open()
{
    Q_D(DocumentManager);

    if (!d->checkSaveChanges()) {
        return;
    }

    QString startingDirectory;

    if (!d->document->isDraft()) {
        startingDirectory = QFileInfo(d->document->filePath()).dir().path();
    }

    QString path = QFileDialog::getOpenFileName(d->editor, tr("Open File"), startingDirectory, DocumentManagerPrivate::FILE_CHOOSER_FILTER);

    if (path.isEmpty()) {
        return;
    }

    Library library;
    Bookmark location = library.lookup(path);

    if (!location.isValid()) {
        location = Bookmark(path);
    }

    openFileAt(location);
}

void DocumentManager::openFileAt(const Bookmark &location)
{
    Q_D(DocumentManager);

    if (!d->checkSaveChanges() || !location.isValid()) {
        return;
    }

    if (!location.isReadable()) {
        MessageBoxHelper::critical(d->editor, tr("Could not open %1").arg(location.filePath()), tr("Permission denied."));
        return;
    }

    const QString oldFilePath = d->document->filePath();
    const int oldCursorPosition = d->editor->textCursor().position();

    if (!d->loadFile(location)) {
        return;
    }

    if (oldFilePath == d->document->filePath()) {
        d->editor->navigateDocument(oldCursorPosition);
    }

    if (d->restoreSessionEnabled) {
        Library().setLastOpened(location, d->fileHistoryEnabled);
        emit sessionHistoryChanged();
    }
}

void DocumentManager::createUntitled()
{
    Q_D(DocumentManager);

    close();

    if (d->restoreSessionEnabled) {
        Library().setLastOpened(Library::UNTITLED);

        if (d->fileHistoryEnabled) {
            emit sessionHistoryChanged();
        }
    }
}

void DocumentManager::reload()
{
    Q_D(DocumentManager);

    if (d->document->isModified()) {
        int response = MessageBoxHelper::question(d->editor,
                                                  tr("The document has been modified."),
                                                  tr("Discard changes?"),
                                                  QMessageBox::Yes | QMessageBox::No,
                                                  QMessageBox::No);

        if (QMessageBox::No == response) {
            return;
        }
    }

    const int pos = d->editor->textCursor().position();
    const auto result = d->document->reload();

    if (!result) {
        MessageBoxHelper::critical(d->editor, tr("Could not read %1").arg(d->document->filePath()), result.errmsg());
        return;
    }

    QTextCursor cursor = d->editor->textCursor();
    cursor.setPosition(pos);
    d->editor->setTextCursor(cursor);
}

void DocumentManager::rename()
{
    Q_D(DocumentManager);

    if (d->document->isDraft()) {
        saveAs();
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(d->editor,
                                                    tr("Rename File"),
                                                    QFileInfo(d->document->filePath()).absoluteDir().absolutePath(),
                                                    DocumentManagerPrivate::FILE_CHOOSER_FILTER);

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(d->document->filePath());
    QFile destFile(filePath);

    if (destFile.exists() && !destFile.remove()) {
        MessageBoxHelper::critical(d->editor, tr("Failed to rename %1").arg(d->document->filePath()), destFile.errorString());
        return;
    }

    if (!file.rename(filePath)) {
        MessageBoxHelper::critical(d->editor, tr("Failed to rename %1").arg(d->document->filePath()), file.errorString());
        return;
    }

    d->document->setFilePath(filePath);

    if (d->restoreSessionEnabled) {
        Library().updateLastOpened(Bookmark(filePath, d->editor->textCursor().position()));

        if (d->fileHistoryEnabled) {
            emit sessionHistoryChanged();
        }
    }

    save();
}

bool DocumentManager::saveFile()
{
    Q_D(DocumentManager);
    return d->documentIsDraft() ? saveAs() : save();
}

bool DocumentManager::save()
{
    Q_D(DocumentManager);

    if (d->document->isDraft()) {
        return saveAs();
    }

    if (!d->checkPermissionsBeforeSave()) {
        return false;
    }

    return d->saveFile();
}

bool DocumentManager::saveAs()
{
    Q_D(DocumentManager);

    QString startingDirectory;

    if (!d->document->isDraft()) {
        startingDirectory = QFileInfo(d->document->filePath()).dir().path();
    }

    QString filePath = QFileDialog::getSaveFileName(d->editor, tr("Save File"), startingDirectory, DocumentManagerPrivate::FILE_CHOOSER_FILTER);

    if (filePath.isEmpty()) {
        return false;
    }

    if (d->documentIsDraft()) {
        QFile draftFile(d->document->filePath());
        draftFile.remove();

        QFile backupFile(d->document->filePath() + ".backup");
        if (backupFile.exists()) {
            backupFile.remove();
        }
    }

    const auto result = d->document->saveAs(QUrl::fromLocalFile(filePath));

    if (!result) {
        MessageBoxHelper::critical(d->editor, tr("Error saving %1").arg(filePath), result.errmsg());
        return false;
    }

    if (d->restoreSessionEnabled) {
        Library().updateLastOpened(Bookmark(filePath, d->editor->textCursor().position()));
        emit sessionHistoryChanged();
    }

    return true;
}

bool DocumentManager::close()
{
    Q_D(DocumentManager);

    if (!d->checkSaveChanges()) {
        return false;
    }

    if (d->restoreSessionEnabled && !d->document->isDraft()) {
        Bookmark location(d->document->filePath(), d->editor->textCursor().position());
        Library().updateLastOpened(location);
        emit sessionHistoryChanged();
    }

    QTextCursor cursor(d->document);
    cursor.setPosition(0);
    d->editor->setTextCursor(cursor);

    const auto result = d->document->close();

    if (!result) {
        MessageBoxHelper::critical(d->editor, tr("Could not close document"), result.errmsg());
        return false;
    }

    d->editor->setReadOnly(false);
    d->document->setReadOnly(false);
    d->document->setModified(false);

    emit documentClosed();
    return true;
}

void DocumentManager::exportFile()
{
    Q_D(DocumentManager);

    ExportDialog exportDialog(d->document);
    connect(&exportDialog, SIGNAL(exportStarted(QString)), this, SIGNAL(operationStarted(QString)));
    connect(&exportDialog, SIGNAL(exportComplete()), this, SIGNAL(operationFinished()));
    exportDialog.exec();
}

bool DocumentManagerPrivate::saveFile()
{
    Q_Q(DocumentManager);

    if (restoreSessionEnabled) {
        Bookmark location(document->filePath(), editor->textCursor().position());

        if (location.isValid()) {
            Library().updateLastOpened(location);
            emit q->sessionHistoryChanged();
        }
    }

    if (createBackupOnSave) {
        backupFile(document->filePath());
    }

    const auto result = document->save();

    if (!result) {
        MessageBoxHelper::critical(editor, DocumentManager::tr("Error saving %1").arg(document->filePath()), result.errmsg());
        return false;
    }

    return true;
}

bool DocumentManagerPrivate::loadFile(const Bookmark &location)
{
    Q_Q(DocumentManager);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit q->operationStarted(DocumentManager::tr("opening %1").arg(location.filePath()));

    const auto openedResult = TextDocument::open(QUrl::fromLocalFile(location.filePath()), document->encoding(), editor);

    if (!openedResult) {
        QApplication::restoreOverrideCursor();
        MessageBoxHelper::critical(editor, DocumentManager::tr("Could not read %1").arg(location.filePath()), openedResult.errmsg());
        return false;
    }

    std::unique_ptr<TextDocument> openedDocument(openedResult.value());

    disconnectDocumentSignals();

    const auto closeResult = document->close();

    if (!closeResult) {
        QApplication::restoreOverrideCursor();
        MessageBoxHelper::critical(editor, DocumentManager::tr("Could not close document"), closeResult.errmsg());
        return false;
    }

    TextDocument *oldDocument = document;
    document = openedDocument.release();

    editor->setDocument(document);
    connectDocumentSignals();

    delete oldDocument;

    editor->navigateDocument(0);
    emit q->operationUpdate();

    editor->navigateDocument(location.cursorPosition());
    editor->setReadOnly(!location.isWriteable());
    document->setReadOnly(!location.isWriteable());
    document->setModified(false);
    document->setTimestamp(QFileInfo(location.filePath()).lastModified());

    emit q->operationFinished();
    emit q->documentDisplayNameChanged(document->displayName());
    emit q->documentModifiedChanged(false);
    QApplication::restoreOverrideCursor();

    editor->centerCursor();
    emit q->documentLoaded();

    return true;
}

bool DocumentManagerPrivate::checkSaveChanges()
{
    Q_Q(DocumentManager);

    if (!document->isModified()) {
        return true;
    }

    if (autoSaveEnabled && !document->isDraft() && !document->isReadOnly()) {
        return q->save();
    }

    QString text =
        document->isDraft() ? DocumentManager::tr("File has been modified.") : DocumentManager::tr("%1 has been modified.").arg(document->displayName());

    int response = MessageBoxHelper::question(editor,
                                              text,
                                              DocumentManager::tr("Would you like to save your changes?"),
                                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                              QMessageBox::Save);

    switch (response) {
    case QMessageBox::Save:
        return document->isDraft() ? q->saveAs() : q->save();
    case QMessageBox::Cancel:
        return false;
    default:
        return true;
    }
}

bool DocumentManagerPrivate::checkPermissionsBeforeSave()
{
    if (!document->isReadOnly()) {
        return true;
    }

    int response = MessageBoxHelper::question(editor,
                                              DocumentManager::tr("%1 is read only.").arg(document->filePath()),
                                              DocumentManager::tr("Overwrite protected file?"),
                                              QMessageBox::Yes | QMessageBox::No,
                                              QMessageBox::Yes);

    if (QMessageBox::No == response) {
        return false;
    }

    QFile file(document->filePath());

    if (!file.remove()) {
        if (!(file.setPermissions(QFile::WriteUser | QFile::ReadUser) && file.remove())) {
            MessageBoxHelper::critical(editor, DocumentManager::tr("Overwrite failed."), DocumentManager::tr("Please save file to another location."));
            return false;
        }
    }

    document->setReadOnly(false);
    return true;
}

void DocumentManagerPrivate::backupFile(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);
    const QString backupFilePath = backupLocation + QDir::separator() + fileInfo.fileName() + ".backup";

    QDir backupDir(backupLocation);

    if (!backupDir.exists() && !backupDir.mkpath(backupLocation)) {
        MessageBoxHelper::critical(editor,
                                   DocumentManager::tr("File backup failed"),
                                   DocumentManager::tr("Error creating backup location: %1").arg(backupLocation));
        return;
    }

    QFile backupFile(backupFilePath);

    if (backupFile.exists() && !backupFile.remove()) {
        MessageBoxHelper::critical(editor, DocumentManager::tr("File backup failed: Could not replace %1").arg(backupFilePath), backupFile.errorString());
        return;
    }

    QFile source(filePath);

    if (source.exists() && !source.copy(backupFilePath)) {
        MessageBoxHelper::critical(editor,
                                   DocumentManager::tr("File backup failed: Could not copy %1 to %2").arg(filePath, backupFilePath),
                                   source.errorString());
    }
}

void DocumentManagerPrivate::autoSaveFile()
{
    Q_Q(DocumentManager);

    if (autoSaveEnabled && !document->isReadOnly() && document->isModified()) {
        q->save();
    }
}

bool DocumentManagerPrivate::documentIsDraft() const
{
    return document->isDraft();
}

void DocumentManagerPrivate::createDraft()
{
    if (!document->isDraft()) {
        return;
    }

    int i = 1;
    QString draftPath;

    do {
        draftPath = draftLocation + "/" + draftName + "-" + QString::number(i) + ".md";
        i++;
    } while (QFileInfo(draftPath).exists());

    const auto result = document->saveAs(QUrl::fromLocalFile(draftPath));

    if (!result) {
        MessageBoxHelper::critical(editor, DocumentManager::tr("Error saving %1").arg(draftPath), result.errmsg());
    }
}

} // namespace ghostwriter
