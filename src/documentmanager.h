/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QScopedPointer>

#include "markdowndocument.h"
#include "markdowneditor.h"

namespace ghostwriter
{
/**
 * Manages the life-cycle of a document, facilitating user interaction for
 * opening, closing, saving, etc.
 */
class DocumentManagerPrivate;
class DocumentManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DocumentManager)

public:
    /**
     * Constructor.  Takes MarkdownEditor as a parameter, which is used
     * to display the current document to the user.  Also takes the
     * document statistics as a parameter in order to reset the statistics
     * when a new file is loaded.
     */
    DocumentManager
    (
        MarkdownEditor *editor,
        QObject *parent = nullptr
    );

    /**
     * Destructor.
     */
    virtual ~DocumentManager();

    /**
     * Gets the current document that is opened.
     */
    MarkdownDocument *document() const;

    /**
     * Gets whether auto-save is enabled.
     */
    bool autoSaveEnabled() const;

    /**
     * Gets whether automatic file backup (i.e., creation of a .backup
     * file at regular intervals), is enabled.
     */
    bool fileBackupEnabled() const;

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
    void documentDisplayNameChanged(const QString &displayName);

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
    void operationStarted(const QString &description);

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
    void operationUpdate(const QString &description = QString());

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
     * Emitted when a document is loaded from disk.
     */
    void documentLoaded();

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
     * Sets draft directory location where draft files (i.e., autosaved
     * untitled documents) will be saved.
     */
    void setDraftLocation(const QString &directory);

    /**
     * Prompts the user for a file path, and loads the document with the
     * file contents at the selected path.
     */
    void open(const QString &filePath = QString());

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
     * Saves document contents to disk. If the file is a saved draft, call
     * saveAs, otherwise just save.
     */
    bool saveFile();

    /**
     * Saves document contents to disk.  This method does nothing if the
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

private:
    QScopedPointer<DocumentManagerPrivate> d_ptr;
};
}

#endif // DOCUMENTMANAGER_H

