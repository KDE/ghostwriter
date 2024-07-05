/*
 * SPDX-FileCopyrightText: 2014-2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QAction>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QStringLiteral>
#include <QToolButton>

#include <KActionCollection>
#include <KHelpMenu>
#include <KStandardAction>

#include "preview/htmlpreview.h"
#include "settings/appsettings.h"
#include "spelling/spellcheckdecorator.h"
#include "statistics/documentstatistics.h"
#include "statistics/documentstatisticswidget.h"
#include "statistics/sessionstatistics.h"
#include "statistics/sessionstatisticswidget.h"
#include "statistics/statisticsindicator.h"
#include "theme/svgicontheme.h"
#include "theme/theme.h"
#include "theme/themerepository.h"

#include "appactions.h"
#include "bookmark.h"
#include "documentmanager.h"
#include "findreplace.h"
#include "outlinewidget.h"
#include "sidebar.h"
#include "timelabel.h"

namespace ghostwriter
{
/**
 * Main window for the application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &filePath = QString(), QWidget *parent = nullptr);
    virtual ~MainWindow();

protected:
    QSize sizeHint() const  override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void quitApplication();
    void changeTheme();
    void openPreferencesDialog();
    void toggleHtmlPreview(bool checked);
    void toggleHemingwayMode(bool checked);
    void toggleFocusMode(bool checked);
    void toggleFullScreen(bool checked);
    void toggleHideMenuBarInFullScreen(bool checked);
    void toggleFileHistoryEnabled(bool checked);
    void toggleDisplayTimeInFullScreen(bool checked);
    void changeEditorWidth(EditorWidth editorWidth);
    void changeInterfaceStyle(InterfaceStyle style);
    void showQuickReferenceGuide();
    void showWikiPage();
    void changeFocusMode(FocusMode focusMode);
    void applyTheme();
    void refreshRecentFiles();
    void clearRecentFileHistory();
    void changeDocumentDisplayName(const QString &displayName);
    void onOperationStarted(const QString &description);
    void onOperationFinished();
    void changeFont();
    void onFontSizeChanged(int size);
    void onSetLocale();
    void copyHtml();
    void showPreviewOptions();
    void onAboutToHideMenuBarMenu();
    void onAboutToShowMenuBarMenu();
    void onSidebarVisibilityChanged(bool visible);
    void toggleSidebarVisible(bool visible);
    void runSpellCheck();

private:
    MarkdownEditor *editor;
    SpellCheckDecorator *spelling;
    FindReplace* findReplace;
    QSplitter *previewSplitter;
    QSplitter *splitter;
    DocumentManager *documentManager;
    ThemeRepository *themeRepo;
    Theme theme;
    QString language;
    Sidebar *sidebar;
    StatisticsIndicator *statisticsIndicator;
    QLabel *statusIndicator;
    TimeLabel *timeIndicator;
    HtmlPreview *htmlPreview;
    OutlineWidget *outlineWidget;
    DocumentStatistics *documentStats;
    DocumentStatisticsWidget *documentStatsWidget;
    SessionStatistics *sessionStats;
    SessionStatisticsWidget *sessionStatsWidget;
    QListWidget *cheatSheetWidget;
    bool menuBarMenuActivated;
    bool sidebarHiddenForResize;
    bool focusModeEnabled;
    SvgIconTheme *primaryIconTheme;
    SvgIconTheme *secondaryIconTheme;

    QList<QAction *> recentFilesActions;

    QList<QWidget *> statusBarWidgets;

    AppSettings *appSettings;

    AppActions *m_actions;
    KActionCollection *m_actionCollection;

    KHelpMenu *m_helpMenu;

    KActionCollection *actionCollection() const;

    QMenu *addMenuBarMenu(const QString &name);

    QAction *appAction(AppActions::ActionType actionType) const;

    void loadTheme();
    void setupActions();
    void setupRecentFileActions(const BookmarkList &recentFiles);
    void setupGui();
    void setupMenuBar();
    void setupStatusBar();
    void setupSidebar();

    void adjustEditor();
};
} // namespace ghostwriter

#endif
