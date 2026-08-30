#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "../document/textdocument.h"
#include "../editor/markdowneditor.h"
#include "../settings/appsettings.h"
#include <QMainWindow>
#include <QObject>
#include <QScopedPointer>

namespace ghostwriter
{
class WorkspacePrivate;
class Workspace : QObject
{
public:
    Workspace(QMainWindow *parent);
    ~Workspace();

    bool setSaveDraftsEnabled();
    bool setBackupsEnabled();
    bool setAutoReloadEnabled();

private slots:
    void onActiveDocumentChanged(int index);

    void openDocument();
    void openRecentDocument();
    void saveActiveDocument();
    void saveActiveDocumentAs();
    void saveAll();
    void renameActiveDocument();
    void closeActiveDocument();
    void exportActiveDocument();
    void revertActiveDocument();
    void reloadActiveDocument();

private:
    QScopedPointer<WorkspacePrivate> d;
};
} // namespace ghostwriter

#endif // WORKSPACE_H