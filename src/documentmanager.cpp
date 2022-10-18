/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QPair>
#include <QString>
#include <QStandardPaths>
#include <QTextDocument>
#include <QTextStream>
#include <QTimer>

#include "asynctextwriter.h"
#include "library.h"
#include "documentmanager.h"
#include "exportdialog.h"
#include "exporter.h"
#include "exporterfactory.h"
#include "markdowndocument.h"
#include "markdowneditor.h"
#include "messageboxhelper.h"
#include "themerepository.h"

namespace ghostwriter
{
class DocumentManagerPrivate
{
    Q_DECLARE_PUBLIC(DocumentManager)

public:
    static const QString FILE_CHOOSER_FILTER;

    DocumentManagerPrivate
    (
        DocumentManager *q_ptr
    ) : q_ptr(q_ptr)
    {
        ;
    }

    ~DocumentManagerPrivate()
    {

    }

    const QString draftName = DocumentManager::tr("untitled");

    QString draftLocation;

    DocumentManager *q_ptr;
    MarkdownDocument *document;
    MarkdownEditor *editor;
    QFileSystemWatcher *fileWatcher;
    bool fileHistoryEnabled;
    bool createBackupOnSave;
    AsyncTextWriter *writer;

    /*
    * This flag is used to prevent notifying the user that the document
    * was modified when the user is the one who modified it by saving.
    * See code for onFileChangedExternally() for details.
    */
    bool saveInProgress;

    /*
    * This timer's timeout signal is connected to the autoSaveFile() slot,
    * which saves the document if it can be saved and has been modified.
    */
    QTimer *autoSaveTimer;
    bool autoSaveEnabled;

    /*
    * Boolean flag used to track if the prompt for the file having been
    * externally modified is already displayed and should not be displayed
    * again.
    */
    bool documentModifiedNotifVisible;

    /*
    * Begins asynchronous save operation.  Called by save() and saveAs().
    */
    void saveFile();

    /*
    * Handles the event where a file as been modified externally on disk.
    */
    void onFileChangedExternally(const QString &path);

    /*
    * Loads the document with the file contents at the given path.
    */
    bool loadFile(const QString &filePath);

    /*
    * Sets the file path for the document, such that the file will be
    * monitored for external changes made to it, and the display name
    * for the document updated.
    */
    void setFilePath(const QString &filePath);

    /*
    * Checks if changes need to be saved before an operation
    * can continue.  The user will be prompted to save if
    * necessary, and this method will return true if the
    * operation can proceed.
    */
    bool checkSaveChanges();

    /*
    * Checks if file on the disk is read only.  This method will return
    * true if save operation can proceed.
    */
    bool checkPermissionsBeforeSave();

    /*
    * Saves the given text to the given file path, returning a null
    * string if successful, otherwise an error message.  Note that this
    * method is intended to be run in a separate thread from the main
    * Qt event loop, and should thus never interact with any widgets.
    */
    QString saveToDisk
    (
        const QString &filePath,
        const QString &text,
        bool createBackup
    ) const;

    /*
    * Creates a backup file with a ".backup" extension of the file having
    * the specified path.  Note that this method is intended to be run in
    * a separate thread from the main Qt event loop, and should thus never
    * interact with any widgets.
    */
    void backupFile(const QString &filePath) const;

    /*
    * Handles autosave operation upon autosave timer expiration.
    */
    void autoSaveFile();

    /*
    * Returns true if document is named as "untitled" and located in the
    * configured draft location.
    */
    bool documentIsDraft();

    /*
    * Creates a draft (named "untitled-##.md") document in the configured
    * draft location.
    */
    void createDraft();
};

const QString DocumentManagerPrivate::FILE_CHOOSER_FILTER =
    QString("%1 (*.md *.markdown *.mdown *.mkdn *.mkd *.mdwn *.mdtxt *.mdtext *.text *.Rmd *.txt);;%2 (*.txt);;%3 (*)")
    .arg(DocumentManager::tr("Markdown"))
    .arg(DocumentManager::tr("Text"))
    .arg(DocumentManager::tr("All"));

DocumentManager::DocumentManager
(
    MarkdownEditor *editor,
    QObject *parent
) : QObject(parent),
    d_ptr(new DocumentManagerPrivate(this))
{
    Q_D(DocumentManager);
    
    d->editor = editor;
    d->fileHistoryEnabled = true;
    d->createBackupOnSave = true;
    d->saveInProgress = false;
    d->autoSaveEnabled = false;
    d->documentModifiedNotifVisible = false;

    d->draftLocation =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    d->fileWatcher = new QFileSystemWatcher(this);
    d->document = (MarkdownDocument *) editor->document();

    d->writer = new AsyncTextWriter(d->document->filePath());

    // Markdown files need to be in UTF-8, since most Markdown processors
    // (i.e., Pandoc, et. al.) can only read UTF-8 encoded text files.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    d->writer->setEncoding(QTextCodec::codecForName("UTF-8"));
#else
    d->writer->setEncoding(QStringConverter::Utf8);
#endif

    this->connect(
        d->writer,
        &AsyncTextWriter::writeComplete,
        [d]() {
            d->saveInProgress = false;
            d->document->setTimestamp(QDateTime::currentDateTime());

            if (!d->fileWatcher->files().contains(d->writer->fileName())) {
                d->fileWatcher->addPath(d->writer->fileName());
            }
        }
    );

    this->connect(
        d->writer,
        &AsyncTextWriter::writeError,
        [d](const QString &err) {
            if (!err.isNull() && !err.isEmpty()) {
                MessageBoxHelper::critical(
                    d->editor,
                    DocumentManager::tr("Error saving %1").arg(d->document->filePath()),
                    err
                );
            }

            d->saveInProgress = false;
        }
    );

    // Set up auto-save timer to save the file once every minute.
    d->autoSaveTimer = new QTimer(this);
    d->autoSaveTimer->start(60000);

    this->connect(d->autoSaveTimer,
        &QTimer::timeout,
        [d]() {
            d->autoSaveFile();
        }
    );

    connect(d->document,
        &MarkdownDocument::modificationChanged,
        [this, d](bool modified) {
            if (d->document->isReadOnly()
                    || !d->autoSaveEnabled) {
                emit documentModifiedChanged(modified);
            }

            if (modified
                    && d->autoSaveEnabled 
                    && d->document->isNew()
                    && (!d->document->isEmpty())) {
                d->createDraft();
            }
        }
    );

    this->connect(d->fileWatcher,
        &QFileSystemWatcher::fileChanged,
        [d](const QString & path) {
            d->onFileChangedExternally(path);
        }
    );
}

DocumentManager::~DocumentManager()
{
    ;
}

MarkdownDocument *DocumentManager::document() const
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
        if (d->document->isNew()
            && (!d->document->isEmpty())
            && d->document->isModified()) {
            d->createDraft();
        }

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

void DocumentManager::setFileHistoryEnabled(bool enabled)
{
    Q_D(DocumentManager);
    
    d->fileHistoryEnabled = enabled;
}

void DocumentManager::open(const QString &filePath)
{
    Q_D(DocumentManager);
    
    if (d->checkSaveChanges()) {
        QString path;

        if (!filePath.isNull() && !filePath.isEmpty()) {
            path = filePath;
        } else {
            QString startingDirectory = QString();

            if (!d->document->isNew()) {
                startingDirectory = QFileInfo(d->document->filePath()).dir().path();
            }

            path =
                QFileDialog::getOpenFileName
                (
                    d->editor,
                    tr("Open File"),
                    startingDirectory,
                    DocumentManagerPrivate::FILE_CHOOSER_FILTER
                );
        }

        if (!path.isNull() && !path.isEmpty()) {
            QFileInfo fileInfo(path);

            if (!fileInfo.isReadable()) {
                MessageBoxHelper::critical
                (
                    d->editor,
                    tr("Could not open %1").arg(path),
                    tr("Permission denied.")
                );

                return;
            }

            QString oldFilePath = d->document->filePath();
            int oldCursorPosition = d->editor->textCursor().position();
            bool oldFileWasNew = d-> document->isNew();

            if (!d->loadFile(path)) {
                // The error dialog should already have been displayed
                // in loadFile().
                //
                return;
            } else if (oldFilePath == d->document->filePath()) {
                d->editor->navigateDocument(oldCursorPosition);
            } else if (d->fileHistoryEnabled) {
                if (!oldFileWasNew) {
                    Library library;
                    library.addRecent
                    (
                        oldFilePath,
                        oldCursorPosition
                    );
                }
            }
        }
    }
}

void DocumentManager::reopenLastClosedFile()
{
    Q_D(DocumentManager);
    
    if (d->fileHistoryEnabled) {
        Library library;

        if (!d->document->isNew()) {
            library.removeRecent(d->document->filePath());
        }

        Bookmarks recent = library.recentFiles(1);

        if (!recent.isEmpty()) {
            open(recent.first().filePath());
            emit documentClosed();
        }
    }
}

void DocumentManager::reload()
{
    Q_D(DocumentManager);
    
    if (!d->document->isNew()) {
        if (d->document->isModified()) {
            // Prompt user if he wants to save changes.
            int response =
                MessageBoxHelper::question
                (
                    d->editor,
                    tr("The document has been modified."),
                    tr("Discard changes?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No
                );

            if (QMessageBox::No == response) {
                return;
            }
        }

        QTextCursor cursor = d->editor->textCursor();
        int pos = cursor.position();

        if (d->loadFile(d->document->filePath())) {
            cursor.setPosition(pos);
            d->editor->setTextCursor(cursor);
        }
    }
}

void DocumentManager::rename()
{
    Q_D(DocumentManager);
    
    if (d->document->isNew()) {
        saveAs();
    } else {
        QString filePath =
            QFileDialog::getSaveFileName
            (
                d->editor,
                tr("Rename File"),
                QString(),
                DocumentManagerPrivate::FILE_CHOOSER_FILTER
            );

        if (!filePath.isNull() && !filePath.isEmpty()) {
            bool success;
            QFile file(d->document->filePath());
            QFile destFile(filePath);

            if (destFile.exists()) {
                success = destFile.remove();

                if (!success) {
                    MessageBoxHelper::critical
                    (
                        d->editor,
                        tr("Failed to rename %1").arg(d->document->filePath()),
                        file.errorString()
                    );
                    return;
                }
            }

            success = file.rename(filePath);

            if (!success) {
                MessageBoxHelper::critical
                (
                    d->editor,
                    tr("Failed to rename %1").arg(d->document->filePath()),
                    file.errorString()
                );
                return;
            }

            d->setFilePath(filePath);
            save();
        }
    }
}

bool DocumentManager::saveFile()
{
    Q_D(DocumentManager);

    if (d->documentIsDraft()) {
        return this->saveAs();
    } else {
        return this->save();
    }
}

bool DocumentManager::save()
{
    Q_D(DocumentManager);
    
    if (d->document->isNew() || !d->checkPermissionsBeforeSave()) {
        return this->saveAs();
    } else {
        d->saveFile();
        return true;
    }
}

bool DocumentManager::saveAs()
{
    Q_D(DocumentManager);
    
    QString startingDirectory = QString();

    if (!d->document->isNew()) {
        startingDirectory = QFileInfo(d->document->filePath()).dir().path();
    }

    QString filePath =
        QFileDialog::getSaveFileName
        (
            d->editor,
            tr("Save File"),
            startingDirectory,
            DocumentManagerPrivate::FILE_CHOOSER_FILTER
        );

    if (!filePath.isNull() && !filePath.isEmpty()) {
        if (d->documentIsDraft()) {
            QFile draftFile(d->document->filePath());
            draftFile.remove();

            QString backupFilePath = d->document->filePath() + ".backup";
            QFile backupFile(backupFilePath);
            if (backupFile.exists()) {
                backupFile.remove();
            }
        }
        d->setFilePath(filePath);
        d->saveFile();
        return true;
    }

    return false;
}

bool DocumentManager::close()
{
    Q_D(DocumentManager);
    
    if (d->checkSaveChanges()) {
        if (d->writer->writeInProgress()) {
            d->writer->waitForFinished();
        }

        // Get the document's information before closing it out
        // so we can store history information about it.
        //
        QString filePath = d->document->filePath();
        int cursorPosition = d->editor->textCursor().position();
        bool documentIsNew = d->document->isNew();

        // Set up a new, untitled document.  Note that the document
        // needs to be wiped clean before emitting the documentClosed()
        // signal, because slots accepting this signal may check the
        // new (replacement) document's status.
        //
        // NOTE: Must set editor's text cursor to the beginning
        // of the document before clearing the document/editor
        // of text to prevent a crash in Qt 5.10 on opening or
        // reloading a file if a file has already been previously
        // opened in the editor.
        //
        QTextCursor cursor(d->document);
        cursor.setPosition(0);
        d->editor->setTextCursor(cursor);

        d->document->clear();
        d->document->clearUndoRedoStacks();

        d->editor->setReadOnly(false);
        d->document->setReadOnly(false);
        d->setFilePath(QString());
        d->document->setModified(false);

        if (d->fileHistoryEnabled && 
                (!documentIsNew || this->autoSaveEnabled())) {
            Library library;
            library.addRecent
            (
                filePath,
                cursorPosition
            );
        }

        emit documentClosed();

        return true;
    }

    return false;
}

void DocumentManager::exportFile()
{
    Q_D(DocumentManager);
    
    ExportDialog exportDialog(d->document);

    connect(&exportDialog, SIGNAL(exportStarted(QString)), this, SIGNAL(operationStarted(QString)));
    connect(&exportDialog, SIGNAL(exportComplete()), this, SIGNAL(operationFinished()));

    exportDialog.exec();
}

void DocumentManagerPrivate::onFileChangedExternally(const QString &path)
{
    Q_Q(DocumentManager);

    QFileInfo fileInfo(path);

    if (!fileInfo.exists()) {
        emit q->documentModifiedChanged(true);

        // Make sure autosave knows the document is modified so it can
        // save it.
        //
        document->setModified(true);
    } else {
        if (fileInfo.isWritable() && document->isReadOnly()) {
            document->setReadOnly(false);

            if (autoSaveEnabled) {
                emit q->documentModifiedChanged(false);
            }
        } else if (!fileInfo.isWritable() && !document->isReadOnly()) {
            document->setReadOnly(true);

            if (document->isModified()) {
                emit q->documentModifiedChanged(true);
            }
        }

        // Need to guard against the QFileSystemWatcher from signalling a
        // file change when we're the one who changed the file by saving.
        // Thus, check the saveInProgress flag before prompting.
        //
        if
        (
            !saveInProgress &&
            (fileInfo.lastModified() > document->timestamp()) &&
            !documentModifiedNotifVisible
        ) {
            documentModifiedNotifVisible = true;

            int response =
                MessageBoxHelper::question
                (
                    editor,
                    DocumentManager::tr("The document has been modified by another program."),
                    DocumentManager::tr("Would you like to reload the document?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes
                );

            documentModifiedNotifVisible = false;

            if (QMessageBox::Yes == response) {
                q->reload();
            }
        }
    }
}

void DocumentManagerPrivate::saveFile()
{
    Q_Q(DocumentManager);

    document->setModified(false);
    emit q->documentModifiedChanged(false);
    document->setTimestamp(QDateTime::currentDateTime());
    saveInProgress = true;

    // If backup is enabled, back up the file first.
    if (createBackupOnSave) {
        backupFile(writer->fileName());
    }

    bool status = writer->write(document->toPlainText());

    if (!status) {
        MessageBoxHelper::critical(
            editor,
            DocumentManager::tr("Error saving %1").arg(writer->fileName()),
            DocumentManager::tr("No file path specified")
        );

        saveInProgress = false;
    }
}

bool DocumentManagerPrivate::loadFile(const QString &filePath)
{
    Q_Q(DocumentManager);

    QFileInfo fileInfo(filePath);
    QFile inputFile(filePath);

    if (!inputFile.open(QIODevice::ReadOnly)) {
        MessageBoxHelper::critical(editor,
            DocumentManager::tr("Could not read %1").arg(filePath),
            inputFile.errorString()
        );
        return false;
    }

    // NOTE: Must set editor's text cursor to the beginning
    // of the document before clearing the document/editor
    // of text to prevent a crash in Qt 5.10 on opening or
    // reloading a file if a file has already been previously
    // opened in the editor.
    //
    QTextCursor cursor(document);
    cursor.setPosition(0);
    editor->setTextCursor(cursor);

    document->clearUndoRedoStacks();
    document->setUndoRedoEnabled(false);
    document->clear();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit q->operationStarted(DocumentManager::tr("opening %1").arg(filePath));
    QTextStream inStream(&inputFile);

    // Markdown files need to be in UTF-8 format, so assume that is
    // what the user is opening by default.  Enable autodetection
    // of of UTF-16 or UTF-32 BOM in case the file isn't UTF-8 encoded.
    //
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    inStream.setCodec("UTF-8");
#else
    inStream.setEncoding(QStringConverter::Utf8);
#endif
    inStream.setAutoDetectUnicode(true);

    QString text = inStream.readAll();

    if (QFile::NoError != inputFile.error()) {
        MessageBoxHelper::critical(editor,
            DocumentManager::tr("Could not read %1").arg(filePath),
            inputFile.errorString()
        );

        inputFile.close();
        return false;
    }

    inputFile.close();
    
    setFilePath(filePath);
    editor->setPlainText(text);
    editor->navigateDocument(0);
    emit q->operationUpdate();

    document->setUndoRedoEnabled(true);

    if (fileHistoryEnabled) {
        Library library;
        Bookmark bookmark = library.lookup(filePath);
        editor->navigateDocument(bookmark.cursorPosition());
    } else {
        editor->navigateDocument(0);
    }

    editor->setReadOnly(false);

    if (!fileInfo.isWritable()) {
        document->setReadOnly(true);
    } else {
        document->setReadOnly(false);
    }

    document->setModified(false);
    document->setTimestamp(fileInfo.lastModified());

    for (QString watchedFile : fileWatcher->files()) {
        fileWatcher->removePath(watchedFile);
    }

    fileWatcher->addPath(filePath);
    emit q->operationFinished();
    emit q->documentModifiedChanged(false);
    QApplication::restoreOverrideCursor();

    editor->centerCursor();
    emit q->documentLoaded();

    return true;
}

void DocumentManagerPrivate::setFilePath(const QString &filePath)
{
    Q_Q(DocumentManager);

    if (!document->isNew()) {
        fileWatcher->removePath(document->filePath());
    }

    document->setFilePath(filePath);
    writer->setFileName(filePath);

    if (!filePath.isNull() && !filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);

        if (fileInfo.exists()) {
            document->setReadOnly(!fileInfo.isWritable());
        } else {
            document->setReadOnly(false);
        }
    } else {
        document->setReadOnly(false);
    }

    emit q->documentDisplayNameChanged(document->displayName());
}

bool DocumentManagerPrivate::checkSaveChanges()
{
    Q_Q(DocumentManager);

    if (document->isModified()) {
        if (autoSaveEnabled && !document->isNew() && !document->isReadOnly()) {
            return q->save();
        } else {
            // Prompt user if he wants to save changes.
            QString text;

            if (document->isNew()) {
                text = DocumentManager::tr("File has been modified.");
            } else {
                text = (DocumentManager::tr("%1 has been modified.")
                        .arg(document->displayName()));
            }

            int response =
                MessageBoxHelper::question
                (
                    editor,
                    text,
                    DocumentManager::tr("Would you like to save your changes?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    QMessageBox::Save
                );

            switch (response) {
            case QMessageBox::Save:
                if (document->isNew()) {
                    return q->saveAs();
                } else {
                    return q->save();
                }
                break;
            case QMessageBox::Cancel:
                return false;
            default:
                break;
            }
        }
    }

    return true;
}

bool DocumentManagerPrivate::checkPermissionsBeforeSave()
{
    if (document->isReadOnly()) {
        int response =
            MessageBoxHelper::question
            (
                editor,
                DocumentManager::tr("%1 is read only.").arg(document->filePath()),
                DocumentManager::tr("Overwrite protected file?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
            );

        if (QMessageBox::No == response) {
            return false;
        } else {
            QFile file(document->filePath());
            fileWatcher->removePath(document->filePath());

            if (!file.remove()) {
                if (file.setPermissions(QFile::WriteUser | QFile::ReadUser) && file.remove()) {
                    document->setReadOnly(false);
                    return true;
                } else {
                    MessageBoxHelper::critical
                    (
                        editor,
                        DocumentManager::tr("Overwrite failed."),
                        DocumentManager::tr("Please save file to another location.")
                    );

                    fileWatcher->addPath(document->filePath());
                    return false;
                }
            } else {
                document->setReadOnly(false);
            }
        }
    }

    return true;
}

void DocumentManagerPrivate::backupFile(const QString &filePath) const
{
    QString backupFilePath = filePath + ".backup";
    QFile backupFile(backupFilePath);

    if (backupFile.exists()) {
        if (!backupFile.remove()) {
            MessageBoxHelper::critical(
                this->editor,
                DocumentManager::tr("File backup failed"),
                backupFile.errorString()
            );
            return;
        }
    }

    QFile file(filePath);

    if (file.exists()) {
        if (!file.copy(backupFilePath)) {
            MessageBoxHelper::critical(
                this->editor,
                DocumentManager::tr("File backup failed"),
                file.errorString()
            );
        }
    }
}

void DocumentManagerPrivate::autoSaveFile()
{
    Q_Q(DocumentManager);

    if
    (
        this->autoSaveEnabled &&
        !this->document->isNew() &&
        !this->document->isReadOnly() &&
        this->document->isModified()
    ) {
        q->save();
    }
}

bool DocumentManagerPrivate::documentIsDraft()
{
    if (document->isNew()) {
        return false;
    }

    QFileInfo info(document->filePath());

    return ((info.dir().absolutePath() == draftLocation)
        && (info.baseName().startsWith(draftName)));
}

void DocumentManagerPrivate::createDraft()
{
    if (document->isNew()) {
        int i = 1;
        QString draftPath;

        // Make sure draft file name is unique.
        do {
            draftPath = draftLocation + "/" + draftName + "-" + QString::number(i) + ".md";
            i++;
        } while (QFileInfo(draftPath).exists());

        this->setFilePath(draftPath);
        saveFile();
    }
}

}
