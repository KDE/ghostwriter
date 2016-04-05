/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
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
#include "EffectsMenuBar.h"
#include "find_dialog.h"
#include "spelling/dictionary_manager.h"
#include "spelling/dictionary_dialog.h"

#define MAX_RECENT_FILES 10

class HudWindow;
class QSettings;
class QFileSystemWatcher;
class QTextBrowser;
class QCheckBox;
class DocumentManager;
class MarkdownHighlighter;
class Outline;

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
        void keyPressEvent(QKeyEvent* e);
        void paintEvent(QPaintEvent* event);
        void closeEvent(QCloseEvent* event);

    private slots:
        void quitApplication();
        void changeTheme();
        void showFindReplaceDialog();
        void toggleFocusMode(bool checked);
        void toggleFullscreen(bool checked);
        void toggleHideMenuBarInFullScreen(bool checked);
        void toggleOutlineAlternateRowColors(bool checked);
        void toggleLiveSpellCheck(bool checked);
        void toggleFileHistoryEnabled(bool checked);
        void toggleLargeLeadingSizes(bool checked);
        void toggleAutoMatch(bool checked);
        void toggleBulletPointCycling(bool checked);
        void toggleDisplayTimeInFullScreen(bool checked);
        void toggleUseUnderlineForEmphasis(bool checked);
        void toggleSpacesForTabs(bool checked);
        void toggleDesktopCompositingEffects(bool checked);
        void insertImage();
        void changeTabulationWidth();
        void changeEditorWidth(QAction* action);
        void changeBlockquoteStyle(QAction* action);
        void changeHudButtonLayout(QAction* action);
        void showQuickReferenceGuide();
        void showOutlineHud();
        void showCheatSheetHud();
        void onQuickRefGuideLinkClicked(const QUrl& url);
        void showAbout();
        void updateWordCount(int newWordCount);
        void changeFocusMode(QAction* action);
        void applyTheme(const Theme& theme);
        void openHtmlPreview();
        void openRecentFile();
        void refreshRecentFiles();
        void clearRecentFileHistory();
        void changeDocumentDisplayName(const QString& displayName);
        void onOperationStarted(const QString& description);
        void onOperationFinished();
        void changeFont();
        void onSetDictionary();
        void onSetLocale();
        void showHudOpacityDialog();
        void changeHudOpacity(int value);
        void showAutoMatchFilterDialog();

	private:
        MarkdownEditor* editor;
        MarkdownHighlighter* highlighter;
        DocumentManager* documentManager;
        ThemeFactory* themeFactory;
        Theme theme;
        QString language;
        QLabel* wordCountLabel;
        FindDialog* findReplaceDialog;
        HtmlPreview* htmlPreview;
        QWebView* quickReferenceGuideViewer;
        QAction* fullScreenMenuAction;
        QCheckBox* fullScreenButton;
        QGraphicsColorizeEffect* fullScreenButtonColorEffect;
        QFrame* statusBarWidget;
        HudWindow* outlineHud;
        Outline* outlineWidget;
        HudWindow* cheatSheetHud;
        QListWidget* cheatSheetWidget;
        QImage originalBackgroundImage;
        QImage adjustedBackgroundImage;
        QFileSystemWatcher* fileWatcher;
        QLabel* statusLabel;
        QDialog* hudOpacityDialog = NULL;
        QAction* recentFilesActions[MAX_RECENT_FILES];
        TimeLabel* timeLabel;
        EffectsMenuBar* effectsMenuBar;

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

        void applyTheme();
        void predrawBackgroundImage();
};

#endif
