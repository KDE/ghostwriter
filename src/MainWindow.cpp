/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
 * Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#include <QWidget>
#include <QApplication>
#include <QIcon>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QIODevice>
#include <QFile>
#include <QFontMetrics>
#include <QFont>
#include <QCommonStyle>
#include <QScrollBar>
#include <QGridLayout>
#include <QGraphicsColorizeEffect>
#include <QLocale>
#include <QPainter>
#include <QSettings>
#include <QFontDialog>
#include <QTextBrowser>
#include <QCheckBox>
#include <QInputDialog>
#include <QDesktopServices>
#include <QDesktopWidget>

#include "MainWindow.h"
#include "ThemeFactory.h"
#include "HtmlPreview.h"
#include "find_dialog.h"
#include "ColorHelper.h"
#include "HudWindow.h"
#include "ThemeSelectionDialog.h"
#include "MarkdownHighlighter.h"
#include "DocumentManager.h"
#include "DocumentHistory.h"
#include "Outline.h"
#include "MessageBoxHelper.h"
#include "SimpleFontDialog.h"

#define GW_MAIN_WINDOW_GEOMETRY_KEY "Window/mainWindowGeometry"
#define GW_MAIN_WINDOW_STATE_KEY "Window/mainWindowState"
#define GW_OUTLINE_HUD_GEOMETRY_KEY "HUD/outlineHudGeometry"
#define GW_CHEAT_SHEET_HUD_GEOMETRY_KEY "HUD/cheatSheetHudGeometry"
#define GW_OUTLINE_HUD_OPEN_KEY "HUD/outlineHudOpen"
#define GW_CHEAT_SHEET_HUD_OPEN_KEY "HUD/cheatSheetHudOpen"
#define GW_HTML_PREVIEW_GEOMETRY_KEY "Preview/htmlPreviewGeometry"
#define GW_HTML_PREVIEW_OPEN "Preview/htmlPreviewOpen"

MainWindow::MainWindow(const QString& filePath, QWidget* parent)
    : QMainWindow(parent)
{
    QString fileToOpen;
    setWindowIcon(QIcon(":/resources/images/ghostwriter.svg"));
    this->setObjectName("mainWindow");
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    appSettings = AppSettings::getInstance();

    outlineWidget = new Outline();

    // We need to set an empty style for the editor's scrollbar in order for the
    // scrollbar CSS stylesheet to take full effect.  Otherwise, the scrollbar's
    // background color will have the Windows 98 checkered look rather than
    // being a solid or transparent color.
    //
    outlineWidget->verticalScrollBar()->setStyle(new QCommonStyle());
    outlineWidget->horizontalScrollBar()->setStyle(new QCommonStyle());

    outlineHud = new HudWindow(this);
    outlineHud->setWindowTitle(tr("Outline"));
    outlineHud->setCentralWidget(outlineWidget);
    outlineHud->setButtonLayout(appSettings->getHudButtonLayout());

    cheatSheetWidget = new QListWidget();

    // Set empty style so that stylesheet takes full effect. (See comment
    // above on outlineWidget for more details.)
    //
    cheatSheetWidget->verticalScrollBar()->setStyle(new QCommonStyle());
    cheatSheetWidget->horizontalScrollBar()->setStyle(new QCommonStyle());

    cheatSheetWidget->addItem(tr("# Heading 1"));
    cheatSheetWidget->addItem(tr("## Heading 2"));
    cheatSheetWidget->addItem(tr("### Heading 3"));
    cheatSheetWidget->addItem(tr("#### Heading 4"));
    cheatSheetWidget->addItem(tr("##### Heading 5"));
    cheatSheetWidget->addItem(tr("###### Heading 6"));
    cheatSheetWidget->addItem(tr("*Emphasis* _Emphasis_"));
    cheatSheetWidget->addItem(tr("**Strong** __Strong__"));
    cheatSheetWidget->addItem(tr("1. Numbered List"));
    cheatSheetWidget->addItem(tr("* Bullet List"));
    cheatSheetWidget->addItem(tr("+ Bullet List"));
    cheatSheetWidget->addItem(tr("- Bullet List"));
    cheatSheetWidget->addItem(tr("> Block Quote"));
    cheatSheetWidget->addItem(tr("`Code Span`"));
    cheatSheetWidget->addItem(tr("``` Code Block"));
    cheatSheetWidget->addItem(tr("[Link](http://url.com \"Title\"]"));
    cheatSheetWidget->addItem(tr("[Reference Link][ID]"));
    cheatSheetWidget->addItem(tr("![Image][./image.jpg \"Title\"]"));
    cheatSheetWidget->addItem(tr("--- *** ___ Horizontal Rule"));

    cheatSheetHud = new HudWindow(this);
    cheatSheetHud->setWindowTitle(tr("Cheat Sheet"));
    cheatSheetHud->setCentralWidget(cheatSheetWidget);
    cheatSheetHud->setButtonLayout(appSettings->getHudButtonLayout());

    TextDocument* document = new TextDocument();

    // The below connection must happen before the Highlighter is created and
    // connected to the text document. See note in Outline class's
    // onTextChanged() slot.
    //
    connect(document, SIGNAL(contentsChange(int,int,int)), outlineWidget, SLOT(onTextChanged(int,int,int)));

    highlighter = new MarkdownHighlighter(document);
    highlighter->setSpellCheckEnabled(appSettings->getLiveSpellCheckEnabled());
    highlighter->setBlockquoteStyle(appSettings->getBlockquoteStyle());
    connect(highlighter, SIGNAL(headingFound(int,int,QString)), outlineWidget, SLOT(insertHeadingIntoOutline(int,int,QString)));
    connect(highlighter, SIGNAL(headingRemoved(int)), outlineWidget, SLOT(removeHeadingFromOutline(int)));

    editor = new MarkdownEditor(document, highlighter, this);
    editor->setFont(appSettings->getFont().family(), appSettings->getFont().pointSize());
    editor->setUseUnderlineForEmphasis(appSettings->getUseUnderlineForEmphasis());
    editor->setEnableLargeHeadingSizes(appSettings->getLargeHeadingSizesEnabled());
    editor->setAutoMatchEnabled(appSettings->getAutoMatchEnabled());
    editor->setBulletPointCyclingEnabled(appSettings->getBulletPointCyclingEnabled());
    editor->setPlainText("");
    editor->setEditorWidth((EditorWidth) appSettings->getEditorWidth());
    connect(outlineWidget, SIGNAL(documentPositionNavigated(int)), editor, SLOT(navigateDocument(int)));
    connect(editor, SIGNAL(cursorPositionChanged(int)), outlineWidget, SLOT(updateCurrentNavigationHeading(int)));

    // We need to set an empty style for the editor's scrollbar in order for the
    // scrollbar CSS stylesheet to take full effect.  Otherwise, the scrollbar's
    // background color will have the Windows 98 checkered look rather than
    // being a solid or transparent color.
    //
    editor->verticalScrollBar()->setStyle(new QCommonStyle());
    editor->horizontalScrollBar()->setStyle(new QCommonStyle());

    documentManager = new DocumentManager(editor, this);
    documentManager->setAutoSaveEnabled(appSettings->getAutoSaveEnabled());
    documentManager->setFileBackupEnabled(appSettings->getBackupFileEnabled());
    documentManager->setFileHistoryEnabled(appSettings->getFileHistoryEnabled());
    setWindowTitle(documentManager->getDocument()->getDisplayName() + "[*] - " + qAppName());
    connect(documentManager, SIGNAL(documentDisplayNameChanged(QString)), this, SLOT(changeDocumentDisplayName(QString)));
    connect(documentManager, SIGNAL(documentModifiedChanged(bool)), this, SLOT(setWindowModified(bool)));
    connect(documentManager, SIGNAL(operationStarted(QString)), this, SLOT(onOperationStarted(QString)));
    connect(documentManager, SIGNAL(operationFinished()), this, SLOT(onOperationFinished()));
    connect(documentManager, SIGNAL(documentClosed()), this, SLOT(refreshRecentFiles()));

    QWidget* editorPane = new QWidget(this);
    editorPane->setObjectName("editorLayoutArea");
    editorPane->setLayout(editor->getPreferredLayout());

    setCentralWidget(editorPane);

    findReplaceDialog = new FindDialog(editor);
    findReplaceDialog->setModal(false);
    connect(findReplaceDialog, SIGNAL(replaceAllComplete()), editor, SLOT(refreshWordCount()));

    QStringList recentFiles;

    if (appSettings->getFileHistoryEnabled())
    {
        DocumentHistory history;
        recentFiles = history.getRecentFiles(MAX_RECENT_FILES + 2);
    }

    if (!filePath.isNull() && !filePath.isEmpty())
    {
        QFileInfo cliFileInfo(filePath);

        if (cliFileInfo.exists())
        {
            fileToOpen = filePath;
            recentFiles.removeAll(cliFileInfo.absoluteFilePath());
        }
        else
        {
            qCritical("File specified on command line does not exist.");
            exit(-1);
        }
    }

    if (fileToOpen.isNull() && appSettings->getFileHistoryEnabled())
    {
        QString lastFile;

        if (!recentFiles.isEmpty())
        {
            lastFile = recentFiles.first();
        }

        if (QFileInfo(lastFile).exists())
        {
            fileToOpen = lastFile;
            recentFiles.removeAll(lastFile);
        }
    }

    for (int i = 0; i < MAX_RECENT_FILES; i++)
    {
        recentFilesActions[i] = new QAction(this);
        connect
        (
            recentFilesActions[i],
            SIGNAL(triggered()),
            this,
            SLOT(openRecentFile())
        );

        if (i < recentFiles.size())
        {
            recentFilesActions[i]->setText(recentFiles.at(i));
            recentFilesActions[i]->setVisible(true);
        }
        else
        {
            recentFilesActions[i]->setVisible(false);
        }
    }

    buildMenuBar();
    buildStatusBar();


    QString themeName = appSettings->getThemeName();

    QString err;

    theme = ThemeFactory::getInstance()->loadTheme(themeName, err);

    fullScreenButtonColorEffect = new QGraphicsColorizeEffect();
    fullScreenButtonColorEffect->setColor(QColor(Qt::white));

    // Determine locale for dictionary language (for use in spell checking).
    language = DictionaryManager::instance().availableDictionary(appSettings->getDictionaryLanguage());

    // If we have an available dictionary, then get it and set up spell checking.
    if (!language.isNull() && !language.isEmpty())
    {
        DictionaryManager::instance().setDefaultLanguage(language);
        DictionaryRef dictionary = DictionaryManager::instance().requestDictionary();
        editor->setDictionary(dictionary);
        editor->setSpellCheckEnabled(appSettings->getLiveSpellCheckEnabled());
    }
    else
    {
        editor->setSpellCheckEnabled(false);
    }

    connect
    (
        editor,
        SIGNAL(wordCountChanged(int)),
        this,
        SLOT(updateWordCount(int))
    );

    // Note that the parent widget for this new window must be NULL, so that
    // it will hide beneath other windows when it is deactivated.
    //
    htmlPreview = new HtmlPreview(documentManager->getDocument(), NULL);

    connect(editor, SIGNAL(typingPaused()), htmlPreview, SLOT(updatePreview()));
    connect(outlineWidget, SIGNAL(headingNumberNavigated(int)), htmlPreview, SLOT(navigateToHeading(int)));
    connect(htmlPreview, SIGNAL(operationStarted(QString)), this, SLOT(onOperationStarted(QString)));
    connect(htmlPreview, SIGNAL(operationFinished()), this, SLOT(onOperationFinished()));

    // Set dimensions for all the windows/HUDs.
    QSettings windowSettings;

    if (windowSettings.contains(GW_MAIN_WINDOW_GEOMETRY_KEY))
    {
        restoreGeometry(windowSettings.value(GW_MAIN_WINDOW_GEOMETRY_KEY).toByteArray());
        restoreState(windowSettings.value(GW_MAIN_WINDOW_STATE_KEY).toByteArray());

        if (this->isFullScreen())
        {
            effectsMenuBar->setAutoHideEnabled(appSettings->getHideMenuBarInFullScreenEnabled());
        }
        else
        {
            effectsMenuBar->setAutoHideEnabled(false);
        }
    }
    else
    {
        this->adjustSize();
    }

    if (windowSettings.contains(GW_OUTLINE_HUD_GEOMETRY_KEY))
    {
        outlineHud->restoreGeometry(windowSettings.value(GW_OUTLINE_HUD_GEOMETRY_KEY).toByteArray());
    }
    else
    {
        outlineHud->move(200, 200);
        outlineHud->adjustSize();
    }

    if (windowSettings.contains(GW_CHEAT_SHEET_HUD_GEOMETRY_KEY))
    {
        cheatSheetHud->restoreGeometry(windowSettings.value(GW_CHEAT_SHEET_HUD_GEOMETRY_KEY).toByteArray());
    }
    else
    {
        cheatSheetHud->move(400, 400);
        cheatSheetHud->adjustSize();
    }

    if (windowSettings.contains(GW_HTML_PREVIEW_GEOMETRY_KEY))
    {
        htmlPreview->restoreGeometry(windowSettings.value(GW_HTML_PREVIEW_GEOMETRY_KEY).toByteArray());
    }
    else
    {
        htmlPreview->adjustSize();
    }

    quickReferenceGuideViewer = NULL;

    show();

    if (windowSettings.value(GW_OUTLINE_HUD_OPEN_KEY, QVariant(false)).toBool())
    {
        outlineHud->show();
    }

    if (windowSettings.value(GW_CHEAT_SHEET_HUD_OPEN_KEY, QVariant(false)).toBool())
    {
        cheatSheetHud->show();
    }

    if (windowSettings.value(GW_HTML_PREVIEW_OPEN, QVariant(false)).toBool())
    {
        htmlPreview->show();
    }

    // Apply the theme only after show() is called on all the widgets,
    // since the Outline scrollbars can end up transparent in Windows if
    // the theme is applied before show().
    //
    applyTheme();

    if (!fileToOpen.isNull() && !fileToOpen.isEmpty())
    {
        documentManager->open(fileToOpen);
    }
}

MainWindow::~MainWindow()
{
    if (NULL != htmlPreview)
    {
        delete htmlPreview;
    }

    if (NULL != quickReferenceGuideViewer)
    {
        delete quickReferenceGuideViewer;
    }
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 500);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    // Resize the editor's margins based on the new size of the window.
    editor->setupPaperMargins(event->size().width());

    if (!originalBackgroundImage.isNull())
    {
        predrawBackgroundImage();
    }
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    int key = e->key();

    switch (key)
    {
        case Qt::Key_Escape:
            if (this->isFullScreen())
            {
                toggleFullscreen(false);
            }
            break;
        default:
            break;
    }

    QMainWindow::keyPressEvent(e);
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), theme.getBackgroundColor().rgb());

    if (!adjustedBackgroundImage.isNull())
    {
        painter.drawImage(0, 0, adjustedBackgroundImage);
    }

    if (EditorAspectStretch == theme.getEditorAspect())
    {
        painter.fillRect(this->rect(), theme.getEditorBackgroundColor());
    }

    painter.end();
    QMainWindow::paintEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (documentManager->close())
    {
        this->quitApplication();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::quitApplication()
{
    if (documentManager->close())
    {
        appSettings->setAutoSaveEnabled(documentManager->getAutoSaveEnabled());
        appSettings->setBackupFileEnabled(documentManager->getFileBackupEnabled());
        appSettings->store();

        QSettings windowSettings;
        windowSettings.setValue(GW_MAIN_WINDOW_GEOMETRY_KEY, saveGeometry());
        windowSettings.setValue(GW_MAIN_WINDOW_STATE_KEY, saveState());
        windowSettings.setValue(GW_OUTLINE_HUD_GEOMETRY_KEY, outlineHud->saveGeometry());
        windowSettings.setValue(GW_OUTLINE_HUD_OPEN_KEY, QVariant(outlineHud->isVisible()));
        windowSettings.setValue(GW_CHEAT_SHEET_HUD_GEOMETRY_KEY, cheatSheetHud->saveGeometry());
        windowSettings.setValue(GW_CHEAT_SHEET_HUD_OPEN_KEY, QVariant(cheatSheetHud->isVisible()));
        windowSettings.setValue(GW_HTML_PREVIEW_GEOMETRY_KEY, htmlPreview->saveGeometry());
        windowSettings.setValue(GW_HTML_PREVIEW_OPEN, QVariant(htmlPreview->isVisible()));
        windowSettings.sync();

        DictionaryManager::instance().addProviders();
        DictionaryManager::instance().setDefaultLanguage(language);

		qApp->quit();
    }
}

void MainWindow::changeTheme()
{
    ThemeSelectionDialog* themeDialog = new ThemeSelectionDialog(theme.getName(), this);
    connect(themeDialog, SIGNAL(applyTheme(Theme)), this, SLOT(applyTheme(Theme)));
    themeDialog->show();
}


void MainWindow::showFindReplaceDialog()
{
    findReplaceDialog->show();
}

void MainWindow::toggleFocusMode(bool checked)
{
    if (checked)
    {
        editor->setFocusMode(appSettings->getFocusMode());
    }
    else
    {
        editor->setFocusMode(FocusModeDisabled);
    }

}

void MainWindow::toggleFullscreen(bool checked)
{
    // This method can be called either from the menu bar (View->Full Screen)
    // or from the full screen toggle button on the status bar.  To keep their
    // "checked/unchecked" states in sync, we have to determine who called
    // this method (which "sender" triggered the event--the button or the
    // menu option), and set the other's checked/unchecked state accordingly.
    // Unfortunately, setting their checked states causes this slot to be
    // triggered again recursively.  This means we have to use only one of the
    // "senders" as the source of "truth" to determine whether to go
    // full screen or to restore.  Thus, whenever the sender is the button in
    // the status bar, we'll toggle full screen mode. Otherwise, we'll just
    // set the button's state, knowing that this slot will be triggered again
    // recursively in doing so and that the full screen mode will be toggled
    // accordingly.
    //

    if (this->isFullScreen() || !checked)
    {
        if (this->sender() != fullScreenButton)
        {
            fullScreenButton->setChecked(false);
        }
        else
        {
            if (appSettings->getDisplayTimeInFullScreenEnabled())
            {
                timeLabel->hide();
            }

            fullScreenMenuAction->setChecked(false);
            showNormal();

            if (appSettings->getHideMenuBarInFullScreenEnabled())
            {
                effectsMenuBar->setAutoHideEnabled(false);
            }
        }
    }
    else
    {
        if (this->sender() != fullScreenButton)
        {
            fullScreenButton->setChecked(true);
        }
        else
        {
            if (appSettings->getDisplayTimeInFullScreenEnabled())
            {
                timeLabel->show();
            }

            fullScreenMenuAction->setChecked(true);
            showFullScreen();

            if (appSettings->getHideMenuBarInFullScreenEnabled())
            {
                effectsMenuBar->setAutoHideEnabled(true);
            }
        }
    }
}

void MainWindow::toggleHideMenuBarInFullScreen(bool checked)
{
    appSettings->setHideMenuBarInFullScreenEnabled(checked);

    if (this->isFullScreen())
    {
        effectsMenuBar->setAutoHideEnabled(checked);
    }
}

void MainWindow::toggleOutlineAlternateRowColors(bool checked)
{
    outlineWidget->setAlternatingRowColors(checked);
    cheatSheetWidget->setAlternatingRowColors(checked);
    appSettings->setAlternateHudRowColorsEnabled(checked);
    applyTheme();
}

void MainWindow::toggleLiveSpellCheck(bool checked)
{
    appSettings->setLiveSpellCheckEnabled(checked);
    editor->setSpellCheckEnabled(checked);
}

void MainWindow::toggleFileHistoryEnabled(bool checked)
{
    if (!checked)
    {
        this->clearRecentFileHistory();
    }

    appSettings->setFileHistoryEnabled(checked);
    documentManager->setFileHistoryEnabled(checked);
}

void MainWindow::toggleLargeLeadingSizes(bool checked)
{
    editor->setEnableLargeHeadingSizes(checked);
    appSettings->setLargeHeadingSizesEnabled(checked);
}

void MainWindow::toggleAutoMatch(bool checked)
{
    editor->setAutoMatchEnabled(checked);
    appSettings->setAutoMatchEnabled(checked);
}

void MainWindow::toggleBulletPointCycling(bool checked)
{
    editor->setBulletPointCyclingEnabled(checked);
    appSettings->setBulletPointCyclingEnabled(checked);
}

void MainWindow::toggleDisplayTimeInFullScreen(bool checked)
{
    appSettings->setDisplayTimeInFullScreenEnabled(checked);

    if (this->isFullScreen())
    {
        if (checked)
        {
            this->timeLabel->show();
        }
        else
        {
            this->timeLabel->hide();
        }
    }
}

void MainWindow::toggleUseUnderlineForEmphasis(bool checked)
{
    editor->setUseUnderlineForEmphasis(checked);
    appSettings->setUseUnderlineForEmphasis(checked);
}

void MainWindow::toggleSpacesForTabs(bool checked)
{
    editor->setInsertSpacesForTabs(checked);
    appSettings->setInsertSpacesForTabsEnabled(checked);
}

void MainWindow::toggleDesktopCompositingEffects(bool checked)
{
    appSettings->setDesktopCompositingEnabled(checked);
    outlineHud->setDesktopCompositingEnabled(checked);
    cheatSheetHud->setDesktopCompositingEnabled(checked);
}

void MainWindow::insertImage()
{
    QString startingDirectory = QString();
    TextDocument* document = documentManager->getDocument();

    if (!document->isNew())
    {
        startingDirectory = QFileInfo(document->getFilePath()).dir().path();
    }

    QString imagePath =
        QFileDialog::getOpenFileName
        (
            this,
            tr("Insert Image"),
            startingDirectory,
            QString("%1 (*.jpg *.jpeg *.gif *.png *.bmp);; %2")
                .arg(tr("Images"))
                .arg(tr("All Files"))
        );

    if (!imagePath.isNull() && !imagePath.isEmpty())
    {
        QFileInfo imgInfo(imagePath);
        bool isRelativePath = false;

        if (imgInfo.exists())
        {
            if (!document->isNew())
            {
                QFileInfo docInfo(document->getFilePath());

                if (docInfo.exists())
                {
                    imagePath = docInfo.dir().relativeFilePath(imagePath);
                    isRelativePath = true;
                }
            }
        }

        if (!isRelativePath)
        {
            imagePath = QString("file://") + imagePath;
        }

        QTextCursor cursor = editor->textCursor();
        cursor.insertText(QString("![](%1)").arg(imagePath));
    }
}

void MainWindow::changeTabulationWidth()
{
    int width = QInputDialog::getInt
        (
            this,
            tr("Tabulation Width"),
            tr("Spaces"),
            appSettings->getTabWidth(),
            appSettings->MIN_TAB_WIDTH,
            appSettings->MAX_TAB_WIDTH
        );
    editor->setTabStopWidth(width);
    appSettings->setTabWidth(width);
}

void MainWindow::changeEditorWidth(QAction* action)
{
    EditorWidth value = (EditorWidth) action->data().toInt();
    appSettings->setEditorWidth(value);
    editor->setEditorWidth(value);
    editor->setupPaperMargins(this->width());
}

void MainWindow::changeBlockquoteStyle(QAction* action)
{
    BlockquoteStyle style = (BlockquoteStyle) action->data().toInt();
    highlighter->setBlockquoteStyle(style);
    appSettings->setBlockquoteStyle(style);
}

void MainWindow::changeHudButtonLayout(QAction* action)
{
    HudWindowButtonLayout layout = (HudWindowButtonLayout) action->data().toInt();
    this->outlineHud->setButtonLayout(layout);
    this->cheatSheetHud->setButtonLayout(layout);
    appSettings->setHudButtonLayout(layout);
}

void MainWindow::showQuickReferenceGuide()
{
    if (NULL == quickReferenceGuideViewer)
    {
        QFile inputFile(":/resources/quickreferenceguide.html");

        if (!inputFile.open(QIODevice::ReadOnly))
        {
            MessageBoxHelper::critical
            (
                this,
                tr("Failed to open Quick Reference Guide."),
                inputFile.errorString()
            );
            inputFile.close();
            return;
        }

        QTextStream inStream(&inputFile);
        inStream.setCodec("UTF-8");
        QString html = inStream.readAll();
        inputFile.close();

        // Note that the parent widget for this new window must be NULL, so that
        // it will hide beneath other windows when it is deactivated.
        //
        quickReferenceGuideViewer = new QWebView(NULL);
        quickReferenceGuideViewer->setWindowTitle(tr("Quick Reference Guide"));
        quickReferenceGuideViewer->setWindowFlags(Qt::Window);
        quickReferenceGuideViewer->settings()->setDefaultTextEncoding("utf-8");
        quickReferenceGuideViewer->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
        quickReferenceGuideViewer->page()->action(QWebPage::Reload)->setVisible(false);
        quickReferenceGuideViewer->page()->action(QWebPage::OpenLink)->setVisible(false);
        quickReferenceGuideViewer->page()->action(QWebPage::OpenLinkInNewWindow)->setVisible(false);
        quickReferenceGuideViewer->settings()->setUserStyleSheetUrl(QUrl("qrc:/resources/github.css"));
        quickReferenceGuideViewer->page()->setContentEditable(false);
        quickReferenceGuideViewer->setContent(html.toUtf8(), "text/html");
        connect(quickReferenceGuideViewer, SIGNAL(linkClicked(QUrl)), this, SLOT(onQuickRefGuideLinkClicked(QUrl)));

        // Set zoom factor for WebKit browser to account for system DPI settings,
        // since WebKit assumes 96 DPI as a fixed resolution.
        //
        QWidget* window = QApplication::desktop()->screen();
        int horizontalDpi = window->logicalDpiX();
        // Don't want to affect image size, only text size.
        quickReferenceGuideViewer->settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
        quickReferenceGuideViewer->setZoomFactor((horizontalDpi / 96.0));

        quickReferenceGuideViewer->resize(500, 600);
        quickReferenceGuideViewer->adjustSize();
    }

    quickReferenceGuideViewer->show();
    quickReferenceGuideViewer->raise();
    quickReferenceGuideViewer->activateWindow();
}

void MainWindow::showOutlineHud()
{
    outlineHud->show();
    outlineHud->activateWindow();
}

void MainWindow::showCheatSheetHud()
{
    cheatSheetHud->show();
    cheatSheetHud->activateWindow();
}

void MainWindow::onQuickRefGuideLinkClicked(const QUrl& url)
{
    QDesktopServices::openUrl(url);
}

void MainWindow::showAbout()
{
    QString aboutText =
        QString("<p><b>") +  qAppName() + QString(" ")
        + qApp->applicationVersion() + QString("</b></p>")
        + tr("<p>Copyright &copy; 2014-2016 wereturtle</b>"
             "<p>You may use and redistribute this software under the terms of the "
             "<a href=\"http://www.gnu.org/licenses/gpl.html\">"
             "GNU General Public License Version 3</a>.</p>"
             "<p>Visit the official website at "
             "<a href=\"http://github.com/wereturtle/ghostwriter\">"
             "http://github.com/wereturtle/ghostwriter</a>.</p>"
             "<p>Special thanks and credit for reused code goes to</p>"
             "<p><a href=\"mailto:graeme@gottcode.org\">Graeme Gott</a>, "
             "author of "
             "<a href=\"http://gottcode.org/focuswriter/\">FocusWriter</a><br/>"
             "Dmitry Shachnev, author of "
             "<a href=\"http://sourceforge.net/p/retext/home/ReText/\">Retext</a><br/>"
             "<a href=\"mailto:gabriel@teuton.org\">Gabriel M. Beddingfield</a>, "
             "author of <a href=\"http://www.teuton.org/~gabriel/stretchplayer/\">"
             "StretchPlayer</a><br/>"
             "<p>I am also deeply indebted to "
             "<a href=\"mailto:w.vollprecht@gmail.com\">Wolf Vollprecht</a>, "
             "the author of "
             "<a href=\"http://uberwriter.wolfvollprecht.de/\">UberWriter</a>, "
             "for the inspiration he provided in creating such a beautiful "
             "Markdown editing tool.</p>");

    QMessageBox::about(this, tr("About %1").arg(qAppName()), aboutText);
}

void MainWindow::updateWordCount(int newWordCount)
{
    wordCountLabel->setText(tr("%n word(s)", "", newWordCount));
}

void MainWindow::changeFocusMode(QAction* action)
{
    appSettings->setFocusMode((FocusMode) action->data().toInt());

    if (FocusModeDisabled != editor->getFocusMode())
    {
        editor->setFocusMode(appSettings->getFocusMode());
    }
}

void MainWindow::applyTheme(const Theme& theme)
{
    this->theme = theme;
    applyTheme();
}

void MainWindow::openHtmlPreview()
{
    if (!htmlPreview->isVisible())
    {
        htmlPreview->show();
        htmlPreview->updatePreview();
    }
    else
    {
        htmlPreview->activateWindow();
    }
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(this->sender());

    if (NULL != action)
    {
        documentManager->open(action->text());
    }
}

void MainWindow::refreshRecentFiles()
{
    if (appSettings->getFileHistoryEnabled())
    {
        DocumentHistory history;
        QStringList recentFiles = history.getRecentFiles(MAX_RECENT_FILES + 1);
        TextDocument* document = documentManager->getDocument();

        if (!document->isNew())
        {
            QString sanitizedPath =
                QFileInfo(document->getFilePath()).absoluteFilePath();
            recentFiles.removeAll(sanitizedPath);
        }

        for (int i = 0; (i < MAX_RECENT_FILES) && (i < recentFiles.size()); i++)
        {
            recentFilesActions[i]->setText(recentFiles.at(i));
            recentFilesActions[i]->setVisible(true);
        }

        for (int i = recentFiles.size(); i < MAX_RECENT_FILES; i++)
        {
            recentFilesActions[i]->setVisible(false);
        }
    }
}

void MainWindow::clearRecentFileHistory()
{
    DocumentHistory history;
    history.clear();

    for (int i = 0; i < MAX_RECENT_FILES; i++)
    {
        recentFilesActions[i]->setVisible(false);
    }
}

void MainWindow::changeDocumentDisplayName(const QString& displayName)
{
    setWindowTitle(displayName + QString("[*] - ") + qAppName());

    if (documentManager->getDocument()->isModified())
    {
        setWindowModified(!appSettings->getAutoSaveEnabled());
    }
    else
    {
        setWindowModified(false);
    }
}

void MainWindow::onOperationStarted(const QString& description)
{
    statusLabel->setText(description);
    statusLabel->show();
    this->update();
    qApp->processEvents();
}

void MainWindow::onOperationFinished()
{
    statusLabel->setText(QString());
    statusLabel->hide();
    this->update();
    qApp->processEvents();
}

void MainWindow::changeFont()
{
    bool success;

    QFont font =
        SimpleFontDialog::getFont(&success, editor->font(), this);

    if (success)
    {
        editor->setFont(font.family(), font.pointSize());
        appSettings->setFont(font);
    }
}

void MainWindow::onSetDictionary()
{
    DictionaryDialog dictionaryDialog(language, this);
    int status = dictionaryDialog.exec();

    if (QDialog::Accepted == status)
    {
        language = dictionaryDialog.getLanguage();
        DictionaryManager::instance().setDefaultLanguage(language);
        editor->setDictionary(DictionaryManager::instance().requestDictionary(language));
        appSettings->setDictionaryLanguage(language);
    }
}

void MainWindow::showHudOpacityDialog()
{
    int opacity = appSettings->getHudOpacity();

    if (NULL == hudOpacityDialog)
    {
        hudOpacityDialog = new QDialog(this);
        hudOpacityDialog->setWindowTitle(tr("Hud Window Opacity"));

        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setMinimum(0);
        slider->setMaximum(255);
        slider->setValue(opacity);

        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(slider);
        hudOpacityDialog->setLayout(layout);

        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(changeHudOpacity(int)));
    }

    hudOpacityDialog->show();
    hudOpacityDialog->raise();
    hudOpacityDialog->activateWindow();
}

void MainWindow::changeHudOpacity(int value)
{
    QColor color = outlineHud->getBackgroundColor();
    color.setAlpha(value);
    outlineHud->setBackgroundColor(color);
    outlineHud->update();
    cheatSheetHud->setBackgroundColor(color);
    cheatSheetHud->update();

    appSettings->setHudOpacity(value);
}

QAction* MainWindow::addMenuAction
(
    QMenu* menu,
    const QString& name,
    const QString& shortcut,
    bool checkable,
    bool checked,
    QActionGroup* actionGroup
)
{
    QAction* action = new QAction(name, this);

    if (0 != shortcut)
    {
        action->setShortcut(shortcut);
    }

    action->setCheckable(checkable);
    action->setChecked(checked);

    if (0 != actionGroup)
    {
        action->setActionGroup(actionGroup);
    }

    menu->addAction(action);

    return action;
}

void MainWindow::buildMenuBar()
{
    effectsMenuBar = new EffectsMenuBar(this);
    this->setMenuBar(effectsMenuBar);

    QMenu* fileMenu = this->menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(tr("&New"), documentManager, SLOT(close()), QKeySequence::New);
    fileMenu->addAction(tr("&Open"), documentManager, SLOT(open()), QKeySequence::Open);

    QMenu* recentFilesMenu = new QMenu(tr("Open &Recent..."));
    recentFilesMenu->addAction(tr("Reopen Closed File"), documentManager, SLOT(reopenLastClosedFile()), QKeySequence("SHIFT+CTRL+T"));
    recentFilesMenu->addSeparator();

    for (int i = 0; i < MAX_RECENT_FILES; i++)
    {
        recentFilesMenu->addAction(recentFilesActions[i]);
    }

    recentFilesMenu->addSeparator();
    recentFilesMenu->addAction(tr("Clear Menu"), this, SLOT(clearRecentFileHistory()));

    fileMenu->addMenu(recentFilesMenu);

    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Save"), documentManager, SLOT(save()), QKeySequence::Save);
    fileMenu->addAction(tr("Save &As..."), documentManager, SLOT(saveAs()), QKeySequence::SaveAs);
    fileMenu->addAction(tr("R&ename..."), documentManager, SLOT(rename()));
    fileMenu->addAction(tr("Re&load from Disk..."), documentManager, SLOT(reload()));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Print Pre&view"), documentManager, SLOT(printPreview()), QKeySequence("SHIFT+CTRL+P"));
    fileMenu->addAction(tr("&Print"), documentManager, SLOT(print()), QKeySequence::Print);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Export"), documentManager, SLOT(exportFile()), QKeySequence("CTRL+E"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), this, SLOT(quitApplication()), QKeySequence::Quit);

    QMenu* editMenu = this->menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Undo"), editor, SLOT(undo()), QKeySequence::Undo);
    editMenu->addAction(tr("&Redo"), editor, SLOT(redo()), QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction(tr("Cu&t"), editor, SLOT(cut()), QKeySequence::Cut);
    editMenu->addAction(tr("&Copy"), editor, SLOT(copy()), QKeySequence::Copy);
    editMenu->addAction(tr("&Paste"), editor, SLOT(paste()), QKeySequence::Paste);
    editMenu->addSeparator();
    editMenu->addAction(tr("&Insert Image..."), this, SLOT(insertImage()));
    editMenu->addSeparator();
    editMenu->addAction(tr("&Find"), findReplaceDialog, SLOT(showFindMode()), QKeySequence::Find);
    editMenu->addAction(tr("Rep&lace"), findReplaceDialog, SLOT(showReplaceMode()), QKeySequence::Replace);
    editMenu->addSeparator();
    editMenu->addAction(tr("&Spell check"), editor, SLOT(runSpellChecker()));

    QMenu* formatMenu = this->menuBar()->addMenu(tr("For&mat"));
    formatMenu->addAction(tr("&Bold"), editor, SLOT(bold()), QKeySequence::Bold);
    formatMenu->addAction(tr("&Italic"), editor, SLOT(italic()), QKeySequence::Italic);
    formatMenu->addAction(tr("Stri&kthrough"), editor, SLOT(strikethrough()), QKeySequence("Ctrl+K"));
    formatMenu->addAction(tr("&HTML Comment"), editor, SLOT(insertComment()), QKeySequence("Ctrl+/"));
    formatMenu->addSeparator();
    formatMenu->addAction(tr("I&ndent"), editor, SLOT(indentText()), QKeySequence("Tab"));
    formatMenu->addAction(tr("&Unindent"), editor, SLOT(unindentText()), QKeySequence("Shift+Tab"));
    formatMenu->addSeparator();
    formatMenu->addAction(tr("Block &Quote"), editor, SLOT(createBlockquote()), QKeySequence("Ctrl+."));
    formatMenu->addAction(tr("&Strip Block Quote"), editor, SLOT(removeBlockquote()), QKeySequence("Ctrl+,"));
    formatMenu->addSeparator();
    formatMenu->addAction(tr("&* Bullet List"), editor, SLOT(createBulletListWithAsteriskMarker()), QKeySequence("Alt+8"));
    formatMenu->addAction(tr("&- Bullet List"), editor, SLOT(createBulletListWithMinusMarker()), QKeySequence("Alt+-"));
    formatMenu->addAction(tr("&+ Bullet List"), editor, SLOT(createBulletListWithPlusMarker()), QKeySequence("Alt+="));
    formatMenu->addSeparator();
    formatMenu->addAction(tr("1&. Numbered List"), editor, SLOT(createNumberedListWithPeriodMarker()), QKeySequence("Alt+1"));
    formatMenu->addAction(tr("1&) Numbered List"), editor, SLOT(createNumberedListWithParenthesisMarker()), QKeySequence("Alt+0"));
    formatMenu->addSeparator();
    formatMenu->addAction(tr("&Task List"), editor, SLOT(createTaskList()), QKeySequence("Ctrl+T"));
    formatMenu->addAction(tr("Toggle Task(s) &Complete"), editor, SLOT(toggleTaskComplete()), QKeySequence("Ctrl+D"));

    QMenu* viewMenu = this->menuBar()->addMenu(tr("&View"));

    fullScreenMenuAction = new QAction(tr("&Full Screen"), this);
    fullScreenMenuAction->setCheckable(true);
    fullScreenMenuAction->setChecked(this->isFullScreen());
    fullScreenMenuAction->setShortcut(QKeySequence("F11"));
    connect(fullScreenMenuAction, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreen(bool)));
    viewMenu->addAction(fullScreenMenuAction);

    viewMenu->addAction(tr("&Preview in HTML"), this, SLOT(openHtmlPreview()), QKeySequence("CTRL+W"));
    viewMenu->addAction(tr("&Outline HUD"), this, SLOT(showOutlineHud()), QKeySequence("CTRL+L"));
    viewMenu->addAction(tr("&Cheat Sheet HUD"), this, SLOT(showCheatSheetHud()));
    viewMenu->addSeparator();

    QMenu* settingsMenu = this->menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(tr("Themes..."), this, SLOT(changeTheme()));
    settingsMenu->addAction(tr("Font..."), this, SLOT(changeFont()));

    QMenu* focusModeMenu = new QMenu(tr("Focus Mode"));

    QActionGroup* focusModeGroup = new QActionGroup(this);
    connect(focusModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeFocusMode(QAction*)));

    QAction* focusSentenceAction = new QAction(tr("Sentence"), this);
    focusSentenceAction->setCheckable(true);
    focusSentenceAction->setChecked(appSettings->getFocusMode() == FocusModeSentence);
    focusSentenceAction->setActionGroup(focusModeGroup);
    focusSentenceAction->setData(QVariant(FocusModeSentence));

    QAction* focusCurrentLineAction = new QAction(tr("Current Line"), this);
    focusCurrentLineAction->setCheckable(true);
    focusCurrentLineAction->setChecked(appSettings->getFocusMode() == FocusModeCurrentLine);
    focusCurrentLineAction->setActionGroup(focusModeGroup);
    focusCurrentLineAction->setData(QVariant(FocusModeCurrentLine));

    QAction* focusThreeLinesAction = new QAction(tr("Three Lines"), this);
    focusThreeLinesAction->setCheckable(true);
    focusThreeLinesAction->setChecked(appSettings->getFocusMode() == FocusModeThreeLines);
    focusThreeLinesAction->setActionGroup(focusModeGroup);
    focusThreeLinesAction->setData(QVariant(FocusModeThreeLines));

    QAction* focusParagraphAction = new QAction(tr("Paragraph"), this);
    focusParagraphAction->setCheckable(true);
    focusParagraphAction->setChecked(appSettings->getFocusMode() == FocusModeParagraph);
    focusParagraphAction->setActionGroup(focusModeGroup);
    focusParagraphAction->setData(QVariant(FocusModeParagraph));

    focusModeMenu->addAction(focusSentenceAction);
    focusModeMenu->addAction(focusCurrentLineAction);
    focusModeMenu->addAction(focusThreeLinesAction);
    focusModeMenu->addAction(focusParagraphAction);

    settingsMenu->addMenu(focusModeMenu);

    QMenu* editorWidthMenu = new QMenu(tr("Editor Width"));
    QActionGroup* editorWidthGroup = new QActionGroup(this);
    connect(editorWidthGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeEditorWidth(QAction*)));

    QAction* narrowAction = new QAction(tr("Narrow"), this);
    narrowAction->setCheckable(true);
    narrowAction->setChecked(appSettings->getEditorWidth() == EditorWidthNarrow);
    narrowAction->setActionGroup(editorWidthGroup);
    narrowAction->setData(QVariant(EditorWidthNarrow));

    QAction* mediumAction = new QAction(tr("Medium"), this);
    mediumAction->setCheckable(true);
    mediumAction->setChecked(appSettings->getEditorWidth() == EditorWidthMedium);
    mediumAction->setActionGroup(editorWidthGroup);
    mediumAction->setData(QVariant(EditorWidthMedium));

    QAction* wideAction = new QAction(tr("Wide"), this);
    wideAction->setCheckable(true);
    wideAction->setChecked(appSettings->getEditorWidth() == EditorWidthWide);
    wideAction->setActionGroup(editorWidthGroup);
    wideAction->setData(QVariant(EditorWidthWide));

    QAction* fullAction = new QAction(tr("Full"), this);
    fullAction->setCheckable(true);
    fullAction->setChecked(appSettings->getEditorWidth() == EditorWidthFull);
    fullAction->setActionGroup(editorWidthGroup);
    fullAction->setData(QVariant(EditorWidthFull));

    editorWidthMenu->addAction(narrowAction);
    editorWidthMenu->addAction(mediumAction);
    editorWidthMenu->addAction(wideAction);
    editorWidthMenu->addAction(fullAction);

    settingsMenu->addMenu(editorWidthMenu);

    QMenu* blockquoteStyleMenu = new QMenu(tr("Blockquote Style"));
    QActionGroup* blockquoteStyleGroup = new QActionGroup(this);
    connect(blockquoteStyleGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeBlockquoteStyle(QAction*)));

    QAction* plainBlockquoteAction = new QAction(tr("Plain"), this);
    plainBlockquoteAction->setCheckable(true);
    plainBlockquoteAction->setChecked(appSettings->getBlockquoteStyle() == BlockquoteStylePlain);
    plainBlockquoteAction->setActionGroup(blockquoteStyleGroup);
    plainBlockquoteAction->setData(QVariant(BlockquoteStylePlain));

    QAction* italicBlockquoteAction = new QAction(tr("Italic"), this);
    italicBlockquoteAction->setCheckable(true);
    italicBlockquoteAction->setChecked(appSettings->getBlockquoteStyle() == BlockquoteStyleItalic);
    italicBlockquoteAction->setActionGroup(blockquoteStyleGroup);
    italicBlockquoteAction->setData(QVariant(BlockquoteStyleItalic));

    QAction* fancyBlockquoteAction = new QAction(tr("Fancy"), this);
    fancyBlockquoteAction->setCheckable(true);
    fancyBlockquoteAction->setChecked(appSettings->getBlockquoteStyle() == BlockquoteStyleFancy);
    fancyBlockquoteAction->setActionGroup(blockquoteStyleGroup);
    fancyBlockquoteAction->setData(QVariant(BlockquoteStyleFancy));

    blockquoteStyleMenu->addAction(plainBlockquoteAction);
    blockquoteStyleMenu->addAction(italicBlockquoteAction);
    blockquoteStyleMenu->addAction(fancyBlockquoteAction);

    settingsMenu->addMenu(blockquoteStyleMenu);

    bool autoHideEnabled = appSettings->getHideMenuBarInFullScreenEnabled();
    QAction* autoHideAction = new QAction(tr("Hide menu bar in full screen mode"), this);
    autoHideAction->setCheckable(true);
    autoHideAction->setChecked(autoHideEnabled);
    connect(autoHideAction, SIGNAL(toggled(bool)), this, SLOT(toggleHideMenuBarInFullScreen(bool)));
    settingsMenu->addAction(autoHideAction);

    bool largeHeadingsEnabled = appSettings->getLargeHeadingSizesEnabled();
    QAction* largeHeadingsAction = new QAction(tr("Use Large Headings"), this);
    largeHeadingsAction->setCheckable(true);
    largeHeadingsAction->setChecked(largeHeadingsEnabled);
    connect(largeHeadingsAction, SIGNAL(toggled(bool)), this, SLOT(toggleLargeLeadingSizes(bool)));
    settingsMenu->addAction(largeHeadingsAction);

    bool underlineEnabled = appSettings->getUseUnderlineForEmphasis();
    QAction* underlineAction = new QAction(tr("Use Underline Instead of Italics for Emphasis"), this);
    underlineAction->setCheckable(true);
    underlineAction->setChecked(underlineEnabled);
    connect(underlineAction, SIGNAL(toggled(bool)), this, SLOT(toggleUseUnderlineForEmphasis(bool)));
    settingsMenu->addAction(underlineAction);

    bool autoMatchEnabled = appSettings->getAutoMatchEnabled();
    QAction* autoMatchAction = new QAction(tr("Automatically Match Characters while Typing"), this);
    autoMatchAction->setCheckable(true);
    autoMatchAction->setChecked(autoMatchEnabled);
    connect(autoMatchAction, SIGNAL(toggled(bool)), this, SLOT(toggleAutoMatch(bool)));
    settingsMenu->addAction(autoMatchAction);

    bool bulletCyclingEnabled = appSettings->getBulletPointCyclingEnabled();
    QAction* bulletCycleAction = new QAction(tr("Cycle Bullet Point Markers"), this);
    bulletCycleAction->setCheckable(true);
    bulletCycleAction->setChecked(bulletCyclingEnabled);
    connect(bulletCycleAction, SIGNAL(toggled(bool)), this, SLOT(toggleBulletPointCycling(bool)));
    settingsMenu->addAction(bulletCycleAction);

    bool displayTimeEnabled = appSettings->getDisplayTimeInFullScreenEnabled();
    QAction* displayTimeAction = new QAction(tr("Display Current Time in Full Screen Mode"), this);
    displayTimeAction->setCheckable(true);
    displayTimeAction->setChecked(displayTimeEnabled);
    connect(displayTimeAction, SIGNAL(toggled(bool)), this, SLOT(toggleDisplayTimeInFullScreen(bool)));
    settingsMenu->addAction(displayTimeAction);

    settingsMenu->addSeparator();

    QAction* liveSpellcheckAction = new QAction(tr("Live Spellcheck Enabled"), this);
    liveSpellcheckAction->setCheckable(true);
    liveSpellcheckAction->setChecked(appSettings->getLiveSpellCheckEnabled());
    connect(liveSpellcheckAction, SIGNAL(toggled(bool)), this, SLOT(toggleLiveSpellCheck(bool)));
    settingsMenu->addAction(liveSpellcheckAction);

    settingsMenu->addAction(tr("Dictionaries..."), this, SLOT(onSetDictionary()));

    settingsMenu->addSeparator();

    QAction* fileHistoryAction = new QAction(tr("Remember File History"), this);
    fileHistoryAction->setCheckable(true);
    fileHistoryAction->setChecked(appSettings->getFileHistoryEnabled());
    connect(fileHistoryAction, SIGNAL(toggled(bool)), this, SLOT(toggleFileHistoryEnabled(bool)));
    settingsMenu->addAction(fileHistoryAction);

    QAction* autoSaveAction = new QAction(tr("Auto Save"), this);
    autoSaveAction->setCheckable(true);
    autoSaveAction->setChecked(appSettings->getAutoSaveEnabled());
    connect(autoSaveAction, SIGNAL(toggled(bool)), documentManager, SLOT(setAutoSaveEnabled(bool)));
    settingsMenu->addAction(autoSaveAction);

    QAction* backupFileAction = new QAction(tr("Backup File on Save"), this);
    backupFileAction->setCheckable(true);
    backupFileAction->setChecked(appSettings->getBackupFileEnabled());
    connect(backupFileAction, SIGNAL(toggled(bool)), documentManager, SLOT(setFileBackupEnabled(bool)));
    settingsMenu->addAction(backupFileAction);

    settingsMenu->addSeparator();

    bool insertSpacesForTabs = appSettings->getInsertSpacesForTabsEnabled();
    QAction* spacesForTabsAction = new QAction(tr("Insert Spaces for Tabs"), this);
    spacesForTabsAction->setCheckable(true);
    spacesForTabsAction->setChecked(insertSpacesForTabs);
    connect(spacesForTabsAction, SIGNAL(toggled(bool)), this, SLOT(toggleSpacesForTabs(bool)));
    settingsMenu->addAction(spacesForTabsAction);
    editor->setInsertSpacesForTabs(insertSpacesForTabs);

    settingsMenu->addAction(tr("Tabulation Width..."), this, SLOT(changeTabulationWidth()));
    editor->setTabulationWidth(appSettings->getTabWidth());

    settingsMenu->addSeparator();

    QAction* outlineAlternateColorsAction = new QAction(tr("Alternate Row Colors in HUD Windows"), this);
    outlineAlternateColorsAction->setCheckable(true);
    outlineAlternateColorsAction->setChecked(appSettings->getAlternateHudRowColorsEnabled());
    connect(outlineAlternateColorsAction, SIGNAL(toggled(bool)), this, SLOT(toggleOutlineAlternateRowColors(bool)));
    settingsMenu->addAction(outlineAlternateColorsAction);
    outlineWidget->setAlternatingRowColors(outlineAlternateColorsAction->isChecked());
    cheatSheetWidget->setAlternatingRowColors(outlineAlternateColorsAction->isChecked());

    QMenu* hudButtonLayoutMenu = new QMenu(tr("HUD Window Button Layout"));
    QActionGroup* hudButtonLayoutGroup = new QActionGroup(this);
    connect(hudButtonLayoutGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeHudButtonLayout(QAction*)));

    QAction* hudButtonLayoutLeftAction = new QAction(tr("Left"), this);
    hudButtonLayoutLeftAction->setCheckable(true);
    hudButtonLayoutLeftAction->setChecked(appSettings->getHudButtonLayout() == HudWindowButtonLayoutLeft);
    hudButtonLayoutLeftAction->setActionGroup(hudButtonLayoutGroup);
    hudButtonLayoutLeftAction->setData(HudWindowButtonLayoutLeft);

    QAction* hudButtonLayoutRightAction = new QAction(tr("Right"), this);
    hudButtonLayoutRightAction->setCheckable(true);
    hudButtonLayoutRightAction->setChecked(appSettings->getHudButtonLayout() == HudWindowButtonLayoutRight);
    hudButtonLayoutRightAction->setActionGroup(hudButtonLayoutGroup);
    hudButtonLayoutRightAction->setData(HudWindowButtonLayoutRight);

    hudButtonLayoutMenu->addAction(hudButtonLayoutLeftAction);
    hudButtonLayoutMenu->addAction(hudButtonLayoutRightAction);

    settingsMenu->addMenu(hudButtonLayoutMenu);

    QAction* desktopCompositingAction = new QAction(tr("Enable Desktop Compositing Effects"), this);
    desktopCompositingAction->setCheckable(true);
    desktopCompositingAction->setChecked(appSettings->getDesktopCompositingEnabled());
    outlineHud->setDesktopCompositingEnabled(desktopCompositingAction->isChecked());
    cheatSheetHud->setDesktopCompositingEnabled(desktopCompositingAction->isChecked());
    connect(desktopCompositingAction, SIGNAL(toggled(bool)), this, SLOT(toggleDesktopCompositingEffects(bool)));
    settingsMenu->addAction(desktopCompositingAction);

    settingsMenu->addAction(tr("HUD Window Opacity..."), this, SLOT(showHudOpacityDialog()));

    QMenu* helpMenu = this->menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, SLOT(showAbout()));
    helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
    helpMenu->addAction(tr("Quick &Reference Guide"), this, SLOT(showQuickReferenceGuide()));

    connect(fileMenu, SIGNAL(aboutToShow()), effectsMenuBar, SLOT(onAboutToShow()));
    connect(fileMenu, SIGNAL(aboutToHide()), effectsMenuBar, SLOT(onAboutToHide()));
    connect(editMenu, SIGNAL(aboutToShow()), effectsMenuBar, SLOT(onAboutToShow()));
    connect(editMenu, SIGNAL(aboutToHide()), effectsMenuBar, SLOT(onAboutToHide()));
    connect(formatMenu, SIGNAL(aboutToShow()), effectsMenuBar, SLOT(onAboutToShow()));
    connect(formatMenu, SIGNAL(aboutToHide()), effectsMenuBar, SLOT(onAboutToHide()));
    connect(viewMenu, SIGNAL(aboutToShow()), effectsMenuBar, SLOT(onAboutToShow()));
    connect(viewMenu, SIGNAL(aboutToHide()), effectsMenuBar, SLOT(onAboutToHide()));
    connect(settingsMenu, SIGNAL(aboutToShow()), effectsMenuBar, SLOT(onAboutToShow()));
    connect(settingsMenu, SIGNAL(aboutToHide()), effectsMenuBar, SLOT(onAboutToHide()));
    connect(helpMenu, SIGNAL(aboutToShow()), effectsMenuBar, SLOT(onAboutToShow()));
    connect(helpMenu, SIGNAL(aboutToHide()), effectsMenuBar, SLOT(onAboutToHide()));
}

void MainWindow::buildStatusBar()
{
    statusBarWidget = new QFrame();
    QGridLayout* statusBarLayout = new QGridLayout(statusBarWidget);

    statusLabel = new QLabel();
    statusBarLayout->addWidget(statusLabel, 0, 0, Qt::AlignLeft);
    statusBarLayout->setColumnStretch(0, 0);
    statusLabel->setStyleSheet("color: white; background-color: black; border-radius: 5px; padding: 3px");
    statusLabel->hide();

    wordCountLabel = new QLabel();
    wordCountLabel->setAlignment(Qt::AlignCenter);
    wordCountLabel->setFrameShape(QFrame::NoFrame);
    wordCountLabel->setLineWidth(0);
    updateWordCount(0);
    statusBarLayout->addWidget(wordCountLabel, 0, 1, Qt::AlignCenter);
    statusBarLayout->setColumnStretch(1, 1);

    timeLabel = new TimeLabel(this);
    statusBar()->addPermanentWidget(timeLabel);

    if (!this->isFullScreen() || !appSettings->getDisplayTimeInFullScreenEnabled())
    {
        timeLabel->hide();
    }

    QPushButton* focusModeButton = new QPushButton(tr("Focus"));
    focusModeButton->setFocusPolicy(Qt::NoFocus);
    focusModeButton->setToolTip(tr("Toggle distraction free mode"));
    focusModeButton->setCheckable(true);
    connect(focusModeButton, SIGNAL(toggled(bool)), this, SLOT(toggleFocusMode(bool)));
    statusBar()->addPermanentWidget(focusModeButton);

    fullScreenButton = new QCheckBox();
    fullScreenButton->setFocusPolicy(Qt::NoFocus);
    fullScreenButton->setObjectName("fullscreenButton");
    fullScreenButton->setToolTip(tr("Toggle full screen mode"));
    fullScreenButton->setCheckable(true);
    fullScreenButton->setChecked(this->isFullScreen());
    connect(fullScreenButton, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreen(bool)));
    statusBar()->addPermanentWidget(fullScreenButton);

    statusBarLayout->setSpacing(0);
    statusBarWidget->setLayout(statusBarLayout);
    statusBarWidget->setObjectName("statusbar");
    statusBarWidget->setStyleSheet("background: transparent; border: 0");
    statusBar()->addWidget(statusBarWidget, 1);
    statusBar()->setSizeGripEnabled(false);

    // The permanent widgets added to the right of the status bar offset the centering
    // of the word count.  Set the minimum column width of the status label
    // (the leftmost column in the layout) to compensate and recenter the word count.
    //
    statusBarLayout->setColumnMinimumWidth
    (
        0,
        focusModeButton->sizeHint().width()
        +
        fullScreenButton->sizeHint().width()
    );

    statusBar()->show();
}

void MainWindow::applyTheme()
{
    if (!theme.getName().isNull() && !theme.getName().isEmpty())
    {
        appSettings->setThemeName(theme.getName());
    }

    if (EditorAspectStretch == theme.getEditorAspect())
    {
        theme.setBackgroundColor(theme.getEditorBackgroundColor());
    }

    QString styleSheet;
    QTextStream stream(&styleSheet);

    QColor scrollBarColor(theme.getDefaultTextColor());
    scrollBarColor.setAlpha(100);
    scrollBarColor =
        ColorHelper::applyAlpha
        (
            scrollBarColor,
            theme.getEditorBackgroundColor()
        );

    QString scrollbarColorRGB = ColorHelper::toRgbString(scrollBarColor);

    QString backgroundColorRGBA;

    if (EditorAspectStretch == theme.getEditorAspect())
    {
        backgroundColorRGBA = "transparent";
    }
    else
    {
        backgroundColorRGBA =
            ColorHelper::toRgbaString(theme.getEditorBackgroundColor());
    }

    QString editorSelectionFgColorRGB =
        ColorHelper::toRgbString(theme.getEditorBackgroundColor());

    QString editorSelectionBgColorRGB =
        ColorHelper::toRgbString(theme.getDefaultTextColor());

    QString menuBarItemFgColorRGB;
    QString menuBarItemBgColorRGBA;
    QString menuBarItemFgPressColorRGB;
    QString menuBarItemBgPressColorRGBA;

    QString fullScreenIcon;
    QString fullScreenIconHover;
    QString fullScreenIconPressed;

    QString restoreIcon;
    QString restoreIconHover;
    QString restoreIconPressed;

    QString statusBarItemFgColorRGB;
    QString statusBarButtonFgPressHoverColorRGB;
    QString statusBarButtonBgPressHoverColorRGBA;

    if (EditorAspectStretch == theme.getEditorAspect())
    {
        QColor fadedTextColor(theme.getDefaultTextColor());
        fadedTextColor.setAlpha(165);
        fadedTextColor =
            ColorHelper::applyAlpha
            (
                fadedTextColor,
                theme.getBackgroundColor()
            );

        menuBarItemFgColorRGB = ColorHelper::toRgbString(fadedTextColor);
        menuBarItemFgColorRGB = ColorHelper::toRgbString(fadedTextColor);
        menuBarItemBgColorRGBA = "transparent";
        menuBarItemFgPressColorRGB =
            ColorHelper::toRgbString(theme.getEditorBackgroundColor());
        menuBarItemBgPressColorRGBA =
            ColorHelper::toRgbaString(fadedTextColor);

        fullScreenIcon = ":/resources/images/view-fullscreen-dark.svg";
        fullScreenIconHover = ":/resources/images/view-fullscreen-dark-hover.svg";
        fullScreenIconPressed = ":/resources/images/view-fullscreen-dark-hover.svg";
        restoreIcon = ":/resources/images/view-restore-dark.svg";
        restoreIconHover = ":/resources/images/view-restore-dark-hover.svg";
        restoreIconPressed = ":/resources/images/view-restore-dark-hover.svg";

        statusBarItemFgColorRGB = menuBarItemFgColorRGB;
        statusBarButtonFgPressHoverColorRGB =
            ColorHelper::toRgbString(theme.getEditorBackgroundColor());
        statusBarButtonBgPressHoverColorRGBA = menuBarItemBgPressColorRGBA;

        if (NULL == fullScreenButton->graphicsEffect())
        {
            // We can't reuse the old QGraphicsColorizeEffect once it's
            // been removed from the button, since Qt seems to delete it
            // at an arbitrary time, sooner or later causing a crash while
            // switching themes.  Thus, create a new one.
            //
            fullScreenButtonColorEffect = new QGraphicsColorizeEffect();
            fullScreenButton->setGraphicsEffect(fullScreenButtonColorEffect);
        }

        fullScreenButtonColorEffect->setColor(fadedTextColor);
    }
    else
    {
        QColor chromeFgColor = QColor("#F2F2F2");

        menuBarItemFgColorRGB = ColorHelper::toRgbString(chromeFgColor);
        menuBarItemBgColorRGBA = "transparent";
        menuBarItemFgPressColorRGB = menuBarItemFgColorRGB;
        chromeFgColor.setAlpha(80);
        menuBarItemBgPressColorRGBA = ColorHelper::toRgbaString(chromeFgColor);


        fullScreenIcon = ":/resources/images/view-fullscreen-light.svg";
        fullScreenIconHover = ":/resources/images/view-fullscreen-light-hover.svg";
        fullScreenIconPressed = ":/resources/images/view-fullscreen-light-hover.svg";
        restoreIcon = ":/resources/images/view-restore-light.svg";
        restoreIconHover = ":/resources/images/view-restore-light-hover.svg";
        restoreIconPressed = ":/resources/images/view-restore-light-hover.svg";

        statusBarItemFgColorRGB = menuBarItemFgColorRGB;
        statusBarButtonFgPressHoverColorRGB = menuBarItemFgPressColorRGB;
        statusBarButtonBgPressHoverColorRGBA = menuBarItemBgPressColorRGBA;

        // Do NOT delete the old QGraphicsColorizeEffect.  Qt seems to
        // delete it at an arbitrary time, based on parental ownership.
        //
        fullScreenButton->setGraphicsEffect(NULL);
    }

    editor->setAspect(theme.getEditorAspect());

    styleSheet = "";

    QString corners = "";

    if
    (
        (EditorCornersRounded == theme.getEditorCorners()) &&
        (EditorAspectStretch != theme.getEditorAspect())
    )
    {
        corners = "border-radius: 10;";
    }

    QString cursorColorRGB =
        ColorHelper::toRgbString(theme.getLinkColor());

    stream
        << "QPlainTextEdit { border: 0; "
        << corners
        << "margin: 0; padding: 5px; background-color: "
        << backgroundColorRGBA
        // The following two lines only set the cursor color, since the
        // Highlighter always colors the default character format for each block
        // with the theme's default text color.
        << "; color: "
        << cursorColorRGB
        << "; selection-color: "
        << editorSelectionFgColorRGB
        << "; selection-background-color: "
        << editorSelectionBgColorRGB
        << " } "
        << "QAbstractScrollArea::corner { background: transparent } "
        << "QScrollBar::horizontal { border: 0; background: transparent; height: 10px; margin: 0 } "
        << "QScrollBar::handle:horizontal { border: 0; background: "
        << scrollbarColorRGB
        << "; min-width: 50px; border-radius: 5px; } "
        << "QScrollBar::vertical { border: 0; background: transparent; width: 10px; margin: 0 } "
        << "QScrollBar::handle:vertical { border: 0; background: "
        << scrollbarColorRGB
        << "; min-height: 50px; border-radius: 5px;} "
        << "QScrollBar::add-line { background: transparent; border: 0 } "
        << "QScrollBar::sub-line { background: transparent; border: 0 } "
        ;

    editor->setColorScheme
    (
        theme.getDefaultTextColor(),
        theme.getEditorBackgroundColor(),
        theme.getMarkupColor(),
        theme.getLinkColor(),
        theme.getSpellingErrorColor()
    );
    editor->setStyleSheet(styleSheet);

    styleSheet = "";

    stream
        << "QCheckBox { background: transparent; padding: 3 3 3 3; margin: 0 5 0 5 } "
        << "QCheckBox::indicator { width: 20px; height: 20px; background: transparent } "
        << "QCheckBox::indicator:unchecked { image: url("
        << fullScreenIcon
        << ") } "
        << "QCheckBox::indicator:unchecked:hover { image: url("
        << fullScreenIconHover
        << ") } "
        << "QCheckBox::indicator:unchecked:pressed { image: url("
        << fullScreenIconPressed
        << ") } "
        << "QCheckBox::indicator:checked { image: url("
        << restoreIcon
        << ") } "
        << "QCheckBox::indicator:checked:hover { image: url("
        << restoreIconHover
        << ") } "
        << "QCheckBox::indicator:checked:pressed { image: url("
        << restoreIconHover
        << ") } "
        ;

    fullScreenButton->setStyleSheet(styleSheet);

    if (EditorAspectCenter == theme.getEditorAspect())
    {
        QGraphicsDropShadowEffect* chromeDropShadowEffect = new QGraphicsDropShadowEffect();
        chromeDropShadowEffect->setColor(QColor(Qt::black));
        chromeDropShadowEffect->setBlurRadius(3.5);
        chromeDropShadowEffect->setXOffset(1.0);
        chromeDropShadowEffect->setYOffset(1.0);

        this->statusBar()->setGraphicsEffect(chromeDropShadowEffect);
        effectsMenuBar->setDropShadow(Qt::black, 3.5, 1.0, 1.0);
    }
    else
    {
        this->statusBar()->setGraphicsEffect(NULL);
        effectsMenuBar->removeDropShadow();
    }

    styleSheet = "";

    // Wipe out old background image drawing material.
    originalBackgroundImage = QImage();
    adjustedBackgroundImage = QImage();

    if
    (
        !theme.getBackgroundImageUrl().isNull() &&
        !theme.getBackgroundImageUrl().isEmpty()
    )
    {
        // Predraw background image for paintEvent()
        originalBackgroundImage.load(theme.getBackgroundImageUrl());
        predrawBackgroundImage();
    }

    stream
        << "#editorLayoutArea { background-color: transparent; border: 0; margin: 0 }"
        << "QMenuBar { background: transparent } "
        << "QMenuBar::item { background: "
        << menuBarItemBgColorRGBA
        << "; color: "
        << menuBarItemFgColorRGB
        << " } "
        << "QMenuBar::item:pressed { background-color: "
        << menuBarItemBgPressColorRGBA
        << "; color: "
        << menuBarItemFgPressColorRGB
        << "; border-radius: 3px"
        << " } "
        ;

    setStyleSheet(styleSheet);

    // Make the word count and focus mode button font size
    // match the menu bar's font size, since on Windows using
    // the default QLabel and QPushButton font sizes results in
    // tiny font sizes for these.  We need them to stand out
    // a little bit more than a typical label or button.
    //
    int menuBarFontSize = this->menuBar()->fontInfo().pointSize();

    styleSheet = "";

    stream
        << "QStatusBar { margin: 0; padding: 0; border: 0; background: transparent } "
        << "QStatusBar::item { border: 0 } "
        << "QLabel { font-size: "
        << menuBarFontSize
        << "pt; margin: 0; padding: 0; border: 0; background: transparent; color: "
        << statusBarItemFgColorRGB
        << " } "
        << "QPushButton { font-size: "
        << menuBarFontSize
        << "pt; padding: 5px; border: 0; border-radius: 5px; background: transparent"
        << "; color: "
        << statusBarItemFgColorRGB
        << " } "
        << "QPushButton:pressed, QPushButton:flat, QPushButton:checked, QPushButton:hover { margin: 0; color: "
        << statusBarButtonFgPressHoverColorRGB
        << "; background-color: "
        << statusBarButtonBgPressHoverColorRGBA
        << " } "
        ;

    statusBar()->setStyleSheet(styleSheet);

    styleSheet = "";

    // Style the HUDs

    QColor alphaHudBackgroundColor = theme.getHudBackgroundColor();
    alphaHudBackgroundColor.setAlpha(appSettings->getHudOpacity());

    outlineHud->setForegroundColor(theme.getHudForegroundColor());
    outlineHud->setBackgroundColor(alphaHudBackgroundColor);
    cheatSheetHud->setForegroundColor(theme.getHudForegroundColor());
    cheatSheetHud->setBackgroundColor(alphaHudBackgroundColor);

    // Style the outline itself.
    alphaHudBackgroundColor.setAlpha(0);

    QString hudFgString = ColorHelper::toRgbString(theme.getHudForegroundColor());

    QColor alphaHudSelectionColor = theme.getHudForegroundColor();
    alphaHudSelectionColor.setAlpha(200);
    QString hudSelectionFgString = ColorHelper::toRgbString(theme.getHudBackgroundColor());
    QString hudSelectionBgString = ColorHelper::toRgbaString(alphaHudSelectionColor);

    // Important!  For QListWidget (used in Outline HUD), set
    // QListWidget { outline: none } for the style sheet to get rid of the
    // focus rectangle without losing keyboard focus capability.
    // Unforntunately, this property isn't in the Qt documentation, so
    // it's being documented here for posterity's sake.
    //
    if (outlineWidget->alternatingRowColors())
    {
        stream << "QListWidget { outline: none; border: 0; padding: 1; background-color: transparent; color: "
               << hudFgString
               << "; alternate-background-color: rgba(255, 255, 255, 50)"
               << " } QListWidget::item { padding: 1 0 1 0; margin: 0; background-color: "
               << "rgba(0, 0, 0, 10)"
               << " } QListWidget::item:alternate { padding: 1; margin: 0; background-color: "
               << "rgba(255, 255, 255, 10)"
               << " } "
               << "QListWidget::item:selected { border-radius: 3px; color: "
               << hudSelectionFgString
               << "; background-color: "
               << hudSelectionBgString
               << " }"
            ;
    }
    else
    {
        stream << "QListWidget { outline: none; border: 0; padding: 1; background-color: transparent; color: "
               << hudFgString
               << " } QListWidget::item { padding: 1 0 1 0; margin: 0; background-color: transparent } "
               << "QListWidget::item:selected { border-radius: 3px; color: "
               << hudSelectionFgString
               << "; background-color: "
               << hudSelectionBgString
               << " }"
            ;
    }

    stream << "QScrollBar::horizontal { border: 0; background: transparent; height: 10px; margin: 0 } "
           << "QScrollBar::handle:horizontal { border: 0; background: "
           << hudFgString
           << "; min-width: 20px; border-radius: 5px; } "
           << "QScrollBar::vertical { border: 0; background: transparent; width: 10px; margin: 0 } "
           << "QScrollBar::handle:vertical { border: 0; background: "
           << hudFgString
           << "; min-height: 20px; border-radius: 5px;} "
           << "QScrollBar::add-line { background: transparent; border: 0 } "
           << "QScrollBar::sub-line { background: transparent; border: 0 } "
           << "QAbstractScrollArea::corner { background: transparent } "
        ;

    outlineWidget->setStyleSheet(styleSheet);
    cheatSheetWidget->setStyleSheet(styleSheet);
    editor->setupPaperMargins(this->width());
}

// Lifted from FocusWriter's theme.cpp file
void MainWindow::predrawBackgroundImage()
{
    adjustedBackgroundImage =
        QImage
        (
            this->size(),
            QImage::Format_ARGB32_Premultiplied
        );
    adjustedBackgroundImage.fill(theme.getBackgroundColor().rgb());

    QPainter painter(&adjustedBackgroundImage);
    painter.setPen(Qt::NoPen);

    if (PictureAspectTile == theme.getBackgroundImageAspect())
    {
        painter.fillRect
        (
            adjustedBackgroundImage.rect(),
            originalBackgroundImage
        );
    }
    else
    {
        Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio;
        bool scaleImage = true;

        switch (theme.getBackgroundImageAspect())
        {
            case PictureAspectZoom:
                aspectRatioMode = Qt::KeepAspectRatioByExpanding;
                break;
            case PictureAspectScale:
                aspectRatioMode = Qt::KeepAspectRatio;
                break;
            case PictureAspectStretch:
                aspectRatioMode = Qt::IgnoreAspectRatio;
                break;
            default:
                // Centered
                scaleImage = false;
                break;
        }

        QImage image(originalBackgroundImage);

        if (scaleImage)
        {
            image = image.scaled
                (
                    this->size(),
                    aspectRatioMode,
                    Qt::SmoothTransformation
                );
        }

        painter.drawImage
        (
            (this->width() - image.width()) / 2,
            (this->height() - image.height()) / 2,
            image
        );

        painter.end();
    }
}
