/*
 * SPDX-FileCopyrightText: 2014-2024 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-FileCopyrightText: 2009-2014 Graeme Gott <graeme@gottcode.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontDialog>
#include <QGridLayout>
#include <QIODevice>
#include <QIcon>
#include <QImageReader>
#include <QLabel>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPushButton>
#include <QScrollBar>
#include <QSettings>
#include <QSizePolicy>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTextDocumentFragment>
#include <QToolButton>
#include <QWhatsThis>

#include <KAboutData>
#include <KActionCollection>
#include <KHelpMenu>
#include <KStandardAction>

#include "export/exporter.h"
#include "export/exporterfactory.h"
#include "settings/localedialog.h"
#include "settings/preferencesdialog.h"
#include "settings/previewoptionsdialog.h"
#include "settings/simplefontdialog.h"
#include "theme/stylesheetbuilder.h"
#include "theme/themeselectiondialog.h"
#include "spelling/spellcheckdecorator.h"
#include "spelling/spellcheckdialog.h"

#include "findreplace.h"
#include "library.h"
#include "mainwindow.h"
#include "messageboxhelper.h"

#define GW_MAIN_WINDOW_GEOMETRY_KEY "Window/mainWindowGeometry"
#define GW_MAIN_WINDOW_STATE_KEY "Window/mainWindowState"
#define GW_SPLITTER_GEOMETRY_KEY "Window/splitterGeometry"

#define MAX_RECENT_FILES (AppActions::OpenLeastRecent - AppActions::OpenMostRecent + 1)

namespace ghostwriter
{
using namespace std::placeholders;

enum SidebarTabIndex {
    FirstSidebarTab,
    OutlineSidebarTab = FirstSidebarTab,
    SessionStatsSidebarTab,
    DocumentStatsSidebarTab,
    CheatSheetSidebarTab,
    LastSidebarTab = CheatSheetSidebarTab
};

MainWindow::MainWindow(const QString &filePath, QWidget *parent)
    : QMainWindow(parent)
{
    Bookmark fileToOpen(filePath);

    focusModeEnabled = false;
    appSettings = AppSettings::instance();

    loadTheme();
    m_actionCollection = new KActionCollection(this);
    m_actions = new AppActions(actionCollection(), primaryIconTheme, this);

    setupGui();
    setupActions();

    setWindowTitle(documentManager->document()->displayName() + "[*] - " + qAppName());

    // If the file specified as a command line argument does not exist, then
    // create it.
    if (!fileToOpen.isValid() && !fileToOpen.isNull()) {
        QFile file(fileToOpen.filePath());
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    if (appSettings->restoreSessionEnabled()) {
        if (!fileToOpen.isValid()) {
            Bookmark lastOpened = Library().lastOpened();

            if (lastOpened.isValid()) {
                fileToOpen = lastOpened;
            }
        }
    }

    connect(appSettings, &AppSettings::autoSaveChanged, documentManager, &DocumentManager::setAutoSaveEnabled);
    connect(appSettings, &AppSettings::backupFileChanged, documentManager, &DocumentManager::setFileBackupEnabled);
    connect(appSettings, &AppSettings::backupLocationChanged, documentManager, &DocumentManager::setBackupLocation);
    connect(appSettings, &AppSettings::focusModeChanged, this, &MainWindow::changeFocusMode);
    connect(appSettings, &AppSettings::hideMenuBarInFullScreenChanged, this, &MainWindow::toggleHideMenuBarInFullScreen);
    connect(appSettings, &AppSettings::fileHistoryChanged, this, &MainWindow::toggleFileHistoryEnabled);
    connect(appSettings, &AppSettings::displayTimeInFullScreenChanged, this, &MainWindow::toggleDisplayTimeInFullScreen);
    connect(appSettings, &AppSettings::editorWidthChanged, this, &MainWindow::changeEditorWidth);
    connect(appSettings, &AppSettings::interfaceStyleChanged, this, &MainWindow::changeInterfaceStyle);
    connect(appSettings, &AppSettings::previewTextFontChanged, this, &MainWindow::applyTheme);
    connect(appSettings, &AppSettings::previewCodeFontChanged, this, &MainWindow::applyTheme);

    connect(documentManager, &DocumentManager::documentLoaded, documentManager, [this]() {
        sessionStats->startNewSession(documentStats->wordCount());
        refreshRecentFiles();
    });

    connect(documentManager, &DocumentManager::documentClosed, documentManager, [this]() {
        sessionStats->startNewSession(0);
    });

    qApp->installEventFilter(this);

    toggleHideMenuBarInFullScreen(appSettings->hideMenuBarInFullScreenEnabled());
    menuBarMenuActivated = false;

    // Need this call for GTK / Gnome 42 segmentation fault workaround.
    qApp->processEvents();

    show();

    // Apply the theme only after show() is called on all the widgets,
    // since the Outline scrollbars can end up transparent in Windows if
    // the theme is applied before show().  We cannot call show() and
    // then apply the theme in the constructor due to a bug with
    // Wayland + GTK that causes a segmentation fault.
    //
    applyTheme();
    adjustEditor();

    // Show the theme right away before loading any files.
    qApp->processEvents();

    // Load file from command line or last session if valid, otherwise create
    // an untitled document.
    if (fileToOpen.isValid()) {
        documentManager->openFileAt(fileToOpen);
    } else {
        documentManager->createUntitled();
    }
}

MainWindow::~MainWindow()
{
    if (primaryIconTheme) {
        delete primaryIconTheme;
        primaryIconTheme = nullptr;
    }

    if (secondaryIconTheme) {
        delete secondaryIconTheme;
        secondaryIconTheme = nullptr;
    }
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 500);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    int width = event->size().width();

    if (width < (0.5 * qApp->primaryScreen()->size().width())) {
        this->sidebar->setVisible(false);
        this->sidebar->setAutoHideEnabled(true);
        this->sidebarHiddenForResize = true;
    }
    else {
        this->sidebarHiddenForResize = false;

        if (!this->focusModeEnabled && this->appSettings->sidebarVisible()) {
            this->sidebar->setAutoHideEnabled(false);
            this->sidebar->setVisible(true);
        }
        else {
            this->sidebar->setAutoHideEnabled(true);
            this->sidebar->setVisible(false);
        }
    }

    adjustEditor();
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();

    switch (key) {
    case Qt::Key_Escape:
    case Qt::Key_F11:
        if (this->isFullScreen()) {
            toggleFullScreen(false);
        }
        break;
    case Qt::Key_Alt:
        if (this->isFullScreen() && appSettings->hideMenuBarInFullScreenEnabled()) {
            if (!this->menuBar()->isVisible()) {
                this->menuBar()->show();
            } else {
                this->menuBar()->hide();
            }
        }
        break;
    case Qt::Key_Tab:
        if (findReplace->isVisible() && findReplace->hasFocus()) {
            findReplace->keyPressEvent(e);
            return;
        }
        else if (!this->editor->hasFocus()) {
            QMainWindow::keyPressEvent(e);
        }
        break;
    default:
        break;
    }

    QMainWindow::keyPressEvent(e);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (this->isFullScreen() && appSettings->hideMenuBarInFullScreenEnabled()) {
        if ((this->menuBar() == obj) 
                && (QEvent::Leave == event->type()) 
                && !menuBarMenuActivated) {
            this->menuBar()->hide();
        } else if (QEvent::MouseMove == event->type()) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

            if ((mouseEvent->globalPosition().y() <= 0) && !this->menuBar()->isVisible()) {
                this->menuBar()->show();
            }
        } else if ((this == obj) 
                && (((QEvent::Leave == event->type()) && !menuBarMenuActivated) 
                    || (QEvent::WindowDeactivate == event->type()))) {
            this->menuBar()->hide();
        }
    }

    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (documentManager->close()) {
        this->quitApplication();
    } else {
        event->ignore();
    }
}

void MainWindow::quitApplication()
{
    if (documentManager->close()) {
        appSettings->store();

        QSettings windowSettings;

        windowSettings.setValue(GW_MAIN_WINDOW_GEOMETRY_KEY, saveGeometry());
        windowSettings.setValue(GW_MAIN_WINDOW_STATE_KEY, saveState());
        windowSettings.setValue(GW_SPLITTER_GEOMETRY_KEY, splitter->saveState());
        windowSettings.sync();

        this->editor->document()->disconnect();
        this->editor->disconnect();
        this->htmlPreview->disconnect();
        StyleSheetBuilder::clearCache();

        qApp->quit();
    }
}

void MainWindow::changeTheme()
{
    ThemeSelectionDialog *themeDialog = new ThemeSelectionDialog(theme.name(), appSettings->darkModeEnabled(), this);
    themeDialog->setAttribute(Qt::WA_DeleteOnClose);

    this->connect(themeDialog, &ThemeSelectionDialog::finished, themeDialog, [this, themeDialog](int result) {
        Q_UNUSED(result)
        this->theme = themeDialog->theme();
        applyTheme();
    });

    themeDialog->open();
}

void MainWindow::openPreferencesDialog()
{
    PreferencesDialog *preferencesDialog = new PreferencesDialog(this);
    preferencesDialog->setAttribute(Qt::WA_DeleteOnClose);
    preferencesDialog->show();
}

void MainWindow::toggleHtmlPreview(bool checked)
{
    htmlPreview->setVisible(checked);
    htmlPreview->updatePreview();
    appSettings->setHtmlPreviewVisible(checked);
    this->update();
    adjustEditor();
}

void MainWindow::toggleHemingwayMode(bool checked)
{
    if (checked) {
        editor->setHemingWayModeEnabled(true);
    } else {
        editor->setHemingWayModeEnabled(false);
    }
}

void MainWindow::toggleFocusMode(bool checked)
{
    this->focusModeEnabled = checked;

    if (checked) {
        editor->setFocusMode(appSettings->focusMode());
        sidebar->setVisible(false);
        sidebar->setAutoHideEnabled(true);
    } else {
        editor->setFocusMode(FocusModeDisabled);

        if (!this->sidebarHiddenForResize && this->appSettings->sidebarVisible()) {
            sidebar->setAutoHideEnabled(false);
            sidebar->setVisible(true);
        }
    }
}

void MainWindow::toggleFullScreen(bool checked)
{
    static bool lastStateWasMaximized = false;

    if (this->isFullScreen() || !checked) {
        if (appSettings->displayTimeInFullScreenEnabled()) {
            timeIndicator->hide();
        }

        // If the window had been maximized prior to entering
        // full screen mode, then put the window back to
        // to maximized.  Don't call showNormal(), as that
        // doesn't restore the window to maximized.
        //
        if (lastStateWasMaximized) {
            showMaximized();
        }
        // Put the window back to normal (not maximized).
        else {
            showNormal();
        }

        if (appSettings->hideMenuBarInFullScreenEnabled()) {
            this->menuBar()->show();
        }
    } else {
        if (appSettings->displayTimeInFullScreenEnabled()) {
            timeIndicator->show();
        }

        if (this->isMaximized()) {
            lastStateWasMaximized = true;
        } else {
            lastStateWasMaximized = false;
        }

        showFullScreen();

        if (appSettings->hideMenuBarInFullScreenEnabled()) {
            this->menuBar()->hide();
        }
    }
}

void MainWindow::toggleHideMenuBarInFullScreen(bool checked)
{
    if (this->isFullScreen()) {
        if (checked) {
            this->menuBar()->hide();
        } else {
            this->menuBar()->show();
        }
    }
}

void MainWindow::toggleFileHistoryEnabled(bool checked)
{
    if (!checked) {
        this->clearRecentFileHistory();
    }

    documentManager->setFileHistoryEnabled(checked);
}

void MainWindow::toggleDisplayTimeInFullScreen(bool checked)
{
    if (this->isFullScreen()) {
        if (checked) {
            this->timeIndicator->show();
        } else {
            this->timeIndicator->hide();
        }
    }
}

void MainWindow::changeEditorWidth(EditorWidth editorWidth)
{
    editor->setEditorWidth(editorWidth);
    adjustEditor();
}

void MainWindow::changeInterfaceStyle(InterfaceStyle style)
{
    Q_UNUSED(style);

    applyTheme();
}

void MainWindow::showQuickReferenceGuide()
{
    QDesktopServices::openUrl(QUrl("https://ghostwriter.kde.org/documentation"));
}

void MainWindow::showWikiPage()
{
    QDesktopServices::openUrl(QUrl("https://github.com/KDE/ghostwriter/wiki"));
}

void MainWindow::changeFocusMode(FocusMode focusMode)
{
    if (FocusModeDisabled != editor->focusMode()) {
        editor->setFocusMode(focusMode);
    }
}

void MainWindow::refreshRecentFiles()
{
    if (appSettings->fileHistoryEnabled()) {
        Library library;
        BookmarkList recentFiles = library.recentFiles(MAX_RECENT_FILES);

        for (int i = 0; i < recentFilesActions.size(); i++) {
            QAction *action = recentFilesActions.at(i);

            if (i < recentFiles.size()) {
                QString path = recentFiles.at(i).filePath();
                action->setText(path);
                action->setData(path);
                action->setVisible(true);
            } else {
                action->setText("");
                action->setData(QVariant());
                action->setVisible(false);
            }
        }

        appAction(AppActions::ReopenLastClosed)->setEnabled(!recentFiles.isEmpty());
    } else {
        appAction(AppActions::ReopenLastClosed)->setEnabled(false);
    }
}

void MainWindow::clearRecentFileHistory()
{
    Library library;
    library.clearHistory();

    for (auto action : recentFilesActions) {
        action->setText("");
        action->setData(QVariant());
        action->setVisible(false);
    }
}

void MainWindow::changeDocumentDisplayName(const QString &displayName)
{
    setWindowTitle(displayName + QString("[*] - ") + qAppName());

    if (documentManager->document()->isModified()) {
        setWindowModified(!appSettings->autoSaveEnabled());
    } else {
        setWindowModified(false);
    }
}

void MainWindow::onOperationStarted(const QString &description)
{
    if (!description.isNull()) {
        statusIndicator->setText(description);
    }

    statisticsIndicator->hide();
    statusIndicator->show();
    this->update();
    qApp->processEvents();
}

void MainWindow::onOperationFinished()
{
    statusIndicator->setText(QString());
    statisticsIndicator->show();
    statusIndicator->hide();
    this->update();
    qApp->processEvents();
}

void MainWindow::changeFont()
{
    bool success;

    QFont font =
        SimpleFontDialog::font(&success, editor->font(), this);

    if (success) {
        editor->setFont(font.family(), font.pointSize());
        appSettings->setEditorFont(font);
    }
}

void MainWindow::onFontSizeChanged(int size)
{
    QFont font = editor->font();
    font.setPointSize(size);
    appSettings->setEditorFont(font);
}

void MainWindow::onSetLocale()
{
    LocaleDialog *dialog = new LocaleDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::copyHtml()
{
    Exporter *htmlExporter = appSettings->currentHtmlExporter();

    if (nullptr != htmlExporter) {
        QTextCursor c = editor->textCursor();
        QString markdownText;
        QString html;

        if (c.hasSelection()) {
            // Get only selected text from the document.
            markdownText = c.selection().toPlainText();
        } else {
            // Get all text from the document.
            markdownText = editor->toPlainText();
        }

        // Convert Markdown to HTML.
        htmlExporter->exportToHtml(markdownText, html);

        // Insert HTML into clipboard.
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(html);
    }
}

void MainWindow::showPreviewOptions()
{
    static PreviewOptionsDialog *dialog = new PreviewOptionsDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setModal(false);
    dialog->show();
}

void MainWindow::onAboutToHideMenuBarMenu()
{
    menuBarMenuActivated = false;

    if (!this->menuBar()->underMouse()
            && this->isFullScreen()
            && appSettings->hideMenuBarInFullScreenEnabled()
            && this->menuBar()->isVisible()) {
        this->menuBar()->hide();
    }
}

void MainWindow::onAboutToShowMenuBarMenu()
{
    menuBarMenuActivated = true;

    if (this->isFullScreen()
            && appSettings->hideMenuBarInFullScreenEnabled()
            && !this->menuBar()->isVisible()) {
        this->menuBar()->show();
    }
}

void MainWindow::onSidebarVisibilityChanged(bool visible)
{
    if (!visible) {
        editor->setFocus();
    }

    this->adjustEditor();
}

void MainWindow::toggleSidebarVisible(bool visible)
{
    this->appSettings->setSidebarVisible(visible);

    if (!this->sidebarHiddenForResize
            && !this->focusModeEnabled
            && this->appSettings->sidebarVisible()) {
        sidebar->setAutoHideEnabled(false);
    }
    else {
        sidebar->setAutoHideEnabled(true);
    }

    this->sidebar->setVisible(visible);
    this->sidebar->setFocus();
    adjustEditor();
}

KActionCollection *MainWindow::actionCollection() const
{
    return m_actionCollection;
}

QMenu *MainWindow::addMenuBarMenu(const QString &name)
{
    QMenu *menu = new QMenu(name, this);
    connect(menu, &QMenu::aboutToShow, this, &MainWindow::onAboutToShowMenuBarMenu);
    connect(menu, &QMenu::aboutToHide, this, &MainWindow::onAboutToHideMenuBarMenu);
    menuBar()->addMenu(menu);

    return menu;
}

QAction *MainWindow::appAction(AppActions::ActionType actionType) const
{
    auto action = m_actions->get(actionType);

    if (nullptr) {
        qCritical() << "Unknown action type:" << actionType;
    }

    return action;
}

void MainWindow::loadTheme()
{
    QString err;
    QString themeName = appSettings->themeName();
    ThemeRepository themeRepo(appSettings->themeDirectoryPath());

    theme = themeRepo.loadTheme(themeName, err);

    if (!theme.name().isEmpty()) {
        appSettings->setThemeName(theme.name());
    }

    ColorScheme colorScheme;

    if (appSettings->darkModeEnabled()) {
        colorScheme = theme.darkColorScheme();
    } else {
        colorScheme = theme.lightColorScheme();
    }

    ChromeColors chromeColors(colorScheme);

    primaryIconTheme = new SvgIconTheme(":/icons");
    primaryIconTheme->setColor(QIcon::Normal, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::NormalState));
    primaryIconTheme->setColor(QIcon::Active, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::ActiveState));
    primaryIconTheme->setColor(QIcon::Selected, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::PressedState));
    primaryIconTheme->setColor(QIcon::Disabled, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::DisabledState));

    secondaryIconTheme = new SvgIconTheme(":/icons");
    secondaryIconTheme->setColor(QIcon::Normal, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::NormalState));
    secondaryIconTheme->setColor(QIcon::Active, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::NormalState));
    secondaryIconTheme->setColor(QIcon::Selected, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::PressedState));
    secondaryIconTheme->setColor(QIcon::Disabled, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::DisabledState));
}

void MainWindow::setupActions()
{
    // File Menu Actions

    m_actions->connect(AppActions::New, documentManager, &DocumentManager::createUntitled);
    m_actions->connect(AppActions::Open, documentManager, &DocumentManager::open);

    auto reopenLastAction = appAction(AppActions::ReopenLastClosed);

    // Get open recent files actions.
    for (int i = AppActions::OpenMostRecent; i <= AppActions::OpenLeastRecent; i++) {
        int index = i - AppActions::OpenMostRecent;
        bool enableReopenLast = false;
        auto action = appAction((AppActions::ActionType)i);

        Library library;
        BookmarkList recentFiles = library.recentFiles();

        if (recentFiles.length() > index) {
            auto filePath = recentFiles.at(index).filePath();
            action->setText(filePath);
            action->setData(filePath);
            action->setVisible(true);
            enableReopenLast = true;
        } else {
            action->setVisible(false);
        }

        reopenLastAction->setEnabled(enableReopenLast);
        recentFilesActions.append(action);

        m_actions->connect((AppActions::ActionType)i, this, [this, action](bool checked) {
            Q_UNUSED(checked)

            if (action->data().isValid()) {
                // Use the action's data for access to the actual file
                // path, since KDE Plasma will add a keyboard
                // accelerator to the action's text by inserting an
                // ampersand (&) into it.
                //
                Library library;
                Bookmark location = library.lookup(action->data().toString());

                if (location.isNull()) {
                    location = Bookmark(action->data().toString());
                }

                documentManager->openFileAt(location);
                refreshRecentFiles();
            }
        });
    }

    m_actions->connect(AppActions::ClearRecentFilesList, this, &MainWindow::clearRecentFileHistory);
    m_actions->connect(AppActions::Save, documentManager, &DocumentManager::saveFile);
    m_actions->connect(AppActions::SaveAs, documentManager, &DocumentManager::saveAs);
    m_actions->connect(AppActions::RenameFile, documentManager, &DocumentManager::rename);
    m_actions->connect(AppActions::Reload, documentManager, &DocumentManager::reload);
    m_actions->connect(AppActions::Export, documentManager, &DocumentManager::exportFile);
    m_actions->connect(AppActions::Quit, this, &MainWindow::quitApplication);

    // Edit Menu Actions

    m_actions->connect(AppActions::Undo, editor, &MarkdownEditor::undo);
    m_actions->connect(AppActions::Redo, editor, &MarkdownEditor::redo);
    m_actions->connect(AppActions::Cut, editor, &MarkdownEditor::cut);
    m_actions->connect(AppActions::Copy, editor, &MarkdownEditor::copy);
    m_actions->connect(AppActions::Paste, editor, &MarkdownEditor::paste);
    m_actions->connect(AppActions::CopyHTML, this, &MainWindow::copyHtml);
    m_actions->connect(AppActions::SelectAll, editor, &MarkdownEditor::selectAll);
    m_actions->connect(AppActions::Deselect, editor, &MarkdownEditor::deselectText);
    m_actions->connect(AppActions::InsertImage, editor, &MarkdownEditor::insertImage);
    // TODO: add Deselect method to editor.
    m_actions->connect(AppActions::Find, findReplace, &FindReplace::showFindView);
    m_actions->connect(AppActions::Replace, findReplace, &FindReplace::showReplaceView);
    m_actions->connect(AppActions::FindNext, findReplace, &FindReplace::findNext);
    m_actions->connect(AppActions::FindPrev, findReplace, &FindReplace::findPrevious);
    m_actions->connect(AppActions::Spelling, this, &MainWindow::runSpellCheck);

    // Format Menu Actions

    m_actions->connect(AppActions::Strong, editor, &MarkdownEditor::bold);
    m_actions->connect(AppActions::Emphasis, editor, &MarkdownEditor::italic);
    m_actions->connect(AppActions::Strikethrough, editor, &MarkdownEditor::strikethrough);
    m_actions->connect(AppActions::InsertHTMLComment, editor, &MarkdownEditor::insertComment);
    m_actions->connect(AppActions::IndentText, editor, &MarkdownEditor::indentText);
    m_actions->connect(AppActions::UnindentText, editor, &MarkdownEditor::unindentText);
    m_actions->connect(AppActions::CodeFences, editor, &MarkdownEditor::insertCodeFences);
    m_actions->connect(AppActions::BlockQuote, editor, &MarkdownEditor::createBlockquote);
    m_actions->connect(AppActions::StripBlockQuote, editor, &MarkdownEditor::removeBlockquote);
    m_actions->connect(AppActions::BulletListAsterisk, editor, &MarkdownEditor::createBulletListWithAsteriskMarker);
    m_actions->connect(AppActions::BulletListMinus, editor, &MarkdownEditor::createBulletListWithMinusMarker);
    m_actions->connect(AppActions::BulletListPlus, editor, &MarkdownEditor::createBulletListWithPlusMarker);
    m_actions->connect(AppActions::NumberedListPeriod, editor, &MarkdownEditor::createNumberedListWithPeriodMarker);
    m_actions->connect(AppActions::NumberedListParenthesis, editor, &MarkdownEditor::createNumberedListWithParenthesisMarker);
    m_actions->connect(AppActions::TaskList, editor, &MarkdownEditor::createTaskList);
    m_actions->connect(AppActions::TaskComplete, editor, &MarkdownEditor::toggleTaskComplete);

    // View Menu Actions

    appAction(AppActions::FullScreen)->setChecked(isFullScreen());
    m_actions->connect(AppActions::FullScreen, this, &MainWindow::toggleFullScreen);
    appAction(AppActions::DistractionFreeMode)->setChecked(false);
    m_actions->connect(AppActions::DistractionFreeMode, this, &MainWindow::toggleFocusMode);
    appAction(AppActions::Preview)->setChecked(appSettings->htmlPreviewVisible());
    m_actions->connect(AppActions::Preview, this, &MainWindow::toggleHtmlPreview);
    m_actions->connect(AppActions::HemingwayMode, this, &MainWindow::toggleHemingwayMode);
    appAction(AppActions::DarkMode)->setChecked(appSettings->darkModeEnabled());
    m_actions->connect(AppActions::DarkMode, this, [this](bool enabled) {
        appSettings->setDarkModeEnabled(enabled);
        applyTheme();
    });
    appAction(AppActions::ShowSidebar)->setChecked(appSettings->sidebarVisible());
    m_actions->connect(AppActions::ShowSidebar, this, &MainWindow::toggleSidebarVisible);
    m_actions->connect(AppActions::ShowOutline, this, [this]() {
        sidebar->setVisible(true);
        sidebar->setCurrentTabIndex(OutlineSidebarTab);
    });
    m_actions->connect(AppActions::ShowSessionStatistics, this, [this]() {
        sidebar->setVisible(true);
        sidebar->setCurrentTabIndex(SessionStatsSidebarTab);
    });
    m_actions->connect(AppActions::ShowDocumentStatistics, this, [this]() {
        sidebar->setVisible(true);
        sidebar->setCurrentTabIndex(DocumentStatsSidebarTab);
    });
    m_actions->connect(AppActions::ShowCheatSheet, this, [this]() {
        sidebar->setVisible(true);
        sidebar->setCurrentTabIndex(CheatSheetSidebarTab);
    });
    m_actions->connect(AppActions::ZoomIn, editor, &MarkdownEditor::increaseFontSize);
    m_actions->connect(AppActions::ZoomOut, editor, &MarkdownEditor::decreaseFontSize);

    // Settings Menu Actions

    m_actions->connect(AppActions::ChangeTheme, this, &MainWindow::changeTheme);
    m_actions->connect(AppActions::ChangeFont, this, &MainWindow::changeFont);
    m_actions->connect(AppActions::SwitchApplicationLanguage, this, &MainWindow::onSetLocale);
    m_actions->connect(AppActions::PreviewOptions, this, &MainWindow::showPreviewOptions);
    m_actions->connect(AppActions::Preferences, this, &MainWindow::openPreferencesDialog);

    // Help Menu Actions

    m_helpMenu = new KHelpMenu(this);
    m_actions->connect(AppActions::AboutApp, m_helpMenu, &KHelpMenu::aboutApplication);
    m_actions->connect(AppActions::AboutKDE, m_helpMenu, &KHelpMenu::aboutKDE);
    m_actions->connect(AppActions::HelpContents, this, &MainWindow::showQuickReferenceGuide);
    m_actions->connect(AppActions::ReportBug, m_helpMenu, &KHelpMenu::reportBug);
    m_actions->connect(AppActions::Donate, m_helpMenu, &KHelpMenu::donate);
    m_actions->connect(AppActions::WhatsThis, this, &QWhatsThis::enterWhatsThisMode);
}

void MainWindow::setupGui()
{
    setObjectName("mainWindow");
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    MarkdownDocument *document = new MarkdownDocument();

    editor = new MarkdownEditor(document, theme.lightColorScheme(), this);
    editor->setMinimumWidth(0.1 * qApp->primaryScreen()->size().width());
    editor->setFont(appSettings->editorFont().family(), appSettings->editorFont().pointSize());
    editor->setUseUnderlineForEmphasis(appSettings->useUnderlineForEmphasis());
    editor->setEnableLargeHeadingSizes(appSettings->largeHeadingSizesEnabled());
    editor->setAutoMatchEnabled(appSettings->autoMatchEnabled());
    editor->setBulletPointCyclingEnabled(appSettings->bulletPointCyclingEnabled());
    editor->setPlainText("");
    editor->setEditorWidth((EditorWidth)appSettings->editorWidth());
    editor->setEditorCorners((InterfaceStyle)appSettings->interfaceStyle());
    editor->setItalicizeBlockquotes(appSettings->italicizeBlockquotes());
    editor->setTabulationWidth(appSettings->tabWidth());
    editor->setInsertSpacesForTabs(appSettings->insertSpacesForTabsEnabled());
    editor->setShowUnbreakableSpaces(appSettings->showUnbreakableSpaceEnabled());

    editor->setAutoMatchEnabled('\"', appSettings->autoMatchCharEnabled('\"'));
    editor->setAutoMatchEnabled('\'', appSettings->autoMatchCharEnabled('\''));
    editor->setAutoMatchEnabled('(', appSettings->autoMatchCharEnabled('('));
    editor->setAutoMatchEnabled('[', appSettings->autoMatchCharEnabled('['));
    editor->setAutoMatchEnabled('{', appSettings->autoMatchCharEnabled('{'));
    editor->setAutoMatchEnabled('*', appSettings->autoMatchCharEnabled('*'));
    editor->setAutoMatchEnabled('_', appSettings->autoMatchCharEnabled('_'));
    editor->setAutoMatchEnabled('`', appSettings->autoMatchCharEnabled('`'));
    editor->setAutoMatchEnabled('<', appSettings->autoMatchCharEnabled('<'));
    connect(appSettings, &AppSettings::tabWidthChanged, editor, &MarkdownEditor::setTabulationWidth);
    connect(appSettings, &AppSettings::insertSpacesForTabsChanged, editor, &MarkdownEditor::setInsertSpacesForTabs);
    connect(appSettings, &AppSettings::showUnbreakableSpaceEnabledChanged, editor, &MarkdownEditor::setShowUnbreakableSpaces);
    connect(appSettings, &AppSettings::useUnderlineForEmphasisChanged, editor, &MarkdownEditor::setUseUnderlineForEmphasis);
    connect(appSettings, &AppSettings::italicizeBlockquotesChanged, editor, &MarkdownEditor::setItalicizeBlockquotes);
    connect(appSettings, &AppSettings::largeHeadingSizesChanged, editor, &MarkdownEditor::setEnableLargeHeadingSizes);
    connect(appSettings, &AppSettings::autoMatchChanged, editor, QOverload<bool>::of(&MarkdownEditor::setAutoMatchEnabled));
    connect(appSettings, &AppSettings::autoMatchCharChanged, editor, QOverload<QChar, bool>::of(&MarkdownEditor::setAutoMatchEnabled));
    connect(appSettings, &AppSettings::bulletPointCyclingChanged, editor, &MarkdownEditor::setBulletPointCyclingEnabled);

    connect(editor, &MarkdownEditor::fontSizeChanged, this, &MainWindow::onFontSizeChanged);
    setFocusProxy(editor);

    documentManager = new DocumentManager(editor, this);
    documentManager->setAutoSaveEnabled(appSettings->autoSaveEnabled());
    documentManager->setFileBackupEnabled(appSettings->backupFileEnabled());
    documentManager->setDraftLocation(appSettings->draftLocation());
    documentManager->setBackupLocation(appSettings->backupLocation());
    documentManager->setFileHistoryEnabled(appSettings->fileHistoryEnabled());
    documentManager->setRestoreSessionEnabled(appSettings->restoreSessionEnabled());
    connect(documentManager, &DocumentManager::documentDisplayNameChanged, this, &MainWindow::changeDocumentDisplayName);
    connect(documentManager, &DocumentManager::documentModifiedChanged, this, &MainWindow::setWindowModified);
    connect(documentManager, &DocumentManager::operationStarted, this, &MainWindow::onOperationStarted);
    connect(documentManager, &DocumentManager::operationUpdate, this, &MainWindow::onOperationStarted);
    connect(documentManager, &DocumentManager::operationFinished, this, &MainWindow::onOperationFinished);
    connect(documentManager, &DocumentManager::sessionHistoryChanged, this, &MainWindow::refreshRecentFiles);

    spelling = new SpellCheckDecorator(editor);
    connect(appSettings, &AppSettings::spellCheckSettingsChanged, spelling, &SpellCheckDecorator::settingsChanged);

    findReplace = new FindReplace(editor, this);
    statusBarWidgets.append(findReplace);
    findReplace->setVisible(false);
    findReplace->setMatchCaseIcon(primaryIconTheme->icon("match-case"));
    findReplace->setWholeWordIcon(primaryIconTheme->icon("whole-word"));
    findReplace->setRegexSearchIcon(primaryIconTheme->icon("regex-search"));
    findReplace->setHighlightMatchesIcon(primaryIconTheme->icon("highlight-matches"));
    findReplace->setFindNextIcon(primaryIconTheme->icon("find-next"));
    findReplace->setFindPreviousIcon(primaryIconTheme->icon("find-previous"));
    findReplace->setCloseIcon(primaryIconTheme->icon("close"));

    setupSidebar();
    setupMenuBar();
    setupStatusBar();

    // Note that the parent widget for this new window must be NULL, so that
    // it will hide beneath other windows when it is deactivated.
    //
    htmlPreview = new HtmlPreview(documentManager->document(), appSettings->currentHtmlExporter(), this);

    connect(editor, &MarkdownEditor::typingPausedScaled, htmlPreview, &HtmlPreview::updatePreview);

    connect(documentManager, &DocumentManager::documentLoaded, htmlPreview, &HtmlPreview::updatePreview);

    connect(documentManager, &DocumentManager::documentClosed, htmlPreview, &HtmlPreview::updatePreview);

    connect(outlineWidget, &OutlineWidget::headingNumberNavigated, htmlPreview, &HtmlPreview::navigateToHeading);
    connect(appSettings, &AppSettings::currentHtmlExporterChanged, htmlPreview, &HtmlPreview::setHtmlExporter);

    htmlPreview->setMinimumWidth(0.1 * qApp->primaryScreen()->size().width());
    htmlPreview->setObjectName("htmlpreview");
    htmlPreview->setVisible(appSettings->htmlPreviewVisible());

    // Set dimensions for the main window.  This is best done before
    // building the status bar, so that we can determine whether the full
    // screen button should be checked.
    //
    QSettings windowSettings;

    if (windowSettings.contains(GW_MAIN_WINDOW_GEOMETRY_KEY)) {
        restoreGeometry(windowSettings.value(GW_MAIN_WINDOW_GEOMETRY_KEY).toByteArray());
        restoreState(windowSettings.value(GW_MAIN_WINDOW_STATE_KEY).toByteArray());
    } else {
        adjustSize();
    }

    splitter = new QSplitter(this);
    splitter->addWidget(sidebar);
    splitter->addWidget(editor);
    splitter->addWidget(htmlPreview);
    splitter->setChildrenCollapsible(false);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 2);
    splitter->setStretchFactor(2, 1);

    // Set default sizes for splitter.
    QList<int> sizes;
    int sidebarWidth = width() * 0.2;
    int otherWidth = (width() - sidebarWidth) / 2;
    sizes.append(sidebarWidth);
    sizes.append(otherWidth);
    sizes.append(otherWidth);

    splitter->setSizes(sizes);

    // If previous splitter geometry was stored, load it.
    if (windowSettings.contains(GW_SPLITTER_GEOMETRY_KEY)) {
        splitter->restoreState(windowSettings.value(GW_SPLITTER_GEOMETRY_KEY).toByteArray());
    }

    connect(splitter, &QSplitter::splitterMoved, splitter, [this](int pos, int index) {
        Q_UNUSED(pos)
        Q_UNUSED(index)
        adjustEditor();
    });

    setCentralWidget(splitter);
}

void MainWindow::setupMenuBar()
{
    QMenu *menu;

    // File Menu

    menu = addMenuBarMenu(tr("&File"));
    menu->addAction(appAction(AppActions::New));
    menu->addAction(appAction(AppActions::Open));

    QAction *openRecentAction = appAction(AppActions::OpenRecent);
    QMenu *submenu = menu->addMenu(openRecentAction->icon(), openRecentAction->text());
    connect(submenu, &QMenu::aboutToShow, this, &MainWindow::onAboutToShowMenuBarMenu);
    connect(submenu, &QMenu::aboutToHide, this, &MainWindow::onAboutToHideMenuBarMenu);

    submenu->addAction(appAction(AppActions::ReopenLastClosed));
    submenu->addSeparator();

    for (int i = AppActions::OpenMostRecent; i <= AppActions::OpenLeastRecent; i++) {
        submenu->addAction(appAction((AppActions::ActionType)i));
    }

    submenu->addSeparator();
    submenu->addAction(appAction(AppActions::ClearRecentFilesList));

    menu->addAction(appAction(AppActions::Save));
    menu->addAction(appAction(AppActions::SaveAs));
    menu->addAction(appAction(AppActions::RenameFile));
    menu->addAction(appAction(AppActions::Reload));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::Export));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::Quit));

    // Edit Menu

    menu = addMenuBarMenu(tr("&Edit"));
    menu->addAction(appAction(AppActions::Undo));
    menu->addAction(appAction(AppActions::Redo));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::Cut));
    menu->addAction(appAction(AppActions::Copy));
    menu->addAction(appAction(AppActions::Paste));
    menu->addAction(appAction(AppActions::CopyHTML));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::SelectAll));
    menu->addAction(appAction(AppActions::Deselect));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::InsertImage));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::Find));
    menu->addAction(appAction(AppActions::Replace));
    menu->addAction(appAction(AppActions::FindNext));
    menu->addAction(appAction(AppActions::FindPrev));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::Spelling));

    // Format Menu

    menu = addMenuBarMenu("&Format");
    menu->addAction(appAction(AppActions::Strong));
    menu->addAction(appAction(AppActions::Emphasis));
    menu->addAction(appAction(AppActions::Strikethrough));
    menu->addAction(appAction(AppActions::InsertHTMLComment));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::IndentText));
    menu->addAction(appAction(AppActions::UnindentText));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::CodeFences));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::BlockQuote));
    menu->addAction(appAction(AppActions::StripBlockQuote));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::BulletListAsterisk));
    menu->addAction(appAction(AppActions::BulletListMinus));
    menu->addAction(appAction(AppActions::BulletListPlus));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::NumberedListPeriod));
    menu->addAction(appAction(AppActions::NumberedListParenthesis));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::TaskList));
    menu->addAction(appAction(AppActions::TaskComplete));

    // View Menu

    menu = addMenuBarMenu(tr("&View"));
    menu->addAction(appAction(AppActions::FullScreen));
    menu->addAction(appAction(AppActions::DistractionFreeMode));
    menu->addAction(appAction(AppActions::Preview));
    menu->addAction(appAction(AppActions::HemingwayMode));
    menu->addAction(appAction(AppActions::DarkMode));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::ShowSidebar));
    menu->addAction(appAction(AppActions::ShowOutline));
    menu->addAction(appAction(AppActions::ShowSessionStatistics));
    menu->addAction(appAction(AppActions::ShowDocumentStatistics));
    menu->addAction(appAction(AppActions::ShowCheatSheet));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::ZoomIn));
    menu->addAction(appAction(AppActions::ZoomOut));

    // Settings Menu

    menu = addMenuBarMenu(tr("&Settings"));
    menu->addAction(appAction(AppActions::ChangeTheme));
    menu->addAction(appAction(AppActions::ChangeFont));
    menu->addAction(appAction(AppActions::SwitchApplicationLanguage));
    menu->addAction(appAction(AppActions::PreviewOptions));
    menu->addAction(appAction(AppActions::Preferences));

    // Help Menu

    menu = addMenuBarMenu(tr("&Help"));
    menu->addAction(appAction(AppActions::HelpContents));
    menu->addAction(appAction(AppActions::WhatsThis));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::ReportBug));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::Donate));
    menu->addSeparator();
    menu->addAction(appAction(AppActions::AboutApp));
    menu->addAction(appAction(AppActions::AboutKDE));

    // Refresh the recent files list with the latest and greatest.
    if (appSettings->fileHistoryEnabled()) {
        refreshRecentFiles();
    }

    // Hide menu bar in full screen mode if enabled to do so.
    if (isFullScreen() && appSettings->hideMenuBarInFullScreenEnabled()) {
        menuBar()->hide();
    }
}

void MainWindow::setupStatusBar()
{
    QGridLayout *statusBarLayout = new QGridLayout();
    statusBarLayout->setSpacing(0);
    statusBarLayout->setContentsMargins(0, 0, 0, 0);

    statusBarLayout->addWidget(findReplace, 0, 0, 1, 3);

    // Divide the status bar into thirds for placing widgets.
    QFrame *leftWidget = new QFrame(statusBar());
    leftWidget->setObjectName("leftStatusBarWidget");
    QFrame *midWidget = new QFrame(statusBar());
    midWidget->setObjectName("midStatusBarWidget");
    QFrame *rightWidget = new QFrame(statusBar());
    rightWidget->setObjectName("rightStatusBarWidget");

    QHBoxLayout *leftLayout = new QHBoxLayout(leftWidget);
    leftWidget->setLayout(leftLayout);
    leftLayout->setContentsMargins(0,0,0,0);
    QHBoxLayout *midLayout = new QHBoxLayout(midWidget);
    midWidget->setLayout(midLayout);
    midLayout->setContentsMargins(0,0,0,0);
    QHBoxLayout *rightLayout = new QHBoxLayout(rightWidget);
    rightWidget->setLayout(rightLayout);
    rightLayout->setContentsMargins(0,0,0,0);

    // Add left-most widgets to status bar.
    QToolButton *button = new QToolButton();
    button->setDefaultAction(appAction(AppActions::ShowSidebar));
    button->setIcon(primaryIconTheme->icon("show-sidebar"));
    button->setObjectName("showSidebarButton");
    button->setFocusPolicy(Qt::NoFocus);

    leftLayout->addWidget(button, 0, Qt::AlignLeft);
    statusBarWidgets.append(button);

    timeIndicator = new TimeLabel(this);
    leftLayout->addWidget(timeIndicator, 0, Qt::AlignLeft);
    leftWidget->setContentsMargins(0, 0, 0, 0);
    statusBarWidgets.append(timeIndicator);

    if (!isFullScreen() || appSettings->displayTimeInFullScreenEnabled()) {
        timeIndicator->hide();
    }

    statusBarLayout->addWidget(leftWidget, 1, 0, 1, 1, Qt::AlignLeft);

    // Add middle widgets to status bar.
    statusIndicator = new QLabel();
    midLayout->addWidget(statusIndicator, 0, Qt::AlignCenter);
    statusIndicator->hide();

    statisticsIndicator = new StatisticsIndicator(this->documentStats, this->sessionStats, this);

    if ((appSettings->favoriteStatistic() >= 0)
            && (appSettings->favoriteStatistic() < statisticsIndicator->count())) {
        statisticsIndicator->setCurrentIndex(appSettings->favoriteStatistic());
    }
    else {
        statisticsIndicator->setCurrentIndex(0);
    }

    connect(statisticsIndicator, QOverload<int>::of(&QComboBox::currentIndexChanged), appSettings, &AppSettings::setFavoriteStatistic);

    midLayout->addWidget(statisticsIndicator, 0, Qt::AlignCenter);
    midWidget->setContentsMargins(0, 0, 0, 0);
    statusBarLayout->addWidget(midWidget, 1, 1, 1, 1, Qt::AlignCenter);
    statusBarWidgets.append(statisticsIndicator);

    // Add right-most widgets to status bar.
    button = new QToolButton();
    button->setDefaultAction(appAction(AppActions::DarkMode));
    button->setIcon(secondaryIconTheme->icon("dark-mode"));
    button->setFocusPolicy(Qt::NoFocus);

    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    button = new QToolButton();
    button->setDefaultAction(appAction(AppActions::Preview));
    button->setIcon(secondaryIconTheme->icon("live-preview"));
    button->setIconSize(QSize(16, 16));
    button->setFocusPolicy(Qt::NoFocus);
    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    button = new QToolButton();
    button->setDefaultAction(appAction(AppActions::HemingwayMode));
    button->setIcon(secondaryIconTheme->icon("hemingway-mode"));
    button->setFocusPolicy(Qt::NoFocus);
    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    button = new QToolButton();
    button->setDefaultAction(appAction(AppActions::DistractionFreeMode));
    button->setIcon(secondaryIconTheme->icon("distraction-free-mode"));
    button->setFocusPolicy(Qt::NoFocus);
    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    button = new QToolButton();
    button->setDefaultAction(appAction(AppActions::FullScreen));
    button->setIcon(secondaryIconTheme->icon("full-screen"));
    button->setFocusPolicy(Qt::NoFocus);
    button->setObjectName("fullscreenButton");
    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    rightWidget->setContentsMargins(0, 0, 0, 0);
    statusBarLayout->addWidget(rightWidget, 1, 2, 1, 1, Qt::AlignRight);
    
    QWidget *container = new QWidget(this);
    container->setObjectName("statusBarWidgetContainer");
    container->setLayout(statusBarLayout);
    container->setContentsMargins(0, 0, 2, 0);
    container->setStyleSheet("#statusBarWidgetContainer { border: 0; margin: 0; padding: 0 }");

    statusBar()->addWidget(container, 1);
    statusBar()->setSizeGripEnabled(false);
}

void MainWindow::setupSidebar()
{
    cheatSheetWidget = new QListWidget(this);

    cheatSheetWidget->setSelectionMode(QAbstractItemView::NoSelection);
    cheatSheetWidget->setAlternatingRowColors(false);

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
    cheatSheetWidget->addItem(tr("[Link](http://url.com \"Title\")"));
    cheatSheetWidget->addItem(tr("[Reference Link][ID]"));
    cheatSheetWidget->addItem(tr("[ID]: http://url.com \"Reference Definition\""));
    cheatSheetWidget->addItem(tr("![Image](./image.jpg \"Title\")"));
    cheatSheetWidget->addItem(tr("--- *** ___ Horizontal Rule"));

    documentStatsWidget = new DocumentStatisticsWidget(this);
    documentStatsWidget->setSelectionMode(QAbstractItemView::NoSelection);
    documentStatsWidget->setAlternatingRowColors(false);

    sessionStatsWidget = new SessionStatisticsWidget(this);
    sessionStatsWidget->setSelectionMode(QAbstractItemView::NoSelection);
    sessionStatsWidget->setAlternatingRowColors(false);

    outlineWidget = new OutlineWidget(editor, this);
    outlineWidget->setAlternatingRowColors(false);

    documentStats = new DocumentStatistics((MarkdownDocument *) editor->document(), this);
    connect(documentStats, &DocumentStatistics::wordCountChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setWordCount);
    connect(documentStats, &DocumentStatistics::characterCountChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setCharacterCount);
    connect(documentStats, &DocumentStatistics::sentenceCountChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setSentenceCount);
    connect(documentStats, &DocumentStatistics::paragraphCountChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setParagraphCount);
    connect(documentStats, &DocumentStatistics::pageCountChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setPageCount);
    connect(documentStats, &DocumentStatistics::complexWordsChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setComplexWords);
    connect(documentStats, &DocumentStatistics::readingTimeChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setReadingTime);
    connect(documentStats, &DocumentStatistics::lixReadingEaseChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setLixReadingEase);
    connect(documentStats, &DocumentStatistics::readabilityIndexChanged,
            documentStatsWidget, &DocumentStatisticsWidget::setReadabilityIndex);
    connect(editor, &MarkdownEditor::textSelected, documentStats, &DocumentStatistics::onTextSelected);
    connect(editor, &MarkdownEditor::textDeselected, documentStats, &DocumentStatistics::onTextDeselected);

    sessionStats = new SessionStatistics(this);
    connect(documentStats, &DocumentStatistics::totalWordCountChanged, sessionStats, &SessionStatistics::onDocumentWordCountChanged);
    connect(sessionStats, &SessionStatistics::wordCountChanged, sessionStatsWidget, &SessionStatisticsWidget::setWordCount);
    connect(sessionStats, &SessionStatistics::pageCountChanged, sessionStatsWidget, &SessionStatisticsWidget::setPageCount);
    connect(sessionStats, &SessionStatistics::wordsPerMinuteChanged, sessionStatsWidget, &SessionStatisticsWidget::setWordsPerMinute);
    connect(sessionStats, &SessionStatistics::writingTimeChanged, sessionStatsWidget, &SessionStatisticsWidget::setWritingTime);
    connect(sessionStats, &SessionStatistics::idleTimePercentageChanged, sessionStatsWidget, &SessionStatisticsWidget::setIdleTime);
    connect(editor, &MarkdownEditor::typingPaused, sessionStats, &SessionStatistics::onTypingPaused);
    connect(editor, &MarkdownEditor::typingResumed, sessionStats, &SessionStatistics::onTypingResumed);

    sidebar = new Sidebar(this);
    sidebar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sidebar->setMinimumWidth(0.1 * QGuiApplication::primaryScreen()->availableSize().width());
    sidebar->setMaximumWidth(0.5 * QGuiApplication::primaryScreen()->availableSize().width());

    sidebar->addTab(primaryIconTheme->icon("outline"), outlineWidget, tr("Outline"));
    sidebar->addTab(primaryIconTheme->icon("session-statistics"), sessionStatsWidget, tr("Session Statistics"));
    sidebar->addTab(primaryIconTheme->icon("document-statistics"), documentStatsWidget, tr("Document Statistics"));
    sidebar->addTab(primaryIconTheme->icon("cheat-sheet"), cheatSheetWidget, tr("Cheat Sheet"), "cheatSheetTab");

    int tabIndex = QSettings().value("sidebarCurrentTab", (int)FirstSidebarTab).toInt();

    if ((tabIndex < 0) || (tabIndex >= sidebar->tabCount())) {
        tabIndex = (int) FirstSidebarTab;
    }

    sidebar->setCurrentTabIndex(tabIndex);

    QPushButton *button = sidebar->addButton(primaryIconTheme->icon("settings"), tr("Settings"));
    connect(button, &QPushButton::clicked, button, [this, button]() {
        static QMenu *popupMenu = nullptr;

        if (nullptr == popupMenu) {
            popupMenu = new QMenu(button);
        }

        popupMenu->addAction(appAction(AppActions::ChangeTheme));
        popupMenu->addAction(appAction(AppActions::ChangeFont));
        popupMenu->addAction(appAction(AppActions::SwitchApplicationLanguage));
        popupMenu->addAction(appAction(AppActions::PreviewOptions));
        popupMenu->addAction(appAction(AppActions::Preferences));
        popupMenu->popup(button->mapToGlobal(QPoint(button->width() / 2, -(button->height() / 2) - 10)));
    });

    if (!sidebarHiddenForResize && !focusModeEnabled && appSettings->sidebarVisible()) {
        sidebar->setAutoHideEnabled(false);
        sidebar->setVisible(true);
    } else {
        sidebar->setAutoHideEnabled(true);
        sidebar->setVisible(false);
    }

    connect(sidebar, &Sidebar::visibilityChanged, this, &MainWindow::onSidebarVisibilityChanged);

    sidebar->setMinimumWidth(0.1 * qApp->primaryScreen()->size().width());
}

void MainWindow::adjustEditor()
{
    // Make sure editor size is updated.
    qApp->processEvents();

    int width = this->width();
    int sidebarWidth = 0;

    // Make sure live preview does not crowd out editor.
    // It should not take up more than 50% of the window space
    // left after the sidebar is accounted for.
    //
    if (sidebar->isVisible()) {
        sidebarWidth = sidebar->width();
    }

    htmlPreview->setMaximumWidth((width - sidebarWidth) / 2);

    // Resize the editor's margins.
    editor->setupPaperMargins();

    // Scroll to cursor position.
    editor->centerCursor();
}

void MainWindow::applyTheme()
{
    if (!theme.name().isNull() && !theme.name().isEmpty()) {
        appSettings->setThemeName(theme.name());
    }

    ColorScheme colorScheme = theme.lightColorScheme();

    if (appSettings->darkModeEnabled()) {
        colorScheme = theme.darkColorScheme();
    }

    ChromeColors chromeColors(colorScheme);

    primaryIconTheme->setColor(QIcon::Normal, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::NormalState));
    primaryIconTheme->setColor(QIcon::Active, chromeColors.color(ChromeColors::Label, ChromeColors::NormalState));
    primaryIconTheme->setColor(QIcon::Selected, chromeColors.color(ChromeColors::Label, ChromeColors::NormalState));
    primaryIconTheme->setColor(QIcon::Disabled, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::DisabledState));

    secondaryIconTheme->setColor(QIcon::Normal, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::NormalState));
    secondaryIconTheme->setColor(QIcon::Active, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::ActiveState));
    secondaryIconTheme->setColor(QIcon::Selected, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::PressedState));
    secondaryIconTheme->setColor(QIcon::Disabled, chromeColors.color(ChromeColors::SecondaryLabel, ChromeColors::DisabledState));

    StyleSheetBuilder styler(chromeColors,
                             secondaryIconTheme,
                             (InterfaceStyleRounded == appSettings->interfaceStyle()),
                             appSettings->editorFont(),
                             appSettings->previewTextFont(),
                             appSettings->previewCodeFont());

    editor->setColorScheme(colorScheme);
    spelling->setErrorColor(colorScheme.error);

    // Do not call MainWindow::setStyleSheet().  Calling it more than once
    // (i.e., when changing a theme) causes a crash in Qt 5.11.  Instead,
    // change the main window's style sheet via qApp.
    //
    QString styleSheet = styler.widgetStyleSheet();

    if (styleSheet.isNull()) {
        qCritical() << "Invalid widget style sheet provided.";
    } else {
        qApp->style()->unpolish(qApp);
        qApp->style()->unpolish(this);
        qApp->setStyleSheet(styleSheet);
        qApp->style()->polish(qApp);
        qApp->style()->polish(this);
    }

    styleSheet = styler.htmlPreviewStyleSheet();

    if (styleSheet.isNull()) {
        qCritical() << "Invalid HTML preview style sheet provided.";
    } else {
        htmlPreview->setStyleSheet(styler.htmlPreviewStyleSheet());
    }

    adjustEditor();
}

void MainWindow::runSpellCheck()
{
    SpellCheckDialog *dialog = new SpellCheckDialog(editor);
    connect(dialog, &SpellCheckDialog::finished, spelling, &SpellCheckDecorator::rehighlight);

    dialog->show();
}

} // namespace ghostwriter
