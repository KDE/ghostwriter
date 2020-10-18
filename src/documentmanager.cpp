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

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QFuture>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QPair>
#include <QString>
#include <QtConcurrentRun>
#include <QTextDocument>
#include <QTextStream>
#include <QTimer>

#include "documenthistory.h"
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

    DocumentManager *q_ptr;
    MarkdownDocument *document;
    MarkdownEditor *editor;
    QFutureWatcher<QString> *saveFutureWatcher;
    QFileSystemWatcher *fileWatcher;
    bool fileHistoryEnabled;
    bool createBackupOnSave;

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

    void onSaveCompleted();

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

    void autoSaveFile();
};

const QString DocumentManagerPrivate::FILE_CHOOSER_FILTER =
    QString("%1 (*.md *.markdown *.txt);;%2 (*.txt);;%3 (*)")
    .arg(QObject::tr("Markdown"))
    .arg(QObject::tr("Text"))
    .arg(QObject::tr("All"));

DocumentManager::DocumentManager
(
    MarkdownEditor *editor,
    QObject *parent
) : QObject(parent),
    d_ptr(new DocumentManagerPrivate(this))
{
    d_func()->editor = editor;
    d_func()->fileHistoryEnabled = true;
    d_func()->createBackupOnSave = true;
    d_func()->saveInProgress = false;
    d_func()->autoSaveEnabled = false;
    d_func()->documentModifiedNotifVisible = false;
    d_func()->saveFutureWatcher = new QFutureWatcher<QString>(this);

    d_func()->fileWatcher = new QFileSystemWatcher(this);
    d_func()->document = (MarkdownDocument *) editor->document();

    // Set up auto-save timer to save the file once every minute.
    d_func()->autoSaveTimer = new QTimer(this);
    d_func()->autoSaveTimer->start(60000);

    this->connect
    (
        d_func()->autoSaveTimer,
        &QTimer::timeout,
    [this]() {
        d_func()->autoSaveFile();
    }
    );


    connect
    (
        d_func()->document,
        &MarkdownDocument::modificationChanged,
    [this](bool modified) {
        if
        (
            d_func()->document->isNew()
            || d_func()->document->isReadOnly()
            || !d_func()->autoSaveEnabled
        ) {
            emit documentModifiedChanged(modified);
        }
    }
    );

    this->connect
    (
        d_func()->saveFutureWatcher,
        &QFutureWatcher<QString>::finished,
    [this]() {
        d_func()->onSaveCompleted();
    }
    );

    this->connect
    (
        d_func()->fileWatcher,
        &QFileSystemWatcher::fileChanged,
    [this](const QString & path) {
        d_func()->onFileChangedExternally(path);
    }
    );
}

DocumentManager::~DocumentManager()
{
    d_func()->saveFutureWatcher->waitForFinished();
}

MarkdownDocument *DocumentManager::document() const
{
    return d_func()->document;
}

bool DocumentManager::autoSaveEnabled() const
{
    return d_func()->autoSaveEnabled;
}

bool DocumentManager::fileBackupEnabled() const
{
    return d_func()->createBackupOnSave;
}

void DocumentManager::setFileHistoryEnabled(bool enabled)
{
    d_func()->fileHistoryEnabled = enabled;
}

void DocumentManager::setAutoSaveEnabled(bool enabled)
{
    d_func()->autoSaveEnabled = enabled;

    if (enabled) {
        emit documentModifiedChanged(false);
    } else if (d_func()->document->isModified()) {
        d_func()->document->setModified(false);
    }
}

void DocumentManager::setFileBackupEnabled(bool enabled)
{
    d_func()->createBackupOnSave = enabled;
}

void DocumentManager::open(const QString &filePath)
{
    if (d_func()->checkSaveChanges()) {
        QString path;

        if (!filePath.isNull() && !filePath.isEmpty()) {
            path = filePath;
        } else {
            QString startingDirectory = QString();

            if (!d_func()->document->isNew()) {
                startingDirectory = QFileInfo(d_func()->document->filePath()).dir().path();
            }

            path =
                QFileDialog::getOpenFileName
                (
                    d_func()->editor,
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
                    d_func()->editor,
                    tr("Could not open %1").arg(path),
                    tr("Permission denied.")
                );

                return;
            }

            QString oldFilePath = d_func()->document->filePath();
            int oldCursorPosition = d_func()->editor->textCursor().position();
            bool oldFileWasNew = d_func()-> document->isNew();

            if (!d_func()->loadFile(path)) {
                // The error dialog should already have been displayed
                // in loadFile().
                //
                return;
            } else if (oldFilePath == d_func()->document->filePath()) {
                d_func()->editor->navigateDocument(oldCursorPosition);
            } else if (d_func()->fileHistoryEnabled) {
                if (!oldFileWasNew) {
                    DocumentHistory history;
                    history.add
                    (
                        oldFilePath,
                        oldCursorPosition
                    );
                }

                // Always emit a documentClosed() signal, even if the document
                // was new and untitled.  This is so that if a file from the
                // displayed history is being loaded, its path will be
                // guaranteed to be removed from the "Open Recent" file list
                // displayed to the user (because it's already opened).
                //
                emit documentClosed();
            }
        }
    }
}

void DocumentManager::reopenLastClosedFile()
{
    if (d_func()->fileHistoryEnabled) {
        DocumentHistory history;
        QStringList recentFiles = history.recentFiles(2);

        if (!d_func()->document->isNew()) {
            recentFiles.removeAll(d_func()->document->filePath());
        }

        if (!recentFiles.isEmpty()) {
            open(recentFiles.first());
            emit documentClosed();
        }
    }
}

void DocumentManager::reload()
{
    if (!d_func()->document->isNew()) {
        if (d_func()->document->isModified()) {
            // Prompt user if he wants to save changes.
            int response =
                MessageBoxHelper::question
                (
                    d_func()->editor,
                    tr("The document has been modified."),
                    tr("Discard changes?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No
                );

            if (QMessageBox::No == response) {
                return;
            }
        }

        QTextCursor cursor = d_func()->editor->textCursor();
        int pos = cursor.position();

        if (d_func()->loadFile(d_func()->document->filePath())) {
            cursor.setPosition(pos);
            d_func()->editor->setTextCursor(cursor);
        }
    }
}

void DocumentManager::rename()
{
    if (d_func()->document->isNew()) {
        saveAs();
    } else {
        QString filePath =
            QFileDialog::getSaveFileName
            (
                d_func()->editor,
                tr("Rename File"),
                QString(),
                DocumentManagerPrivate::FILE_CHOOSER_FILTER
            );

        if (!filePath.isNull() && !filePath.isEmpty()) {
            QFile file(d_func()->document->filePath());
            bool success = file.rename(filePath);

            if (!success) {
                MessageBoxHelper::critical
                (
                    d_func()->editor,
                    tr("Failed to rename %1").arg(d_func()->document->filePath()),
                    file.errorString()
                );
                return;
            }

            d_func()->setFilePath(filePath);
            save();
        }
    }
}

bool DocumentManager::save()
{
    if (d_func()->document->isNew() || !d_func()->checkPermissionsBeforeSave()) {
        return this->saveAs();
    } else {
        d_func()->saveFile();
        return true;
    }
}

bool DocumentManager::saveAs()
{
    QString startingDirectory = QString();

    if (!d_func()->document->isNew()) {
        startingDirectory = QFileInfo(d_func()->document->filePath()).dir().path();
    }

    QString filePath =
        QFileDialog::getSaveFileName
        (
            d_func()->editor,
            tr("Save File"),
            startingDirectory,
            DocumentManagerPrivate::FILE_CHOOSER_FILTER
        );

    if (!filePath.isNull() && !filePath.isEmpty()) {
        d_func()->setFilePath(filePath);
        d_func()->saveFile();
        return true;
    }

    return false;
}

bool DocumentManager::close()
{
    if (d_func()->checkSaveChanges()) {
        if (d_func()->saveFutureWatcher->isRunning() || d_func()->saveFutureWatcher->isStarted()) {
            d_func()->saveFutureWatcher->waitForFinished();
        }

        // Get the document's information before closing it out
        // so we can store history information about it.
        //
        QString filePath = d_func()->document->filePath();
        int cursorPosition = d_func()->editor->textCursor().position();
        bool documentIsNew = d_func()->document->isNew();

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
        QTextCursor cursor(d_func()->document);
        cursor.setPosition(0);
        d_func()->editor->setTextCursor(cursor);

        d_func()->document->setPlainText("");
        d_func()->document->clearUndoRedoStacks();
        d_func()->editor->setReadOnly(false);
        d_func()->document->setReadOnly(false);
        d_func()->setFilePath(QString());
        d_func()->document->setModified(false);

        if (d_func()->fileHistoryEnabled && !documentIsNew) {
            DocumentHistory history;
            history.add
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
    ExportDialog exportDialog(d_func()->document);

    connect(&exportDialog, SIGNAL(exportStarted(QString)), this, SIGNAL(operationStarted(QString)));
    connect(&exportDialog, SIGNAL(exportComplete()), this, SIGNAL(operationFinished()));

    exportDialog.exec();
}

void DocumentManagerPrivate::onSaveCompleted()
{
    QString err = this->saveFutureWatcher->result();

    if (!err.isNull() && !err.isEmpty()) {
        MessageBoxHelper::critical
        (
            editor,
            QObject::tr("Error saving %1").arg(this->document->filePath()),
            err
        );
    } else if (!this->fileWatcher->files().contains(this->document->filePath())) {
        fileWatcher->addPath(document->filePath());
    }

    this->document->setTimestamp(QDateTime::currentDateTime());
    this->saveInProgress = false;
}

void DocumentManagerPrivate::onFileChangedExternally(const QString &path)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists()) {
        emit q_func()->documentModifiedChanged(true);

        // Make sure autosave knows the document is modified so it can
        // save it.
        //
        document->setModified(true);
    } else {
        if (fileInfo.isWritable() && document->isReadOnly()) {
            document->setReadOnly(false);

            if (autoSaveEnabled) {
                emit q_func()->documentModifiedChanged(false);
            }
        } else if (!fileInfo.isWritable() && !document->isReadOnly()) {
            document->setReadOnly(true);

            if (document->isModified()) {
                emit q_func()->documentModifiedChanged(true);
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
                    QObject::tr("The document has been modified by another program."),
                    QObject::tr("Would you like to reload the document?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes
                );

            documentModifiedNotifVisible = false;

            if (QMessageBox::Yes == response) {
                q_func()->reload();
            }
        }
    }
}

void DocumentManagerPrivate::autoSaveFile()
{
    if
    (
        this->autoSaveEnabled &&
        !this->document->isNew() &&
        !this->document->isReadOnly() &&
        this->document->isModified()
    ) {
        q_func()->save();
    }
}

void DocumentManagerPrivate::saveFile()
{
    document->setModified(false);
    emit q_func()->documentModifiedChanged(false);

    if
    (
        this->saveFutureWatcher->isRunning() ||
        this->saveFutureWatcher->isStarted()
    ) {
        this->saveFutureWatcher->waitForFinished();
    }

    saveInProgress = true;

    if (fileWatcher->files().contains(document->filePath())) {
        this->fileWatcher->removePath(document->filePath());
    }

    document->setTimestamp(QDateTime::currentDateTime());

    QFuture<QString> future =
        QtConcurrent::run
        (
            this,
            &DocumentManagerPrivate::saveToDisk,
            document->filePath(),
            document->toPlainText(),
            createBackupOnSave
        );

    this->saveFutureWatcher->setFuture(future);
}

bool DocumentManagerPrivate::loadFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QFile inputFile(filePath);

    if (!inputFile.open(QIODevice::ReadOnly)) {
        MessageBoxHelper::critical
        (
            editor,
            QObject::tr("Could not read %1").arg(filePath),
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
    document->setPlainText("");

    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit q_func()->operationStarted(QObject::tr("opening %1").arg(filePath));
    QTextStream inStream(&inputFile);

    // Markdown files need to be in UTF-8 format, so assume that is
    // what the user is opening by default.  Enable autodection
    // of of UTF-16 or UTF-32 BOM in case the file isn't UTF-8 encoded.
    //
    inStream.setCodec("UTF-8");
    inStream.setAutoDetectUnicode(true);

    QString text = inStream.readAll();

    editor->setPlainText(text);
    editor->navigateDocument(0);
    emit q_func()->operationUpdate();

    document->setUndoRedoEnabled(true);

    if (QFile::NoError != inputFile.error()) {
        MessageBoxHelper::critical
        (
            editor,
            QObject::tr("Could not read %1").arg(filePath),
            inputFile.errorString()
        );

        inputFile.close();
        return false;
    }

    inputFile.close();

    if (fileHistoryEnabled) {
        DocumentHistory history;
        editor->navigateDocument(history.cursorPosition(filePath));
    } else {
        editor->navigateDocument(0);
    }

    setFilePath(filePath);
    editor->setReadOnly(false);

    if (!fileInfo.isWritable()) {
        document->setReadOnly(true);
    } else {
        document->setReadOnly(false);
    }

    document->setModified(false);
    document->setTimestamp(fileInfo.lastModified());

    QString watchedFile;

    foreach (watchedFile, fileWatcher->files()) {
        fileWatcher->removePath(watchedFile);
    }

    fileWatcher->addPath(filePath);
    emit q_func()->operationFinished();
    emit q_func()->documentModifiedChanged(false);
    QApplication::restoreOverrideCursor();

    editor->centerCursor();
    emit q_func()->documentLoaded();

    return true;
}

void DocumentManagerPrivate::setFilePath(const QString &filePath)
{
    if (!document->isNew()) {
        fileWatcher->removePath(document->filePath());
    }

    document->setFilePath(filePath);

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

    emit q_func()->documentDisplayNameChanged(document->displayName());
}

bool DocumentManagerPrivate::checkSaveChanges()
{
    if (document->isModified()) {
        if (autoSaveEnabled && !document->isNew() && !document->isReadOnly()) {
            return q_func()->save();
        } else {
            // Prompt user if he wants to save changes.
            QString text;

            if (document->isNew()) {
                text = QObject::tr("File has been modified.");
            } else {
                text = (QObject::tr("%1 has been modified.")
                        .arg(document->displayName()));
            }

            int response =
                MessageBoxHelper::question
                (
                    editor,
                    text,
                    QObject::tr("Would you like to save your changes?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    QMessageBox::Save
                );

            switch (response) {
            case QMessageBox::Save:
                if (document->isNew()) {
                    return q_func()->saveAs();
                } else {
                    return q_func()->save();
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
                QObject::tr("%1 is read only.").arg(document->filePath()),
                QObject::tr("Overwrite protected file?"),
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
                        QObject::tr("Overwrite failed."),
                        QObject::tr("Please save file to another location.")
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

QString DocumentManagerPrivate::saveToDisk
(
    const QString &filePath,
    const QString &text,
    bool createBackup
) const
{
    QString err;

    if (filePath.isNull() || filePath.isEmpty()) {
        return QObject::tr("Null or empty file path provided for writing.");
    }

    QFile outputFile(filePath);

    if (createBackup && outputFile.exists()) {
        backupFile(filePath);
    }

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return outputFile.errorString();
    }

    // Write contents to disk.
    QTextStream outStream(&outputFile);

    // Markdown files need to be in UTF-8, since most Markdown processors
    // (i.e., Pandoc, et. al.) can only read UTF-8 encoded text files.
    //
    outStream.setCodec("UTF-8");
    outStream << text;

    if (QFile::NoError != outputFile.error()) {
        err = outputFile.errorString();
    }

    // Close the file.  All done!
    outputFile.close();
    return err;
}

void DocumentManagerPrivate::backupFile(const QString &filePath) const
{
    QString backupFilePath = filePath + ".backup";
    QFile backupFile(backupFilePath);

    if (backupFile.exists()) {
        if (!backupFile.remove()) {
            qCritical("Could not remove backup file %s before saving: %s",
                      backupFilePath.toLatin1().data(),
                      backupFile.errorString().toLatin1().data());
            return;
        }
    }

    QFile file(filePath);

    if (!file.copy(backupFilePath)) {
        qCritical("Failed to backup file to %s: %s",
                  backupFilePath.toLatin1().data(),
                  file.errorString().toLatin1().data());
    }
}
}
