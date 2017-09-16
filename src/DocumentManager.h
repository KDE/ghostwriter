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

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QWidget>
#include <QFutureWatcher>
#include <QPrinter>

#include "MarkdownEditor.h"
#include "Outline.h"
#include "DocumentStatistics.h"
#include "SessionStatistics.h"
#include "TextDocument.h"

class QFileSystemWatcher;

/**
 * Manages the life-cycle of a document, facilitating user interaction for
 * opening, closing, saving, etc.
 */
class DocumentManager : public QObject
{
    Q_OBJECT

    public:
        /**
         * Constructor.  Takes MarkdownEditor as a parameter, which is used
         * to display the current document to the user.  Also takes the
         * document statistics as a parameter in order to reset the statistics
         * when a new file is loaded.
         */
        DocumentManager
        (
            MarkdownEditor* editor,
            Outline* outline,
            DocumentStatistics* documentStats,
            SessionStatistics* sessionStats,
            QWidget* parent = 0
        );

        /**
         * Destructor.
         */
        virtual ~DocumentManager();

        /**
         * Gets the current document that is opened.
         */
        TextDocument* getDocument() const;

        /**
         * Gets whether auto-save is enabled.
         */
        bool getAutoSaveEnabled() const;

        /**
         * Gets whether automatic file backup (i.e., creation of a .backup
         * file at regular intervals), is enabled.
         */
        bool getFileBackupEnabled() const;

        /**
         * Gets whether tracking the recent file history is enabled.
         */
        void setFileHistoryEnabled(bool enabled);

    signals:
        /**
         * Emitted when the document's display name changes, which is useful
         * for updating the editor's containing window or tab to have the new
         * document display name.
         */
        void documentDisplayNameChanged(const QString& displayName);

        /**
         * Emitted when the document's modification state changes.  The
         * modified parameter will be true if the document has been modified.
         */
        void documentModifiedChanged(bool modified);

        /**
         * Emitted when an operation on the document has started, such as
         * when a document is being loaded into the editor either from being
         * opened or reloaded from disk.  Connect to this signal to notify
         * the user of a possibly long operation for the document, such as
         * with a progress bar.  The description parameter will contain
         * descriptive text to display to the user regarding the operation.
         */
        void operationStarted(const QString& description);

        /**
         * Emitted to update the status of a document operation begun with
         * operationStarted(). The description is optional.  If no
         * description is provided, then the previous description used in
         * operationStarted() should be used to display status to the user.
         * Upon receipt of this signal, it is recommended that the GUI
         * be updated to refresh the status as well as the editor and
         * any other widgets that might have changed due to a document
         * operation, so that those changes can be displayed to the user.
         *
         * A call to QWidget's update() and qApp->processEvents() will
         * perform this refresh.
         */
        void operationUpdate(const QString& description = QString());

        /**
         * Emitted when an operation on the document has finished, such as
         * when a document has finished loading into the editor either from
         * being opened or reloaded from disk.  Connect to this signal to notify
         * the user that a long operation for the document has completed,
         * such as by removing a progress bar that was previously displayed
         * when the operation first started.
         */
        void operationFinished();

        /**
         * Emitted when the document is closed.
         */
        void documentClosed();

    public slots:

        /**
         * Sets whether auto-saving of the file is enabled.
         */
        void setAutoSaveEnabled(bool enabled);

        /**
         * Sets whether a backup file is created (with a .backup extension)
         * on disk before the document is saved.
         */
        void setFileBackupEnabled(bool enabled);

        /**
         * Prompts the user for a file path, and loads the document with the
         * file contents at the selected path.
         */
        void open(const QString& filePath = QString());

        /**
         * Reopens the last closed file, if any is available in the document
         * history.
         */
        void reopenLastClosedFile();

        /**
         * Reloads document from disk contents.  This method does nothing if
         * the document is new and is not associated with a file on disk.
         * Note that if the document is modified, this method will discard
         * changes before reloading.  It is left to the caller to check for
         * modification and save any changes before calling this method.
         */
        void reload();

        /**
         * Renames file represented by this document to the given file path.
         * This method does nothing if the document is new and is not
         * associated with a file on disk.
         */
        void rename();

        /**
         * Savse document contents to disk.  This method does nothing if the
         * document is new and is not associated with a file on disk.
         */
        bool save();

        /**
         * Prompts the user for a file path location, and saves the document
         * contents to the selected file path. This method is
         * also called if the document is new and is now going to be saved to
         * a file path for the first time.  Future save operations to the same
         * file path can be achieved by calling save() instead.
         */
        bool saveAs();

        /**
         * Closes the current file, clearing the editor of text and leaving
         * only an untitled "new" document in its place.  Note that isNew()
         * will return true after this method is called.
         */
        bool close();

        /**
         * Exports the current file, prompting the user for the desired
         * export format.
         */
        void exportFile();

        /**
         * Opens a print preview dialog for the current document.
         */
        void printPreview();

        /**
         * Prints the current document, first prompting the user for printing
         * options and a printer device.
         */
        void print();

    private slots:

        void onDocumentModifiedChanged(bool modified);
        void onSaveCompleted();
        void onFileChangedExternally(const QString& path);
        void printFileToPrinter(QPrinter* printer);
        void autoSaveFile();

    private:
        static const QString FILE_CHOOSER_FILTER;

        QWidget* parentWidget;
        TextDocument* document;
        MarkdownEditor* editor;
        Outline* outline;
        DocumentStatistics* documentStats;
        SessionStatistics* sessionStats;
        QFutureWatcher<QString>* saveFutureWatcher;
        QFileSystemWatcher* fileWatcher;
        bool fileHistoryEnabled;
        bool createBackupOnSave;

        /*
         * Used to set default page layout options for printing.  Also,
         * if the user closes the print preview dialog, the page layout and
         * page size settings are remembered in the event that the user reopens
         * the dialog during the same application session.
         */
        QPrinter printer;

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
        QTimer* autoSaveTimer;
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
         * Loads the document with the file contents at the given path.
         */
        bool loadFile(const QString& filePath);

        /*
         * Sets the file path for the document, such that the file will be
         * monitored for external changes made to it, and the display name
         * for the document updated.
         */
        void setFilePath(const QString& filePath);

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
            const QString& filePath,
            const QString& text,
            bool createBackup
        ) const;

        /*
         * Creates a backup file with a ".backup" extension of the file having
         * the specified path.  Note that this method is intended to be run in
         * a separate thread from the main Qt event loop, and should thus never
         * interact with any widgets.
         */
        void backupFile(const QString& filePath) const;
};

#endif // DOCUMENTMANAGER_H

