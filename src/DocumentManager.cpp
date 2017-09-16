/***********************************************************************
 *
 * Copyright (C) 2014-2017 wereturtle
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

#include <QTextDocument>
#include <QPair>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include <QFileSystemWatcher>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QTimer>
#include <QApplication>

#include "DocumentManager.h"
#include "DocumentHistory.h"
#include "MarkdownEditor.h"
#include "Exporter.h"
#include "ExporterFactory.h"
#include "MarkdownTokenizer.h"
#include "ExportDialog.h"
#include "MessageBoxHelper.h"
#include "ThemeFactory.h"
#include "TextDocument.h"

const QString DocumentManager::FILE_CHOOSER_FILTER =
    QString("%1 (*.md *.markdown *.txt);;%2 (*.txt);;%3 (*)")
        .arg(QObject::tr("Markdown"))
        .arg(QObject::tr("Text"))
        .arg(QObject::tr("All"));

DocumentManager::DocumentManager
(
    MarkdownEditor* editor,
    Outline* outline,
    DocumentStatistics* documentStats,
    SessionStatistics* sessionStats,
    QWidget* parent
)
    : QObject(parent), parentWidget(parent), editor(editor),
        outline(outline), documentStats(documentStats),
        sessionStats(sessionStats), fileHistoryEnabled(true),
        createBackupOnSave(true), saveInProgress(false),
        autoSaveEnabled(false), documentModifiedNotifVisible(false)
{
    saveFutureWatcher = new QFutureWatcher<QString>(this);

    fileWatcher = new QFileSystemWatcher(this);
    document = (TextDocument*) editor->document();

    connect(document, SIGNAL(modificationChanged(bool)), this, SLOT(onDocumentModifiedChanged(bool)));

    // Set up default page layout and page size for printing.
    printer.setPaperSize(QPrinter::Letter);
    printer.setPageMargins(0.5, 0.5, 0.5, 0.5, QPrinter::Inch);

    // Set up auto-save timer to save the file once every minute.
    autoSaveTimer = new QTimer(this);
    autoSaveTimer->start(60000);

    connect
    (
        autoSaveTimer,
        SIGNAL(timeout()),
        this,
        SLOT(autoSaveFile())
    );

    connect(saveFutureWatcher, SIGNAL(finished()), this, SLOT(onSaveCompleted()));
    connect(fileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChangedExternally(QString)));
}

DocumentManager::~DocumentManager()
{
    this->saveFutureWatcher->waitForFinished();
}

TextDocument* DocumentManager::getDocument() const
{
    return this->document;
}

bool DocumentManager::getAutoSaveEnabled() const
{
    return autoSaveEnabled;
}

bool DocumentManager::getFileBackupEnabled() const
{
    return createBackupOnSave;
}

void DocumentManager::setFileHistoryEnabled(bool enabled)
{
    fileHistoryEnabled = enabled;
}

void DocumentManager::setAutoSaveEnabled(bool enabled)
{
    autoSaveEnabled = enabled;

    if (enabled)
    {
        emit documentModifiedChanged(false);
    }
    else if (document->isModified())
    {
        document->setModified(false);
    }
}

void DocumentManager::setFileBackupEnabled(bool enabled)
{
    this->createBackupOnSave = enabled;
}

void DocumentManager::open(const QString& filePath)
{
    if (checkSaveChanges())
    {
        QString path;

        if (!filePath.isNull() && !filePath.isEmpty())
        {
            path = filePath;
        }
        else
        {
            QString startingDirectory = QString();

            if (!document->isNew())
            {
                startingDirectory = QFileInfo(document->getFilePath()).dir().path();
            }

            path =
                QFileDialog::getOpenFileName
                (
                    parentWidget,
                    tr("Open File"),
                    startingDirectory,
                    FILE_CHOOSER_FILTER
                );
        }

        if (!path.isNull() && !path.isEmpty())
        {
            QFileInfo fileInfo(path);

            if (!fileInfo.isReadable())
            {
                MessageBoxHelper::critical
                (
                    parentWidget,
                    tr("Could not open %1").arg(path),
                    tr("Permission denied.")
                );

                return;
            }

            QString oldFilePath = document->getFilePath();
            int oldCursorPosition = editor->textCursor().position();
            bool oldFileWasNew = document->isNew();

            if (!loadFile(path))
            {
                // The error dialog should already have been displayed
                // in loadFile().
                //
                return;
            }
            else if (oldFilePath == document->getFilePath())
            {
                editor->navigateDocument(oldCursorPosition);
            }
            else if (fileHistoryEnabled)
            {
                if (!oldFileWasNew)
                {
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
    if (fileHistoryEnabled)
    {
        DocumentHistory history;
        QStringList recentFiles = history.getRecentFiles(2);

        if (!document->isNew())
        {
            recentFiles.removeAll(document->getFilePath());
        }

        if (!recentFiles.isEmpty())
        {
            open(recentFiles.first());
            emit documentClosed();
        }
    }
}

void DocumentManager::reload()
{
    if (!document->isNew())
    {
        if (document->isModified())
        {
            // Prompt user if he wants to save changes.
            int response =
                MessageBoxHelper::question
                (
                    parentWidget,
                    tr("The document has been modified."),
                    tr("Discard changes?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No
                );

            if (QMessageBox::No == response)
            {
                return;
            }
        }

        QTextCursor cursor = editor->textCursor();
        int pos = cursor.position();

        if (loadFile(document->getFilePath()))
        {
            cursor.setPosition(pos);
            editor->setTextCursor(cursor);
        }
    }
}

void DocumentManager::rename()
{
    if (document->isNew())
    {
        saveAs();
    }
    else
    {
        QString filePath =
            QFileDialog::getSaveFileName
            (
                parentWidget,
                tr("Rename File"),
                QString(),
                FILE_CHOOSER_FILTER
            );

        if (!filePath.isNull() && !filePath.isEmpty())
        {
            QFile file(document->getFilePath());
            bool success = file.rename(filePath);

            if (!success)
            {
                MessageBoxHelper::critical
                (
                    parentWidget,
                    tr("Failed to rename %1").arg(document->getFilePath()),
                    file.errorString()
                );
                return;
            }

            setFilePath(filePath);
            save();
        }
    }
}

bool DocumentManager::save()
{
    if (document->isNew() || !checkPermissionsBeforeSave())
    {
        return saveAs();
    }
    else
    {
        saveFile();
        return true;
    }
}

bool DocumentManager::saveAs()
{
    QString startingDirectory = QString();

    if (!document->isNew())
    {
        startingDirectory = QFileInfo(document->getFilePath()).dir().path();
    }

    QString filePath =
        QFileDialog::getSaveFileName
        (
            parentWidget,
            tr("Save File"),
            startingDirectory,
            FILE_CHOOSER_FILTER
        );

    if (!filePath.isNull() && !filePath.isEmpty())
    {
        setFilePath(filePath);
        saveFile();
        return true;
    }

    return false;
}

bool DocumentManager::close()
{
    if (checkSaveChanges())
    {
        if (saveFutureWatcher->isRunning() || saveFutureWatcher->isStarted())
        {
            this->saveFutureWatcher->waitForFinished();
        }

        // Get the document's information before closing it out
        // so we can store history information about it.
        //
        QString filePath = document->getFilePath();
        int cursorPosition = editor->textCursor().position();
        bool documentIsNew = document->isNew();

        // Set up a new, untitled document.  Note that the document
        // needs to be wiped clean before emitting the documentClosed()
        // signal, because slots accepting this signal may check the
        // new (replacement) document's status.
        //
        document->setPlainText("");
        document->clearUndoRedoStacks();
        editor->setReadOnly(false);
        document->setReadOnly(false);
        setFilePath(QString());
        document->setModified(false);
        sessionStats->startNewSession(0);

        if (fileHistoryEnabled && !documentIsNew)
        {
            DocumentHistory history;
            history.add
            (
                filePath,
                cursorPosition
            );

            emit documentClosed();
        }

        return true;
    }

    return false;
}

void DocumentManager::exportFile()
{
    ExportDialog exportDialog(document);

    connect(&exportDialog, SIGNAL(exportStarted(QString)), this, SIGNAL(operationStarted(QString)));
    connect(&exportDialog, SIGNAL(exportComplete()), this, SIGNAL(operationFinished()));

    exportDialog.exec();
}

void DocumentManager::printPreview()
{
    QPrintPreviewDialog printPreviewDialog(&printer, parentWidget);
    connect(&printPreviewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printFileToPrinter(QPrinter*)));
    printPreviewDialog.exec();
}

void DocumentManager::print()
{
    QPrintDialog printDialog(&printer, parentWidget);

    if (printDialog.exec() == QDialog::Accepted)
    {
        printFileToPrinter(&printer);
    }
}

void DocumentManager::onDocumentModifiedChanged(bool modified)
{
    if (document->isNew() || document->isReadOnly() || !autoSaveEnabled)
    {
        emit documentModifiedChanged(modified);
    }
}

void DocumentManager::onSaveCompleted()
{
    QString err = this->saveFutureWatcher->result();

    if (!err.isNull() && !err.isEmpty())
    {
        MessageBoxHelper::critical
        (
            parentWidget,
            tr("Error saving %1").arg(document->getFilePath()),
            err
        );
    }
    else if (!fileWatcher->files().contains(document->getFilePath()))
    {
        fileWatcher->addPath(document->getFilePath());
    }

    document->setTimestamp(QDateTime::currentDateTime());
    saveInProgress = false;
}

void DocumentManager::onFileChangedExternally(const QString& path)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
    {
        emit documentModifiedChanged(true);

        // Make sure autosave knows the document is modified so it can
        // save it.
        //
        document->setModified(true);
    }
    else
    {
        if (fileInfo.isWritable() && document->isReadOnly())
        {
            document->setReadOnly(false);

            if (autoSaveEnabled)
            {
                emit documentModifiedChanged(false);
            }
        }
        else if (!fileInfo.isWritable() && !document->isReadOnly())
        {
            document->setReadOnly(true);

            if (document->isModified())
            {
                emit documentModifiedChanged(true);
            }
        }

        // Need to guard against the QFileSystemWatcher from signalling a
        // file change when we're the one who changed the file by saving.
        // Thus, check the saveInProgress flag before prompting.
        //
        if
        (
            !saveInProgress &&
            (fileInfo.lastModified() > document->getTimestamp()) &&
            !documentModifiedNotifVisible
        )
        {
            documentModifiedNotifVisible = true;

            int response =
                MessageBoxHelper::question
                (
                    parentWidget,
                    tr("The document has been modified by another program."),
                    tr("Would you like to reload the document?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes
                );

            documentModifiedNotifVisible = false;

            if (QMessageBox::Yes == response)
            {
                reload();
            }
        }
    }
}

void DocumentManager::printFileToPrinter(QPrinter* printer)
{
    QString text = editor->document()->toPlainText();
    TextDocument doc(text);
    MarkdownEditor e(&doc);

    Theme printerTheme = ThemeFactory::getInstance()->getPrinterFriendlyTheme();
    e.setColorScheme
    (
        printerTheme.getDefaultTextColor(),
        printerTheme.getBackgroundColor(),
        printerTheme.getMarkupColor(),
        printerTheme.getLinkColor(),
        printerTheme.getHeadingColor(),
        printerTheme.getEmphasisColor(),
        printerTheme.getBlockquoteColor(),
        printerTheme.getCodeColor(),
        printerTheme.getSpellingErrorColor()
    );
    e.setSpellCheckEnabled(false);
    e.setFont(editor->font().family(), editor->font().pointSizeF());
    doc.print(printer);
}

void DocumentManager::autoSaveFile()
{
    if
    (
        autoSaveEnabled &&
        !document->isNew() &&
        !document->isReadOnly() &&
        document->isModified()
    )
    {
        save();
    }
}

void DocumentManager::saveFile()
{
    document->setModified(false);
    emit documentModifiedChanged(false);

    if
    (
        this->saveFutureWatcher->isRunning() ||
        this->saveFutureWatcher->isStarted()
    )
    {
        this->saveFutureWatcher->waitForFinished();
    }

    saveInProgress = true;

    if (fileWatcher->files().contains(document->getFilePath()))
    {
        this->fileWatcher->removePath(document->getFilePath());
    }

    document->setTimestamp(QDateTime::currentDateTime());

    QFuture<QString> future =
        QtConcurrent::run
        (
            this,
            &DocumentManager::saveToDisk,
            document->getFilePath(),
            document->toPlainText(),
            createBackupOnSave
        );

    this->saveFutureWatcher->setFuture(future);
}

bool DocumentManager::loadFile(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QFile inputFile(filePath);

    if (!inputFile.open(QIODevice::ReadOnly))
    {
        MessageBoxHelper::critical
        (
            parentWidget,
            tr("Could not read %1").arg(filePath),
            inputFile.errorString()
        );
        return false;
    }

    outline->clear();
    document->clearUndoRedoStacks();
    document->setUndoRedoEnabled(false);
    document->setPlainText("");

    QTextCursor cursor(document);
    cursor.setPosition(0);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit operationStarted(tr("opening %1").arg(filePath));
    QTextStream inStream(&inputFile);

    // Markdown files need to be in UTF-8 format, so assume that is
    // what the user is opening by default.  Enable autodection
    // of of UTF-16 or UTF-32 BOM in case the file isn't UTF-8 encoded.
    //
    inStream.setCodec("UTF-8");
    inStream.setAutoDetectUnicode(true);

    QString text = inStream.read(2048L);

    int count = 0;

    while (!text.isNull())
    {
        cursor.insertText(text);

        // Front load enough text to show the beginning of the document
        // in the editor, and emit the operationUpdate signal to ensure
        // the GUI is updated to show the front-loaded text.
        //
        // Don't put in too much text, or else the application will not
        // be able to open the document as quickly.
        //
        if (count < 5)
        {
            editor->navigateDocument(0);
            emit operationUpdate();
        }

        count++;
        text = inStream.read(2048L);
    }

    document->setUndoRedoEnabled(true);

    if (QFile::NoError != inputFile.error())
    {
        MessageBoxHelper::critical
        (
            parentWidget,
            tr("Could not read %1").arg(filePath),
            inputFile.errorString()
        );

        inputFile.close();
        return false;
    }

    inputFile.close();

    if (fileHistoryEnabled)
    {
        DocumentHistory history;
        editor->navigateDocument(history.getCursorPosition(filePath));
    }
    else
    {
        editor->navigateDocument(0);
    }

    setFilePath(filePath);
    editor->setReadOnly(false);

    if (!fileInfo.isWritable())
    {
        document->setReadOnly(true);
    }
    else
    {
        document->setReadOnly(false);
    }

    document->setModified(false);
    document->setTimestamp(fileInfo.lastModified());

    QString watchedFile;

    foreach (watchedFile, fileWatcher->files())
    {
        fileWatcher->removePath(watchedFile);
    }

    fileWatcher->addPath(filePath);
    emit operationFinished();
    emit documentModifiedChanged(false);
    QApplication::restoreOverrideCursor();

    editor->centerCursor();
    sessionStats->startNewSession(documentStats->getWordCount());

    return true;
}

void DocumentManager::setFilePath(const QString& filePath)
{
    if (!document->isNew())
    {
        fileWatcher->removePath(document->getFilePath());
    }

    document->setFilePath(filePath);

    if (!filePath.isNull() && !filePath.isEmpty())
    {
        QFileInfo fileInfo(filePath);

        if (fileInfo.exists())
        {
            document->setReadOnly(!fileInfo.isWritable());
        }
        else
        {
            document->setReadOnly(false);
        }
    }
    else
    {
        document->setReadOnly(false);
    }

    emit documentDisplayNameChanged(document->getDisplayName());
}

bool DocumentManager::checkSaveChanges()
{
    if (document->isModified())
    {
        if (autoSaveEnabled && !document->isNew() && !document->isReadOnly())
        {
            return save();
        }
        else
        {
            // Prompt user if he wants to save changes.
            QString text;

            if (document->isNew())
            {
                text = tr("File has been modified.");
            }
            else
            {
                text = (tr("%1 has been modified.")
                    .arg(document->getDisplayName()));
            }

            int response =
                MessageBoxHelper::question
                (
                    parentWidget,
                    text,
                    tr("Would you like to save your changes?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    QMessageBox::Save
                );

            switch (response)
            {
                case QMessageBox::Save:
                    if (document->isNew())
                    {
                        return saveAs();
                    }
                    else
                    {
                        return save();
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

bool DocumentManager::checkPermissionsBeforeSave()
{
    if (document->isReadOnly())
    {
        int response =
            MessageBoxHelper::question
            (
                parentWidget,
                tr("%1 is read only.").arg(document->getFilePath()),
                tr("Overwrite protected file?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
            );

        if (QMessageBox::No == response)
        {
            return false;
        }
        else
        {
            QFile file(document->getFilePath());
            fileWatcher->removePath(document->getFilePath());

            if (!file.remove())
            {
                if (file.setPermissions(QFile::WriteUser | QFile::ReadUser) && file.remove())
                {
                    document->setReadOnly(false);
                    return true;
                }
                else
                {
                    MessageBoxHelper::critical
                    (
                        parentWidget,
                        tr("Overwrite failed."),
                        tr("Please save file to another location.")
                    );

                    fileWatcher->addPath(document->getFilePath());
                    return false;
                }
            }
            else
            {
                document->setReadOnly(false);
            }
        }
    }

    return true;
}

QString DocumentManager::saveToDisk
(
    const QString& filePath,
    const QString& text,
    bool createBackup
) const
{
    QString err;

    if (filePath.isNull() || filePath.isEmpty())
    {
        return QObject::tr("Null or empty file path provided for writing.");
    }

    QFile outputFile(filePath);

    if (createBackup && outputFile.exists())
    {
        backupFile(filePath);
    }

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return outputFile.errorString();
    }

    // Write contents to disk.
    QTextStream outStream(&outputFile);

    // Markdown files need to be in UTF-8, since most Markdown processors
    // (i.e., Pandoc, et. al.) can only read UTF-8 encoded text files.
    //
    outStream.setCodec("UTF-8");
    outStream << text;

    if (QFile::NoError != outputFile.error())
    {
        err = outputFile.errorString();
    }

    // Close the file.  All done!
    outputFile.close();
    return err;
}

void DocumentManager::backupFile(const QString& filePath) const
{
    QString backupFilePath = filePath + ".backup";
    QFile backupFile(backupFilePath);

    if (backupFile.exists())
    {
        if (!backupFile.remove())
        {
            qCritical("Could not remove backup file %s before saving: %s",
                backupFilePath.toLatin1().data(),
            backupFile.errorString().toLatin1().data());
            return;
        }
    }

    QFile file(filePath);

    if (!file.rename(backupFilePath))
    {
        qCritical("Failed to backup file to %s: %s",
            backupFilePath.toLatin1().data(),
            file.errorString().toLatin1().data());
    }
}
