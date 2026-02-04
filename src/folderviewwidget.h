/*
 * SPDX-FileCopyrightText: 2026 Nate Peterson
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef FOLDER_VIEW_WIDGET_H
#define FOLDER_VIEW_WIDGET_H

#include <QScopedPointer>
#include <QString>
#include <QTreeView>
#include <QWidget>

namespace ghostwriter
{
/**
 * Folder view widget for displaying and selecting files from the directory
 * of the file that was opened.
 */
class FolderViewWidgetPrivate;
class FolderViewWidget : public QTreeView
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FolderViewWidget)

public:
    FolderViewWidget(QWidget *parent = nullptr);
    virtual ~FolderViewWidget();

    bool isFileSelectionInProgress() const;

signals:
    /**
     * Emitted when the user selects a file in the folder view.
     * This signal informs connected slots the path of the file
     * that was selected.
     * This is not emitted when a directory is selected.
     */
    void fileSelected(const QString &filePath);

public slots:
    /**
     * Reloads the folder view from the given path.
     * If the path is a directory, then the directory is shown.
     * If the path is a file, then the parent directory of the file is shown.
     * Optionally set whether on initial load all files can be shown instead
     * of just markdown files.
     */
    void reloadFolderViewFromPath(const QString &path, bool showAllFiles = false);
    
    /**
     * Sets whether all files are shown in the folder view
     * or only markdown files.
     */
    void setShowAllFiles(bool showAllFiles);

private:
    QScopedPointer<FolderViewWidgetPrivate> d_ptr;

    /**
     * Sets up connections for tracking the state of interactions with the view
     * (ie. file selection changes).
     */
    void setupConnections();
};
} // namespace ghostwriter

#endif // FOLDER_VIEW_WIDGET_H