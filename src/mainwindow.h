/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QAction>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>

#include <3rdparty/QtAwesome/QtAwesome.h>

#include "preview/htmlpreview.h"
#include "settings/appsettings.h"
#include "spelling/spellcheckdecorator.h"
#include "statistics/documentstatistics.h"
#include "statistics/documentstatisticswidget.h"
#include "statistics/sessionstatistics.h"
#include "statistics/sessionstatisticswidget.h"
#include "statistics/statisticsindicator.h"
#include "theme/theme.h"
#include "theme/themerepository.h"

#include "documentmanager.h"
#include "findreplace.h"
#include "outlinewidget.h"
#include "sidebar.h"
#include "timelabel.h"

#define MAX_RECENT_FILES 10

namespace ghostwriter
{
/**
 * Main window for the application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &filePath = QString(), QWidget *parent = nullptr);
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
    QtAwesome *awesome;
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
    QPushButton *sidebarToggleButton;
    StatisticsIndicator *statisticsIndicator;
    QLabel *statusIndicator;
    TimeLabel *timeIndicator;
    QPushButton *toggleSidebarButton;
    QPushButton *previewOptionsButton;
    QPushButton *exportButton;
    QPushButton *copyHtmlButton;
    QPushButton *hemingwayModeButton;
    QPushButton *focusModeButton;
    QPushButton *htmlPreviewButton;
    HtmlPreview *htmlPreview;
    QAction *htmlPreviewMenuAction;
    QAction *fullScreenMenuAction;
    QPushButton *fullScreenButton;
    OutlineWidget *outlineWidget;
    DocumentStatistics *documentStats;
    DocumentStatisticsWidget *documentStatsWidget;
    SessionStatistics *sessionStats;
    SessionStatisticsWidget *sessionStatsWidget;
    QListWidget *cheatSheetWidget;
    QAction *recentFilesActions[MAX_RECENT_FILES];
    bool menuBarMenuActivated;
    QAction *showSidebarAction;
    bool sidebarHiddenForResize;
    bool focusModeEnabled;

    QList<QWidget *> statusBarButtons;
    QList<QWidget *> statusBarWidgets;

    AppSettings *appSettings;

    QAction* createWindowAction
    (
        const QString &text,
        QObject *receiver,
        const char *member,
        const QKeySequence &shortcut = QKeySequence()
    );

    QAction* createWidgetAction
    (
        const QString &text,
        QWidget *receiver,
        const char *member,
        const QKeySequence &shortcut = QKeySequence()
    );

    void buildMenuBar();
    void buildStatusBar();
    void buildSidebar();

    void adjustEditor();
};
} // namespace ghostwriter

#endif
