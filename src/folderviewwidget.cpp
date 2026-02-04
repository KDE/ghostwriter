/*
 * SPDX-FileCopyrightText: 2026 Nate Peterson
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QString>
#include <QTreeView>

#include "folderviewwidget.h"
#include "library.h"

namespace ghostwriter
{

class FolderViewWidgetPrivate
{
    Q_DECLARE_PUBLIC(FolderViewWidget)

public:
    FolderViewWidgetPrivate(FolderViewWidget *q_ptr)
        : q_ptr(q_ptr)
    {
        folderModel = new QFileSystemModel(q_ptr);

        // Disable custom directory icons as they can potentially impact performance
        folderModel->setOption(QFileSystemModel::DontUseCustomDirectoryIcons);

        // Hide files excluded by name filters rather than just graying them out
        folderModel->setNameFilterDisables(false);
    }

    ~FolderViewWidgetPrivate()
    {
        ;
    }

    FolderViewWidget *q_ptr;
    QFileSystemModel *folderModel;
    bool fileSelectionInProgress = false;

    /**
     * Loads the folder view from the given path.
     * See doc on FolderViewWidget::reloadFolderViewFromPath for more details.
     */
    void loadView(const QString &path, bool showAllFiles = false);

    /**
     * Invoked when the user selects a file in the folder view.
     * Emits FolderViewWidget::fileSelected.
     */
    void onFolderViewItemSelectionChanged(const QString &filePath);

    /**
     * Sets up the name filters for the folder model.
     * Either filters to only markdown/text files or shows all files.
     */
    void setNameFilters(bool showAllFiles);
};

FolderViewWidget::FolderViewWidget(QWidget *parent)
    : QTreeView(parent),
      d_ptr(new FolderViewWidgetPrivate(this))
{
    ;
}

FolderViewWidget::~FolderViewWidget()
{
    ;
}

bool FolderViewWidget::isFileSelectionInProgress() const
{
    Q_D(const FolderViewWidget);
    return d->fileSelectionInProgress;
}

void FolderViewWidget::reloadFolderViewFromPath(const QString &path, bool showAllFiles)
{
    Q_D(FolderViewWidget);

    d->loadView(path, showAllFiles);
}

void FolderViewWidget::setShowAllFiles(bool showAllFiles)
{
    Q_D(FolderViewWidget);

    d->setNameFilters(showAllFiles);
}

void FolderViewWidget::setupConnections()
{
    Q_D(FolderViewWidget);

    QItemSelectionModel *selectionModel = new QItemSelectionModel(d->folderModel, this);
    this->setSelectionModel(selectionModel);

    this->connect
    (
        selectionModel,
        &QItemSelectionModel::currentChanged,
        [d](const QModelIndex &current, const QModelIndex &) {
            QString selectedPath = d->folderModel->filePath(current);
            d->onFolderViewItemSelectionChanged(selectedPath);
        }
    );
}

void FolderViewWidgetPrivate::loadView(const QString &path, bool showAllFiles)
{
    Q_Q(FolderViewWidget);

    // Don't change the folder view to a subdirectory when selecting a file from one
    if (fileSelectionInProgress) {
        fileSelectionInProgress = false;
        return;
    }

    if (path.isNull() || path.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(path);
    QString folderPath;
    if (fileInfo.isFile()) {
        folderPath = fileInfo.absoluteDir().path();
    } else {
        folderPath = fileInfo.absoluteFilePath();
    }

    // If we have a folder path saved then use that instead,
    // as long as it is a parent directory of the requested folder path.
    Library library;
    QString lastLoadedFolderPath = library.lastLoadedFolderPath();
    if (!lastLoadedFolderPath.isNull() && !lastLoadedFolderPath.isEmpty() && folderPath.contains(lastLoadedFolderPath)) {
        folderPath = lastLoadedFolderPath;
    }
    
    // Skip loading if the currently loaded folder is the same as the requested one
    if (folderModel->rootPath().compare(folderPath) == 0) {
        return;
    }

    setNameFilters(showAllFiles);
    folderModel->setRootPath(folderPath);

    q->setUniformRowHeights(true);
    q->setAutoExpandDelay(0);
    q->setModel(folderModel);
    q->setRootIndex(folderModel->index(folderPath));
    q->setupConnections();

    // Select the active file in the folder view
    QString fileSelected;
    if (fileInfo.isFile()) {
        fileSelected = fileInfo.absoluteFilePath();
    } else {
        fileSelected = library.lastOpened().filePath();
    }

    q->selectionModel()->setCurrentIndex(folderModel->index(fileSelected), QItemSelectionModel::SelectCurrent);
    
    // Hide unnecessary UI elements
    q->setHeaderHidden(true);
    q->hideColumn(1); // Size
    q->hideColumn(2); // Type
    q->hideColumn(3); // Date Modified

    // Ensure the selected item is visible in the folder view
    q->scrollTo(q->selectionModel()->currentIndex(), QAbstractItemView::PositionAtTop);
    
    // Save the loaded folder path so we can restore it on next launch
    library.setLastLoadedFolder(folderPath);
}

void FolderViewWidgetPrivate::onFolderViewItemSelectionChanged(const QString &filePath)
{
    Q_Q(FolderViewWidget);

    fileSelectionInProgress = true;

    QFileInfo fileInfo(filePath);
    if (fileInfo.isFile()) {
        emit q->fileSelected(filePath);
    }
}

void FolderViewWidgetPrivate::setNameFilters(bool showAllFiles)
{
    QStringList nameFilters;
    if (showAllFiles) {
        nameFilters = QStringList();
    } else {
        // Use the same set of file extensions as the DocumentManager
        nameFilters = { "*.md", "*.markdown", "*.mdown", "*.mkdn", "*.mkd", "*.mdwn", "*.mdtxt", "*.mdtext", "*.text", "*.Rmd", "*.txt" };
    }

    folderModel->setNameFilters(nameFilters);
}

} // namespace ghostwriter