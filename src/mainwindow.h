/***********************************************************************
 *
 * Copyright (C) 2014-2022 wereturtle
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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QAction>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>

#include "3rdparty/QtAwesome/QtAwesome.h"

#include "appsettings.h"
#include "documentmanager.h"
#include "documentstatistics.h"
#include "documentstatisticswidget.h"
#include "findreplace.h"
#include "htmlpreview.h"
#include "outlinewidget.h"
#include "sessionstatistics.h"
#include "sessionstatisticswidget.h"
#include "sidebar.h"
#include "statisticsindicator.h"
#include "theme.h"
#include "themerepository.h"
#include "timelabel.h"
#include "spelling/spellcheckdecorator.h"

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
    MainWindow(const QString &filePath = QString(), QWidget *parent = 0);
    virtual ~MainWindow();

protected:
    QSize sizeHint() const;
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);

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
    void insertImage();
    void showQuickReferenceGuide();
    void showWikiPage();
    void showAbout();
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
    
#ifdef Q_OS_WIN
    void applyWindowsDarkUI();
#endif

    void adjustEditor();
};
} // namespace ghostwriter

#endif
