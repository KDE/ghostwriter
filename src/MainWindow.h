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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QGraphicsColorizeEffect>
#include <QGraphicsDropShadowEffect>

#include "MarkdownEditor.h"
#include "HtmlPreview.h"
#include "ThemeFactory.h"
#include "HtmlPreview.h"
#include "AppSettings.h"
#include "TimeLabel.h"
#include "find_dialog.h"
#include "spelling/dictionary_manager.h"

#define MAX_RECENT_FILES 10

class HudWindow;
class QSettings;
class QFileSystemWatcher;
class QTextBrowser;
class QCheckBox;
class QListWidget;
class DocumentManager;
class Outline;
class DocumentStatistics;
class DocumentStatisticsWidget;
class SessionStatistics;
class SessionStatisticsWidget;

/**
 * Main window for the application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

	public:
        MainWindow(const QString& filePath = QString(), QWidget* parent = 0);
        virtual ~MainWindow();

    protected:
        QSize sizeHint() const;
        void resizeEvent(QResizeEvent* event);
        void moveEvent(QMoveEvent* event);
        void keyPressEvent(QKeyEvent* e);
        bool eventFilter(QObject* obj, QEvent* event);
        void paintEvent(QPaintEvent* event);
        void closeEvent(QCloseEvent* event);

    private slots:
        void quitApplication();
        void changeTheme();
        void showFindReplaceDialog();
        void openPreferencesDialog();
        void toggleHtmlPreview(bool checked);
        void toggleHemingwayMode(bool checked);
        void toggleFocusMode(bool checked);
        void toggleFullScreen(bool checked);
        void toggleHideMenuBarInFullScreen(bool checked);
        void toggleOutlineAlternateRowColors(bool checked);
        void toggleFileHistoryEnabled(bool checked);
        void toggleDisplayTimeInFullScreen(bool checked);
        void toggleDesktopCompositingEffects(bool checked);
        void changeHudButtonLayout(HudWindowButtonLayout layout);
        void changeEditorWidth(EditorWidth editorWidth);
        void changeInterfaceStyle(InterfaceStyle style);
        void insertImage();
        void showStyleSheetManager();
        void showQuickReferenceGuide();
        void showWikiPage();
        void showOutlineHud();
        void showCheatSheetHud();
        void showDocumentStatisticsHud();
        void showSessionStatisticsHud();
        void onQuickRefGuideLinkClicked(const QUrl& url);
        void showAbout();
        void updateWordCount(int newWordCount);
        void changeFocusMode(FocusMode focusMode);
        void applyTheme(const Theme& theme);
        void openRecentFile();
        void refreshRecentFiles();
        void clearRecentFileHistory();
        void changeDocumentDisplayName(const QString& displayName);
        void onOperationStarted(const QString& description);
        void onOperationFinished();
        void changeFont();
        void onFontSizeChanged(int size);
        void onSetLocale();
        void changeHudOpacity(int value);
        void copyHtml();
        void showPreviewOptions();
        void onAboutToHideMenuBarMenu();
        void onAboutToShowMenuBarMenu();

	private:
        MarkdownEditor* editor;
        QSplitter* splitter;
        DocumentManager* documentManager;
        ThemeFactory* themeFactory;
        Theme theme;
        QString language;
        QLabel* wordCountLabel;
        QLabel* statusLabel;
        TimeLabel* timeLabel;
        QPushButton* previewOptionsButton;
        QPushButton* exportButton;
        QPushButton* copyHtmlButton;
        QPushButton* hemingwayModeButton;
        QPushButton* focusModeButton;
        QPushButton* htmlPreviewButton;
        FindDialog* findReplaceDialog;
        HtmlPreview* htmlPreview;
        QWebView* quickReferenceGuideViewer;
        QAction* fullScreenMenuAction;
        QPushButton* fullScreenButton;
        QGraphicsColorizeEffect* fullScreenButtonColorEffect;
        HudWindow* outlineHud;
        Outline* outlineWidget;
        HudWindow* cheatSheetHud;
        HudWindow* documentStatsHud;
        HudWindow* sessionStatsHud;
        QVector<HudWindow*> huds;
        QVector<QString> hudGeometryKeys;
        QVector<QString> hudOpenKeys;
        DocumentStatistics* documentStats;
        DocumentStatisticsWidget* documentStatsWidget;
        SessionStatistics* sessionStats;
        SessionStatisticsWidget* sessionStatsWidget;
        QListWidget* cheatSheetWidget;
        QPixmap originalBackgroundImage;
        QPixmap adjustedBackgroundImage;
        QFileSystemWatcher* fileWatcher;
        QAction* recentFilesActions[MAX_RECENT_FILES];
        int menuBarHeight = 0;
        QPoint lastMousePos;
        bool menuBarMenuActivated;

        QList<QWidget*> statusBarButtons;
        QList<QWidget*> statusBarWidgets;

        Exporter* exporter;
        QThread* exporterThread;

        AppSettings* appSettings;

		QAction* addMenuAction
        (
            QMenu* menu,
            const QString& name,
            const QString& shortcut = 0,
            bool checkable = false,
            bool checked = false,
            QActionGroup* actionGroup = 0
        );

        void buildMenuBar();
        void buildStatusBar();

        void adjustEditorWidth(int width);
        void applyStatusBarStyle();
        void applyTheme();
        void predrawBackgroundImage();
        void showMenuBar();
        void hideMenuBar();
        bool isMenuBarVisible() const;
};

#endif
