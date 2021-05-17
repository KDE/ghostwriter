/***********************************************************************
 *
 * Copyright (C) 2014-2021 wereturtle
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

#include <QDebug>

#include <QApplication>
#include <QClipboard>
#include <QCommonStyle>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontDialog>
#include <QGridLayout>
#include <QIcon>
#include <QIODevice>
#include <QLabel>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QScrollBar>
#include <QSettings>
#include <QStatusBar>
#include <QTextStream>

#include "3rdparty/QtAwesome/QtAwesome.h"

#include "documenthistory.h"
#include "exporter.h"
#include "exporterfactory.h"
#include "findreplace.h"
#include "localedialog.h"
#include "mainwindow.h"
#include "messageboxhelper.h"
#include "preferencesdialog.h"
#include "previewoptionsdialog.h"
#include "sandboxedwebpage.h"
#include "simplefontdialog.h"
#include "stylesheetbuilder.h"
#include "themeselectiondialog.h"
#include "spelling/dictionary_manager.h"

namespace ghostwriter
{
enum SidebarTabIndex {
    FirstSidebarTab,
    OutlineSidebarTab = FirstSidebarTab,
    SessionStatsSidebarTab,
    DocumentStatsSidebarTab,
    CheatSheetSidebarTab,
    LastSidebarTab = CheatSheetSidebarTab
};

#define GW_MAIN_WINDOW_GEOMETRY_KEY "Window/mainWindowGeometry"
#define GW_MAIN_WINDOW_STATE_KEY "Window/mainWindowState"

MainWindow::MainWindow(const QString &filePath, QWidget *parent)
    : QMainWindow(parent)
{
    this->awesome = new QtAwesome(qApp);
    this->awesome->initFontAwesome();
    QString fileToOpen;
    setWindowIcon(QIcon(":/resources/images/ghostwriter.svg"));
    this->setObjectName("mainWindow");
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    appSettings = AppSettings::instance();

    QString themeName = appSettings->themeName();

    QString err;
    theme = ThemeRepository::instance()->loadTheme(themeName, err);

    MarkdownDocument *document = new MarkdownDocument();

    editor = new MarkdownEditor(document, theme.lightColorScheme(), this);
    editor->setFont(appSettings->editorFont().family(), appSettings->editorFont().pointSize());
    editor->setUseUnderlineForEmphasis(appSettings->useUnderlineForEmphasis());
    editor->setEnableLargeHeadingSizes(appSettings->largeHeadingSizesEnabled());
    editor->setAutoMatchEnabled(appSettings->autoMatchEnabled());
    editor->setBulletPointCyclingEnabled(appSettings->bulletPointCyclingEnabled());
    editor->setPlainText("");
    editor->setEditorWidth((EditorWidth) appSettings->editorWidth());
    editor->setEditorCorners((InterfaceStyle) appSettings->interfaceStyle());
    editor->setSpellCheckEnabled(appSettings->liveSpellCheckEnabled());
    editor->setItalicizeBlockquotes(appSettings->italicizeBlockquotes());
    editor->setTabulationWidth(appSettings->tabWidth());
    editor->setInsertSpacesForTabs(appSettings->insertSpacesForTabsEnabled());
    connect(editor, SIGNAL(fontSizeChanged(int)), this, SLOT(onFontSizeChanged(int)));
    this->setFocusProxy(editor);


    // We need to set an empty style for the editor's scrollbar in order for the
    // scrollbar CSS stylesheet to take full effect.  Otherwise, the scrollbar's
    // background color will have the Windows 98 checkered look rather than
    // being a solid or transparent color.
    //
    editor->verticalScrollBar()->setStyle(new QCommonStyle());
    editor->horizontalScrollBar()->setStyle(new QCommonStyle());

    buildSidebar();

    documentManager = new DocumentManager(editor, this);
    documentManager->setAutoSaveEnabled(appSettings->autoSaveEnabled());
    documentManager->setFileBackupEnabled(appSettings->backupFileEnabled());
    documentManager->setFileHistoryEnabled(appSettings->fileHistoryEnabled());
    setWindowTitle(documentManager->document()->displayName() + "[*] - " + qAppName());
    connect(documentManager, SIGNAL(documentDisplayNameChanged(QString)), this, SLOT(changeDocumentDisplayName(QString)));
    connect(documentManager, SIGNAL(documentModifiedChanged(bool)), this, SLOT(setWindowModified(bool)));
    connect(documentManager, SIGNAL(operationStarted(QString)), this, SLOT(onOperationStarted(QString)));
    connect(documentManager, SIGNAL(operationUpdate(QString)), this, SLOT(onOperationStarted(QString)));
    connect(documentManager, SIGNAL(operationFinished()), this, SLOT(onOperationFinished()));
    connect(documentManager, SIGNAL(documentClosed()), this, SLOT(refreshRecentFiles()));

    editor->setAutoMatchEnabled('\"', appSettings->autoMatchCharEnabled('\"'));
    editor->setAutoMatchEnabled('\'', appSettings->autoMatchCharEnabled('\''));
    editor->setAutoMatchEnabled('(', appSettings->autoMatchCharEnabled('('));
    editor->setAutoMatchEnabled('[', appSettings->autoMatchCharEnabled('['));
    editor->setAutoMatchEnabled('{', appSettings->autoMatchCharEnabled('{'));
    editor->setAutoMatchEnabled('*', appSettings->autoMatchCharEnabled('*'));
    editor->setAutoMatchEnabled('_', appSettings->autoMatchCharEnabled('_'));
    editor->setAutoMatchEnabled('`', appSettings->autoMatchCharEnabled('`'));
    editor->setAutoMatchEnabled('<', appSettings->autoMatchCharEnabled('<'));

    QWidget *editorPane = new QWidget(this);
    editorPane->setObjectName("editorLayoutArea");
    editorPane->setLayout(editor->preferredLayout());

    QStringList recentFiles;

    if (appSettings->fileHistoryEnabled()) {
        DocumentHistory history;
        recentFiles = history.recentFiles(MAX_RECENT_FILES + 2);
    }

    bool fileLoadError = false;

    if (!filePath.isNull() && !filePath.isEmpty()) {
        QFileInfo cliFileInfo(filePath);

        if (!cliFileInfo.exists()) {
            QFile cliFile(filePath);

            // Try to create a new file if the specified file does not exist.
            cliFile.open(QIODevice::WriteOnly);
            cliFile.close();

            if (!cliFile.exists()) {
                fileLoadError = true;
                qCritical("Could not create new file. Check permissions.");
            }
        }

        if (!fileLoadError) {
            fileToOpen = filePath;
            recentFiles.removeAll(cliFileInfo.absoluteFilePath());
        }
    }

    if (fileToOpen.isNull() && appSettings->fileHistoryEnabled()) {
        QString lastFile;

        if (!recentFiles.isEmpty()) {
            lastFile = recentFiles.first();
        }

        if (QFileInfo(lastFile).exists()) {
            fileToOpen = lastFile;
            recentFiles.removeAll(lastFile);
        }
    }

    for (int i = 0; i < MAX_RECENT_FILES; i++) {
        recentFilesActions[i] = new QAction(this);

        this->connect
        (
            recentFilesActions[i],
            &QAction::triggered,
            [this, i]() {

                if (nullptr != recentFilesActions[i]) {
                    // Use the action's data for access to the actual file path, since
                    // KDE Plasma will add a keyboard accelerator to the action's text
                    // by inserting an ampersand (&) into it.
                    //
                    documentManager->open(recentFilesActions[i]->data().toString());
                    refreshRecentFiles();
                }
            }
        );

        if (i < recentFiles.size()) {
            recentFilesActions[i]->setText(recentFiles.at(i));

            // Use the action's data for access to the actual file path, since
            // KDE Plasma will add a keyboard accelerator to the action's text
            // by inserting an ampersand (&) into it.
            //
            recentFilesActions[i]->setData(recentFiles.at(i));

            recentFilesActions[i]->setVisible(true);
        } else {
            recentFilesActions[i]->setVisible(false);
        }
    }

    // Set dimensions for the main window.  This is best done before
    // building the status bar, so that we can determine whether the full
    // screen button should be checked.
    //
    QSettings windowSettings;

    if (windowSettings.contains(GW_MAIN_WINDOW_GEOMETRY_KEY)) {
        restoreGeometry(windowSettings.value(GW_MAIN_WINDOW_GEOMETRY_KEY).toByteArray());
        restoreState(windowSettings.value(GW_MAIN_WINDOW_STATE_KEY).toByteArray());
    } else {
        this->adjustSize();
    }

    connect(appSettings, SIGNAL(autoSaveChanged(bool)), documentManager, SLOT(setAutoSaveEnabled(bool)));
    connect(appSettings, SIGNAL(backupFileChanged(bool)), documentManager, SLOT(setFileBackupEnabled(bool)));
    connect(appSettings, SIGNAL(tabWidthChanged(int)), editor, SLOT(setTabulationWidth(int)));
    connect(appSettings, SIGNAL(insertSpacesForTabsChanged(bool)), editor, SLOT(setInsertSpacesForTabs(bool)));
    connect(appSettings, SIGNAL(useUnderlineForEmphasisChanged(bool)), editor, SLOT(setUseUnderlineForEmphasis(bool)));
    connect(appSettings, SIGNAL(italicizeBlockquotesChanged(bool)), editor, SLOT(setItalicizeBlockquotes(bool)));
    connect(appSettings, SIGNAL(largeHeadingSizesChanged(bool)), editor, SLOT(setEnableLargeHeadingSizes(bool)));
    connect(appSettings, SIGNAL(autoMatchChanged(bool)), editor, SLOT(setAutoMatchEnabled(bool)));
    connect(appSettings, SIGNAL(autoMatchCharChanged(QChar, bool)), editor, SLOT(setAutoMatchEnabled(QChar, bool)));
    connect(appSettings, SIGNAL(bulletPointCyclingChanged(bool)), editor, SLOT(setBulletPointCyclingEnabled(bool)));
    connect(appSettings, SIGNAL(autoMatchChanged(bool)), editor, SLOT(setAutoMatchEnabled(bool)));
    connect(appSettings, SIGNAL(focusModeChanged(FocusMode)), this, SLOT(changeFocusMode(FocusMode)));
    connect(appSettings, SIGNAL(hideMenuBarInFullScreenChanged(bool)), this, SLOT(toggleHideMenuBarInFullScreen(bool)));
    connect(appSettings, SIGNAL(fileHistoryChanged(bool)), this, SLOT(toggleFileHistoryEnabled(bool)));
    connect(appSettings, SIGNAL(displayTimeInFullScreenChanged(bool)), this, SLOT(toggleDisplayTimeInFullScreen(bool)));
    connect(appSettings, SIGNAL(dictionaryLanguageChanged(QString)), editor, SLOT(setDictionary(QString)));
    connect(appSettings, SIGNAL(liveSpellCheckChanged(bool)), editor, SLOT(setSpellCheckEnabled(bool)));
    connect(appSettings, SIGNAL(editorWidthChanged(EditorWidth)), this, SLOT(changeEditorWidth(EditorWidth)));
    connect(appSettings, SIGNAL(interfaceStyleChanged(InterfaceStyle)), this, SLOT(changeInterfaceStyle(InterfaceStyle)));
    connect(appSettings, SIGNAL(previewTextFontChanged(QFont)), this, SLOT(applyTheme()));
    connect(appSettings, SIGNAL(previewCodeFontChanged(QFont)), this, SLOT(applyTheme()));

    if (this->isFullScreen() && appSettings->hideMenuBarInFullScreenEnabled()) {
        this->menuBar()->hide();
    }

    // Default language for dictionary is set from AppSettings intialization.
    QString language = appSettings->dictionaryLanguage();

    // If we have an available dictionary, then set up spell checking.
    if (!language.isNull() && !language.isEmpty()) {
        editor->setDictionary(language);
        editor->setSpellCheckEnabled(appSettings->liveSpellCheckEnabled());
    } else {
        editor->setSpellCheckEnabled(false);
    }

    connect
    (
        documentStats,
        SIGNAL(wordCountChanged(int)),
        this,
        SLOT(updateWordCount(int))
    );

    this->connect
    (
        documentManager,
        &DocumentManager::documentLoaded,
        [this]() {
            this->sessionStats->startNewSession(this->documentStats->wordCount());
            refreshRecentFiles();
        }
    );

    this->connect
    (
        documentManager,
        &DocumentManager::documentClosed,
        [this]() {
            this->sessionStats->startNewSession(0);
        }
    );

    // Note that the parent widget for this new window must be NULL, so that
    // it will hide beneath other windows when it is deactivated.
    //
    htmlPreview = new HtmlPreview
    (
        documentManager->document(),
        appSettings->currentHtmlExporter(),
        this
    );

    connect(editor, SIGNAL(textChanged()), htmlPreview, SLOT(updatePreview()));
    connect(outlineWidget, SIGNAL(headingNumberNavigated(int)), htmlPreview, SLOT(navigateToHeading(int)));
    connect(appSettings, SIGNAL(currentHtmlExporterChanged(Exporter *)), htmlPreview, SLOT(setHtmlExporter(Exporter *)));

    htmlPreview->setMinimumWidth(0);
    htmlPreview->setObjectName("htmlpreview");
    htmlPreview->setVisible(appSettings->htmlPreviewVisible());

    previewSplitter = new QSplitter(this);
    previewSplitter->addWidget(editorPane);
    previewSplitter->addWidget(htmlPreview);
    previewSplitter->setCollapsible(0, true);
    previewSplitter->setCollapsible(1, true);


    this->findReplace = new FindReplace(this->editor, this);
    statusBarWidgets.append(this->findReplace);
    this->findReplace->setVisible(false);

    buildMenuBar();
    buildStatusBar();
    
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QWidget *mainPane = new QWidget(this);
    mainPane->setLayout(mainLayout);
    mainLayout->addWidget(previewSplitter, 500);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    sidebarSplitter = new QSplitter(this);
    sidebarSplitter->addWidget(sidebar);
    sidebarSplitter->addWidget(mainPane);
    sidebarSplitter->setCollapsible(0, false);
    sidebarSplitter->setCollapsible(1, true);

    this->setCentralWidget(sidebarSplitter);

    // Show the main window.
    show();

    // Apply the theme only after show() is called on all the widgets,
    // since the Outline scrollbars can end up transparent in Windows if
    // the theme is applied before show().
    //
    applyTheme();
    adjustEditorWidth(this->width());

    this->update();
    qApp->processEvents();

    if (!fileToOpen.isNull() && !fileToOpen.isEmpty()) {
        documentManager->open(fileToOpen);
    }

    if (fileLoadError) {
        QMessageBox::critical
        (
            this,
            QApplication::applicationName(),
            tr("Could not create file %1. Check permissions.").arg(filePath)
        );
    }

    toggleHideMenuBarInFullScreen(appSettings->hideMenuBarInFullScreenEnabled());
    menuBarMenuActivated = false;

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    ;
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 500);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    adjustEditorWidth(event->size().width());
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
            if ((mouseEvent->globalY()) <= 0 && !this->menuBar()->isVisible()) {
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
        windowSettings.sync();

        DictionaryManager::instance().addProviders();
        DictionaryManager::instance().setDefaultLanguage(language);

        this->editor->document()->disconnect();
        this->editor->disconnect();
        this->htmlPreview->disconnect();

        qApp->quit();
    }
}

void MainWindow::changeTheme()
{
    ThemeSelectionDialog *themeDialog = new ThemeSelectionDialog(theme.name(), appSettings->darkModeEnabled(), this);
    themeDialog->setAttribute(Qt::WA_DeleteOnClose);

    this->connect
    (
        themeDialog,
        &ThemeSelectionDialog::finished,
        [this, themeDialog](int result) {
            Q_UNUSED(result)
            this->theme = themeDialog->theme();
            applyTheme();
        }
    );

    themeDialog->open();
}

void MainWindow::openPreferencesDialog()
{
    PreferencesDialog *preferencesDialog = new PreferencesDialog(this);
    preferencesDialog->show();
}

void MainWindow::toggleHtmlPreview(bool checked)
{
    QList<int> splitterSizes;

    htmlPreviewMenuAction->blockSignals(true);

    htmlPreviewMenuAction->setChecked(checked);
    htmlPreview->setVisible(checked);
    htmlPreview->updatePreview();
    adjustEditorWidth(this->width());

    htmlPreviewMenuAction->blockSignals(false);
    appSettings->setHtmlPreviewVisible(checked);
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
    if (checked) {
        editor->setFocusMode(appSettings->focusMode());        
    } else {
        editor->setFocusMode(FocusModeDisabled);
    }

    sidebar->setAutoHideEnabled(checked);
}

void MainWindow::toggleFullScreen(bool checked)
{
    static bool lastStateWasMaximized = false;

    this->fullScreenMenuAction->blockSignals(true);
    this->fullScreenMenuAction->setChecked(checked);
    this->fullScreenMenuAction->blockSignals(false);

    this->fullScreenButton->blockSignals(true);
    this->fullScreenButton->setChecked(checked);
    this->fullScreenButton->blockSignals(false);

    if (this->isFullScreen() || !checked) {
        if (appSettings->displayTimeInFullScreenEnabled()) {
            timeLabel->hide();
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
            timeLabel->show();
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
            this->timeLabel->show();
        } else {
            this->timeLabel->hide();
        }
    }
}

void MainWindow::changeEditorWidth(EditorWidth editorWidth)
{
    editor->setEditorWidth(editorWidth);
    adjustEditorWidth(this->width());
}

void MainWindow::changeInterfaceStyle(InterfaceStyle style)
{
    Q_UNUSED(style);

    applyTheme();
}

void MainWindow::insertImage()
{
    QString startingDirectory = QString();
    MarkdownDocument *document = documentManager->document();

    if (!document->isNew()) {
        startingDirectory = QFileInfo(document->filePath()).dir().path();
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

    if (!imagePath.isNull() && !imagePath.isEmpty()) {
        QFileInfo imgInfo(imagePath);
        bool isRelativePath = false;

        if (imgInfo.exists()) {
            if (!document->isNew()) {
                QFileInfo docInfo(document->filePath());

                if (docInfo.exists()) {
                    imagePath = docInfo.dir().relativeFilePath(imagePath);
                    isRelativePath = true;
                }
            }
        }

        if (!isRelativePath) {
            imagePath = QString("file://") + imagePath;
        }

        QTextCursor cursor = editor->textCursor();
        cursor.insertText(QString("![](%1)").arg(imagePath));
    }
}

void MainWindow::showQuickReferenceGuide()
{
    QDesktopServices::openUrl(QUrl("https://wereturtle.github.io/ghostwriter/quickrefguide.html"));
}

void MainWindow::showWikiPage()
{
    QDesktopServices::openUrl(QUrl("https://github.com/wereturtle/ghostwriter/wiki"));
}

void MainWindow::showAbout()
{
    QString aboutText =
        QString("<p><b>") +  qAppName() + QString(" ")
        + qApp->applicationVersion() + QString("</b></p>")
        + tr("<p>Copyright &copy; 2014-2021 wereturtle</b>"
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
    wordCountLabel->setText(tr("%Ln word(s)", "", newWordCount));
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
        DocumentHistory history;
        QStringList recentFiles = history.recentFiles(MAX_RECENT_FILES + 1);
        MarkdownDocument *document = documentManager->document();

        if (!document->isNew()) {
            QString sanitizedPath =
                QFileInfo(document->filePath()).absoluteFilePath();
            recentFiles.removeAll(sanitizedPath);
        }

        for (int i = 0; (i < MAX_RECENT_FILES) && (i < recentFiles.size()); i++) {
            recentFilesActions[i]->setText(recentFiles.at(i));
            recentFilesActions[i]->setData(recentFiles.at(i));
            recentFilesActions[i]->setVisible(true);
        }

        for (int i = recentFiles.size(); i < MAX_RECENT_FILES; i++) {
            recentFilesActions[i]->setVisible(false);
        }
    }
}

void MainWindow::clearRecentFileHistory()
{
    DocumentHistory history;
    history.clear();

    for (int i = 0; i < MAX_RECENT_FILES; i++) {
        recentFilesActions[i]->setVisible(false);
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
        statusLabel->setText(description);
    }

    wordCountLabel->hide();
    statusLabel->show();
    this->update();
    qApp->processEvents();
}

void MainWindow::onOperationFinished()
{
    statusLabel->setText(QString());
    wordCountLabel->show();
    statusLabel->hide();
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
    bool ok;

    QString locale =
        LocaleDialog::locale
        (
            &ok,
            appSettings->locale(),
            appSettings->translationsPath()
        );

    if (ok && (locale != appSettings->locale())) {
        appSettings->setLocale(locale);

        QMessageBox::information
        (
            this,
            QApplication::applicationName(),
            tr("Please restart the application for changes to take effect.")
        );
    }
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
    this->adjustEditorWidth(this->width());

    if (visible) {
        toggleSidebarButton->setText(QChar(fa::chevronleft));
    } else {
        toggleSidebarButton->setText(QChar(fa::chevronright));
    }

    if (!visible) {
        editor->setFocus();
    }
}

void MainWindow::toggleSidebarVisible(bool visible)
{
    this->appSettings->setSidebarVisible(visible);
    this->sidebar->setAutoHideEnabled(!visible);
    this->sidebar->setVisible(visible);

    this->showSidebarAction->blockSignals(true);
    this->showSidebarAction->setChecked(visible);
    this->showSidebarAction->blockSignals(false);
}

QAction* MainWindow::createWindowAction
(
    const QString &text,
    QObject *receiver,
    const char *member,
    const QKeySequence &shortcut
)
{
    QAction* action = new QAction(text, this);
    action->setShortcut(shortcut);
    action->setShortcutContext(Qt::WindowShortcut);
    
    connect(action, SIGNAL(triggered(bool)), receiver, member);
    this->addAction(action);

    return action;
}

QAction* MainWindow::createWidgetAction
(
    const QString &text,
    QWidget *receiver,
    const char *member,
    const QKeySequence &shortcut
)
{
    QAction* action = new QAction(text, receiver);
    action->setShortcut(shortcut);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    
    connect(action, SIGNAL(triggered(bool)), receiver, member);
    receiver->addAction(action);

    return action;
}

void MainWindow::buildMenuBar()
{

    QMenu *fileMenu = this->menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(createWindowAction(tr("&New"), documentManager, SLOT(close()), QKeySequence::New));
    fileMenu->addAction(createWindowAction(tr("&Open"), documentManager, SLOT(open()), QKeySequence::Open));

    QMenu *recentFilesMenu = new QMenu(tr("Open &Recent..."));
    recentFilesMenu->addAction(createWindowAction(tr("Reopen Closed File"), documentManager, SLOT(reopenLastClosedFile()), QKeySequence("SHIFT+CTRL+T")));
    recentFilesMenu->addSeparator();

    for (int i = 0; i < MAX_RECENT_FILES; i++) {
        recentFilesMenu->addAction(recentFilesActions[i]);
    }

    recentFilesMenu->addSeparator();
    recentFilesMenu->addAction(createWindowAction(tr("Clear Menu"), this, SLOT(clearRecentFileHistory())));

    fileMenu->addMenu(recentFilesMenu);

    fileMenu->addSeparator();
    fileMenu->addAction(createWindowAction(tr("&Save"), documentManager, SLOT(save()), QKeySequence::Save));
    fileMenu->addAction(createWindowAction(tr("Save &As..."), documentManager, SLOT(saveAs()), QKeySequence::SaveAs));
    fileMenu->addAction(createWindowAction(tr("R&ename..."), documentManager, SLOT(rename())));
    fileMenu->addAction(createWindowAction(tr("Re&load from Disk..."), documentManager, SLOT(reload())));
    fileMenu->addSeparator();
    fileMenu->addAction(createWindowAction(tr("&Export"), documentManager, SLOT(exportFile()), QKeySequence("CTRL+E")));
    fileMenu->addSeparator();
    QAction *quitAction = createWindowAction(tr("&Quit"), this, SLOT(quitApplication()), QKeySequence::Quit);
    quitAction->setMenuRole(QAction::QuitRole);
    fileMenu->addAction(quitAction);

    QMenu *editMenu = this->menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(createWidgetAction(tr("&Undo"), editor, SLOT(undo()), QKeySequence::Undo));
    editMenu->addAction(createWidgetAction(tr("&Redo"), editor, SLOT(redo()), QKeySequence::Redo));
    editMenu->addSeparator();
    editMenu->addAction(createWidgetAction(tr("Cu&t"), editor, SLOT(cut()), QKeySequence::Cut));
    editMenu->addAction(createWidgetAction(tr("&Copy"), editor, SLOT(copy()), QKeySequence::Copy));
    editMenu->addAction(createWidgetAction(tr("&Paste"), editor, SLOT(paste()), QKeySequence::Paste));
    editMenu->addAction(createWidgetAction(tr("Copy &HTML"), this, SLOT(copyHtml()), QKeySequence("SHIFT+CTRL+C")));
    editMenu->addSeparator();
    editMenu->addAction(createWidgetAction(tr("&Insert Image..."), this, SLOT(insertImage())));
    editMenu->addSeparator();

    editMenu->addAction(createWindowAction(tr("&Find"), findReplace, SLOT(showFindView()), QKeySequence::Find));
    editMenu->addAction(createWindowAction(tr("Rep&lace"), findReplace, SLOT(showReplaceView()), QKeySequence::Replace));
    editMenu->addAction(createWindowAction(tr("Find &Next"), findReplace, SLOT(findNext()), QKeySequence::FindNext));
    editMenu->addAction(createWindowAction(tr("Find &Previous"), findReplace, SLOT(findPrevious()), QKeySequence::FindPrevious));
    editMenu->addSeparator();
    editMenu->addAction(createWindowAction(tr("&Spell check"), editor, SLOT(runSpellChecker())));

    QMenu *formatMenu = this->menuBar()->addMenu(tr("For&mat"));
    formatMenu->addAction(createWidgetAction(tr("&Bold"), editor, SLOT(bold()), QKeySequence::Bold));
    formatMenu->addAction(createWidgetAction(tr("&Italic"), editor, SLOT(italic()), QKeySequence::Italic));
    formatMenu->addAction(createWidgetAction(tr("Stri&kethrough"), editor, SLOT(strikethrough()), QKeySequence("Ctrl+K")));
    formatMenu->addAction(createWidgetAction(tr("&HTML Comment"), editor, SLOT(insertComment()), QKeySequence("Ctrl+/")));
    formatMenu->addSeparator();

    formatMenu->addAction(createWidgetAction(tr("I&ndent"), editor, SLOT(indentText()), QKeySequence("Tab")));
    formatMenu->addAction(createWidgetAction(tr("&Unindent"), editor, SLOT(unindentText()), QKeySequence("Shift+Tab")));
    formatMenu->addSeparator();
    formatMenu->addAction(createWidgetAction(tr("Block &Quote"), editor, SLOT(createBlockquote()), QKeySequence("Ctrl+.")));
    formatMenu->addAction(createWidgetAction(tr("&Strip Block Quote"), editor, SLOT(removeBlockquote()), QKeySequence("Ctrl+,")));
    formatMenu->addSeparator();
    formatMenu->addAction(createWidgetAction(tr("&* Bullet List"), editor, SLOT(createBulletListWithAsteriskMarker()), QKeySequence("Ctrl+8")));
    formatMenu->addAction(createWidgetAction(tr("&- Bullet List"), editor, SLOT(createBulletListWithMinusMarker()), QKeySequence("Ctrl+Shift+-")));
    formatMenu->addAction(createWidgetAction(tr("&+ Bullet List"), editor, SLOT(createBulletListWithPlusMarker()), QKeySequence("Ctrl+Shift+=")));
    formatMenu->addSeparator();
    formatMenu->addAction(createWidgetAction(tr("1&. Numbered List"), editor, SLOT(createNumberedListWithPeriodMarker()), QKeySequence("Ctrl+1")));
    formatMenu->addAction(createWidgetAction(tr("1&) Numbered List"), editor, SLOT(createNumberedListWithParenthesisMarker()), QKeySequence("Ctrl+0")));
    formatMenu->addSeparator();
    formatMenu->addAction(createWidgetAction(tr("&Task List"), editor, SLOT(createTaskList()), QKeySequence("Ctrl+T")));
    formatMenu->addAction(createWidgetAction(tr("Toggle Task(s) &Complete"), editor, SLOT(toggleTaskComplete()), QKeySequence("Ctrl+D")));


    QMenu *viewMenu = this->menuBar()->addMenu(tr("&View"));

    fullScreenMenuAction = new QAction(tr("&Full Screen"), this);
    fullScreenMenuAction->setCheckable(true);
    fullScreenMenuAction->setChecked(this->isFullScreen());
    fullScreenMenuAction->setShortcut(QKeySequence("F11"));
    fullScreenMenuAction->setShortcutContext(Qt::WindowShortcut);
    connect(fullScreenMenuAction, SIGNAL(toggled(bool)), this, SLOT(toggleFullScreen(bool)));
    viewMenu->addAction(fullScreenMenuAction);
    
    htmlPreviewMenuAction = createWindowAction(tr("&Preview in HTML"), this, SLOT(toggleHtmlPreview(bool)), QKeySequence("CTRL+P"));
    htmlPreviewMenuAction->setCheckable(true);
    htmlPreviewMenuAction->setChecked(appSettings->htmlPreviewVisible());
    viewMenu->addAction(htmlPreviewMenuAction);

    this->addAction(showSidebarAction);
    viewMenu->addAction(showSidebarAction);

    QAction *showSidebarTabAction = viewMenu->addAction(tr("&Outline"),
        this,
        [this]() {
            sidebar->setVisible(true);
            sidebar->setCurrentTabIndex(OutlineSidebarTab);
        },
        QKeySequence("CTRL+J"));
    showSidebarTabAction->setShortcutContext(Qt::WindowShortcut);
    this->addAction(showSidebarTabAction);
    
    showSidebarTabAction = viewMenu->addAction(tr("&Session Statistics"),
        this,
        [this]() {
            sidebar->setVisible(true);
            sidebar->setCurrentTabIndex(SessionStatsSidebarTab);
        });
    showSidebarTabAction->setShortcutContext(Qt::WindowShortcut);
    this->addAction(showSidebarTabAction);

    showSidebarTabAction = viewMenu->addAction(tr("&Document Statistics"),
        this,
        [this]() {
            sidebar->setVisible(true);
            sidebar->setCurrentTabIndex(DocumentStatsSidebarTab);
        });
    showSidebarTabAction->setShortcutContext(Qt::WindowShortcut);
    this->addAction(showSidebarTabAction);

    showSidebarTabAction = viewMenu->addAction(tr("&Cheat Sheet"),
        this,
        [this]() {
            sidebar->setVisible(true);
            sidebar->setCurrentTabIndex(CheatSheetSidebarTab);
        },
        QKeySequence::HelpContents);
    showSidebarTabAction->setShortcutContext(Qt::WindowShortcut);
    this->addAction(showSidebarTabAction);
    
    viewMenu->addSeparator();
    viewMenu->addAction(createWidgetAction(tr("Increase Font Size"), editor, SLOT(increaseFontSize()), QKeySequence("CTRL+=")));
    viewMenu->addAction(createWidgetAction(tr("Decrease Font Size"), editor, SLOT(decreaseFontSize()), QKeySequence("CTRL+-")));

    QMenu *settingsMenu = this->menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(createWindowAction(tr("Themes..."), this, SLOT(changeTheme())));
    settingsMenu->addAction(createWindowAction(tr("Font..."), this, SLOT(changeFont())));
    settingsMenu->addAction(createWindowAction(tr("Application Language..."), this, SLOT(onSetLocale())));
    settingsMenu->addAction(createWindowAction(tr("Preview Options..."), this, SLOT(showPreviewOptions())));
    QAction *preferencesAction = createWindowAction(tr("Preferences..."), this, SLOT(openPreferencesDialog()));
    preferencesAction->setMenuRole(QAction::PreferencesRole);
    settingsMenu->addAction(preferencesAction);

    QMenu *helpMenu = this->menuBar()->addMenu(tr("&Help"));
    QAction *helpAction = createWindowAction(tr("&About"), this, SLOT(showAbout()));
    helpAction->setMenuRole(QAction::AboutRole);
    helpMenu->addAction(helpAction);
    helpAction = createWindowAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
    helpAction->setMenuRole(QAction::AboutQtRole);
    helpMenu->addAction(helpAction);
    helpMenu->addAction(createWindowAction(tr("Quick &Reference Guide"), this, SLOT(showQuickReferenceGuide())));
    helpMenu->addAction(createWindowAction(tr("Wiki"), this, SLOT(showWikiPage())));

    connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(onAboutToShowMenuBarMenu()));
    connect(fileMenu, SIGNAL(aboutToHide()), this, SLOT(onAboutToHideMenuBarMenu()));
    connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(onAboutToShowMenuBarMenu()));
    connect(editMenu, SIGNAL(aboutToHide()), this, SLOT(onAboutToHideMenuBarMenu()));
    connect(formatMenu, SIGNAL(aboutToShow()), this, SLOT(onAboutToShowMenuBarMenu()));
    connect(formatMenu, SIGNAL(aboutToHide()), this, SLOT(onAboutToHideMenuBarMenu()));
    connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(onAboutToShowMenuBarMenu()));
    connect(viewMenu, SIGNAL(aboutToHide()), this, SLOT(onAboutToHideMenuBarMenu()));
    connect(settingsMenu, SIGNAL(aboutToShow()), this, SLOT(onAboutToShowMenuBarMenu()));
    connect(settingsMenu, SIGNAL(aboutToHide()), this, SLOT(onAboutToHideMenuBarMenu()));
    connect(helpMenu, SIGNAL(aboutToShow()), this, SLOT(onAboutToShowMenuBarMenu()));
    connect(helpMenu, SIGNAL(aboutToHide()), this, SLOT(onAboutToHideMenuBarMenu()));
}

void MainWindow::buildStatusBar()
{
    QGridLayout *statusBarLayout = new QGridLayout();
    statusBarLayout->setSpacing(0);
    statusBarLayout->setContentsMargins(0, 0, 0, 0);

    statusBarLayout->addWidget(this->findReplace, 0, 0, 1, 3);

    // Divide the status bar into thirds for placing widgets.
    QFrame *leftWidget = new QFrame(this->statusBar());
    leftWidget->setObjectName("leftStatusBarWidget");
    leftWidget->setStyleSheet("#leftStatusBarWidget { border: 0; margin: 0; padding: 0 }");
    QFrame *midWidget = new QFrame(this->statusBar());
    midWidget->setObjectName("midStatusBarWidget");
    midWidget->setStyleSheet("#midStatusBarWidget { border: 0; margin: 0; padding: 0 }");
    QFrame *rightWidget = new QFrame(this->statusBar());
    rightWidget->setObjectName("rightStatusBarWidget");
    rightWidget->setStyleSheet("#rightStatusBarWidget { border: 0; margin: 0; padding: 0 }");

    QHBoxLayout *leftLayout = new QHBoxLayout(leftWidget);
    leftWidget->setLayout(leftLayout);
    leftLayout->setMargin(0);
    QHBoxLayout *midLayout = new QHBoxLayout(midWidget);
    midWidget->setLayout(midLayout);
    midLayout->setMargin(0);
    QHBoxLayout *rightLayout = new QHBoxLayout(rightWidget);
    rightWidget->setLayout(rightLayout);
    rightLayout->setMargin(0);

    // Add left-most widgets to status bar.
    QFont buttonFont(this->awesome->font(style::stfas, 16));

    toggleSidebarButton = new QPushButton(QChar(fa::chevronright));
    toggleSidebarButton->setObjectName("showSidebarButton");
    toggleSidebarButton->setFont(buttonFont);
    toggleSidebarButton->setFocusPolicy(Qt::NoFocus);
    toggleSidebarButton->setToolTip(tr("Toggle sidebar"));
    toggleSidebarButton->setCheckable(false);
    toggleSidebarButton->setChecked(false);

    leftLayout->addWidget(toggleSidebarButton, 0, Qt::AlignLeft);
    statusBarWidgets.append(toggleSidebarButton);

    if (appSettings->sidebarVisible()) {
        toggleSidebarButton->setText(QChar(fa::chevronleft));
    }

    this->connect(toggleSidebarButton,
        &QPushButton::clicked,
        [this]() {
            toggleSidebarVisible(!sidebar->isVisible());
        }
    );
    
    timeLabel = new TimeLabel(this);
    leftLayout->addWidget(timeLabel, 0, Qt::AlignLeft);
    leftWidget->setContentsMargins(0, 0, 0, 0);
    statusBarWidgets.append(timeLabel);

    if (!this->isFullScreen() || appSettings->displayTimeInFullScreenEnabled()) {
        timeLabel->hide();
    }

    statusBarLayout->addWidget(leftWidget, 1, 0, 1, 1, Qt::AlignLeft);

    // Add middle widgets to status bar.
    statusLabel = new QLabel();
    midLayout->addWidget(statusLabel, 0, Qt::AlignCenter);
    statusLabel->hide();

    wordCountLabel = new QLabel();
    wordCountLabel->setAlignment(Qt::AlignCenter);
    wordCountLabel->setFrameShape(QFrame::NoFrame);
    wordCountLabel->setLineWidth(0);
    updateWordCount(0);
    midLayout->addWidget(wordCountLabel, 0, Qt::AlignCenter);
    midWidget->setContentsMargins(0, 0, 0, 0);
    statusBarLayout->addWidget(midWidget, 1, 1, 1, 1, Qt::AlignCenter);
    statusBarWidgets.append(wordCountLabel);

    // Add right-most widgets to status bar.
    QPushButton *button = new QPushButton(QChar(fa::moon));
    button->setFont(buttonFont);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolTip(tr("Toggle dark mode"));
    button->setCheckable(true);
    button->setChecked(appSettings->darkModeEnabled());

    this->connect
    (
        button,
        &QPushButton::toggled,
        [this](bool enabled) {
            appSettings->setDarkModeEnabled(enabled);
            applyTheme();
        }
    );

    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    button = new QPushButton(QChar(fa::code));
    button->setFont(buttonFont);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolTip(tr("Toggle Live HTML Preview"));
    button->setCheckable(true);
    button->setChecked(appSettings->htmlPreviewVisible());
    connect(button, SIGNAL(toggled(bool)), this, SLOT(toggleHtmlPreview(bool)));
    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    this->connect
    (
        this->htmlPreviewMenuAction,
        &QAction::toggled,
        [button](bool checked) {
            button->blockSignals(true);
            button->setChecked(checked);
            button->blockSignals(false);
        }
    );

    button = new QPushButton(QChar(fa::backspace));
    button->setFont(buttonFont);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolTip(tr("Toggle Hemingway mode"));
    button->setCheckable(true);
    connect(button, SIGNAL(toggled(bool)), this, SLOT(toggleHemingwayMode(bool)));
    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    button = new QPushButton(QChar(fa::headphonesalt));
    button->setFont(buttonFont);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolTip(tr("Toggle distraction free mode"));
    button->setCheckable(true);
    connect(button, SIGNAL(toggled(bool)), this, SLOT(toggleFocusMode(bool)));
    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);

    button = new QPushButton(QChar(fa::expand));
    button->setFont(buttonFont);
    button->setFocusPolicy(Qt::NoFocus);
    button->setObjectName("fullscreenButton");
    button->setToolTip(tr("Toggle full screen mode"));
    button->setCheckable(true);
    button->setChecked(this->isFullScreen());
    connect(button, SIGNAL(toggled(bool)), this, SLOT(toggleFullScreen(bool)));
    rightLayout->addWidget(button, 0, Qt::AlignRight);
    statusBarWidgets.append(button);
    this->fullScreenButton = button;

    rightWidget->setContentsMargins(0, 0, 0, 0);
    statusBarLayout->addWidget(rightWidget, 1, 2, 1, 1, Qt::AlignRight);
    
    QWidget *container = new QWidget(this);
    container->setObjectName("statusBarWidgetContainer");
    container->setLayout(statusBarLayout);
    container->setContentsMargins(0, 0, 2, 0);
    container->setStyleSheet("#statusBarWidgetContainer { border: 0; margin: 0; padding: 0 }");

    this->statusBar()->addWidget(container, 1);
    this->statusBar()->setSizeGripEnabled(false);
}

void MainWindow::buildSidebar()
{
    this->showSidebarAction = new QAction(tr("Show Sidebar"), this);
    showSidebarAction->setCheckable(true);
    showSidebarAction->setChecked(appSettings->sidebarVisible());
    showSidebarAction->setShortcut(QKeySequence("CTRL+SPACE"));
    showSidebarAction->setShortcutContext(Qt::WindowShortcut);
    
    connect(this->showSidebarAction,
        &QAction::toggled,
        this,
        &MainWindow::toggleSidebarVisible);

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

    documentStatsWidget = new DocumentStatisticsWidget();
    documentStatsWidget->setSelectionMode(QAbstractItemView::NoSelection);
    documentStatsWidget->setAlternatingRowColors(false);

    sessionStatsWidget = new SessionStatisticsWidget();
    sessionStatsWidget->setSelectionMode(QAbstractItemView::NoSelection);
    sessionStatsWidget->setAlternatingRowColors(false);

    outlineWidget = new OutlineWidget(editor, this);
    outlineWidget->setAlternatingRowColors(false);

    documentStats = new DocumentStatistics((MarkdownDocument *) editor->document(), this);
    connect(documentStats, SIGNAL(wordCountChanged(int)), documentStatsWidget, SLOT(setWordCount(int)));
    connect(documentStats, SIGNAL(characterCountChanged(int)), documentStatsWidget, SLOT(setCharacterCount(int)));
    connect(documentStats, SIGNAL(sentenceCountChanged(int)), documentStatsWidget, SLOT(setSentenceCount(int)));
    connect(documentStats, SIGNAL(paragraphCountChanged(int)), documentStatsWidget, SLOT(setParagraphCount(int)));
    connect(documentStats, SIGNAL(pageCountChanged(int)), documentStatsWidget, SLOT(setPageCount(int)));
    connect(documentStats, SIGNAL(complexWordsChanged(int)), documentStatsWidget, SLOT(setComplexWords(int)));
    connect(documentStats, SIGNAL(readingTimeChanged(int)), documentStatsWidget, SLOT(setReadingTime(int)));
    connect(documentStats, SIGNAL(lixReadingEaseChanged(int)), documentStatsWidget, SLOT(setLixReadingEase(int)));
    connect(documentStats, SIGNAL(readabilityIndexChanged(int)), documentStatsWidget, SLOT(setReadabilityIndex(int)));
    connect(editor, SIGNAL(textSelected(QString, int, int)), documentStats, SLOT(onTextSelected(QString, int, int)));
    connect(editor, SIGNAL(textDeselected()), documentStats, SLOT(onTextDeselected()));

    sessionStats = new SessionStatistics(this);
    connect(documentStats, SIGNAL(totalWordCountChanged(int)), sessionStats, SLOT(onDocumentWordCountChanged(int)));
    connect(sessionStats, SIGNAL(wordCountChanged(int)), sessionStatsWidget, SLOT(setWordCount(int)));
    connect(sessionStats, SIGNAL(pageCountChanged(int)), sessionStatsWidget, SLOT(setPageCount(int)));
    connect(sessionStats, SIGNAL(wordsPerMinuteChanged(int)), sessionStatsWidget, SLOT(setWordsPerMinute(int)));
    connect(sessionStats, SIGNAL(writingTimeChanged(unsigned long)), sessionStatsWidget, SLOT(setWritingTime(unsigned long)));
    connect(sessionStats, SIGNAL(idleTimePercentageChanged(int)), sessionStatsWidget, SLOT(setIdleTime(int)));
    connect(editor, SIGNAL(typingPaused()), sessionStats, SLOT(onTypingPaused()));
    connect(editor, SIGNAL(typingResumed()), sessionStats, SLOT(onTypingResumed()));

    sidebar = new Sidebar(this);
    sidebar->setMinimumWidth(0.10 * QGuiApplication::primaryScreen()->availableSize().width());
    sidebar->setMaximumWidth(0.33 * QGuiApplication::primaryScreen()->availableSize().width());

    QPushButton *tabButton = new QPushButton();
    tabButton->setFont(this->awesome->font(style::stfas, 16));
    tabButton->setText(QChar(fa::hashtag));
    tabButton->setToolTip(tr("Outline"));
    sidebar->addTab(tabButton, outlineWidget);

    tabButton = new QPushButton();
    tabButton->setFont(this->awesome->font(style::stfas, 16));
    tabButton->setText(QChar(fa::tachometeralt));
    tabButton->setToolTip(tr("Session Statistics"));
    sidebar->addTab(tabButton, sessionStatsWidget);

    tabButton = new QPushButton();
    tabButton->setFont(this->awesome->font(style::stfas, 16));
    tabButton->setText(QChar(fa::chartbar));
    tabButton->setToolTip(tr("Document Statistics"));
    sidebar->addTab(tabButton, documentStatsWidget);

    tabButton = new QPushButton();
    tabButton->setFont(this->awesome->font(style::stfab, 16));
    tabButton->setText(QChar(fa::markdown));
    tabButton->setToolTip(tr("Cheat Sheet"));
    sidebar->addTab(tabButton, cheatSheetWidget);

    // We need to set an empty style for the scrollbar in order for the
    // scrollbar CSS stylesheet to take full effect.  Otherwise, the scrollbar's
    // background color will have the Windows 98 checkered look rather than
    // being a solid or transparent color.
    //
    outlineWidget->verticalScrollBar()->setStyle(new QCommonStyle());
    outlineWidget->horizontalScrollBar()->setStyle(new QCommonStyle());
    documentStatsWidget->verticalScrollBar()->setStyle(new QCommonStyle());
    documentStatsWidget->horizontalScrollBar()->setStyle(new QCommonStyle());
    sessionStatsWidget->verticalScrollBar()->setStyle(new QCommonStyle());
    sessionStatsWidget->horizontalScrollBar()->setStyle(new QCommonStyle());
    cheatSheetWidget->verticalScrollBar()->setStyle(new QCommonStyle());
    cheatSheetWidget->horizontalScrollBar()->setStyle(new QCommonStyle());

    int tabIndex = QSettings().value("sidebarCurrentTab", (int)FirstSidebarTab).toInt();

    if (tabIndex < 0 || tabIndex >= sidebar->tabCount()) {
        tabIndex = (int) FirstSidebarTab;
    }

    sidebar->setCurrentTabIndex(tabIndex);

    QPushButton *button = new QPushButton(QChar(fa::cog));
    button->setFont(this->awesome->font(style::stfas, 16));
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolTip(tr("Settings"));
    button->setCheckable(false);
    sidebar->addButton(button);
    this->connect
    (
        button,
        &QPushButton::clicked,
        [this, button]() {
            QMenu *popupMenu = new QMenu(button);
            popupMenu->addAction(tr("Themes..."), this, SLOT(changeTheme()));
            popupMenu->addAction(tr("Font..."), this, SLOT(changeFont()));
            popupMenu->addAction(tr("Application Language..."), this, SLOT(onSetLocale()));
            popupMenu->addAction(tr("Preview Options..."), this, SLOT(showPreviewOptions()));
            popupMenu->addAction(tr("Preferences..."), this, SLOT(openPreferencesDialog()))->setMenuRole(QAction::PreferencesRole);
            popupMenu->popup(button->mapToGlobal(QPoint(button->width() / 2, -(button->height() / 2) - 10)));
        }
    );

    this->connect(this->sidebar,
        &Sidebar::visibilityChanged,
        this,
        &MainWindow::onSidebarVisibilityChanged);
    
    this->toggleSidebarVisible(appSettings->sidebarVisible());
}

void MainWindow::adjustEditorWidth(int width)
{
    QList<int> sidebarSplitterSizes;
    QList<int> previewSplitterSizes;
    int editorWidth = width;

    if (width < (0.5 * qApp->primaryScreen()->size().width())) {
        sidebar->setMinimumWidth(0);
        sidebarSplitterSizes.append(0);
    } else {
        sidebar->setMinimumWidth(0.1 * qApp->primaryScreen()->size().width());
        sidebar->setMaximumWidth(0.5 * this->width());
        sidebar->resize(sidebar->sizeHint().width(), this->height());
        sidebarSplitterSizes.append(sidebar->width());
        editorWidth -= sidebar->width();
    }

    sidebarSplitterSizes.append(editorWidth);

    if (htmlPreview->isVisible()) {
        editorWidth /= 2;
        previewSplitterSizes.append(editorWidth);
    }

    previewSplitterSizes.append(editorWidth);
    previewSplitter->setSizes(previewSplitterSizes);
    sidebarSplitter->setSizes(sidebarSplitterSizes);

    // Resize the editor's margins based on the size of the window.
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

    StyleSheetBuilder styler(colorScheme,
        (InterfaceStyleRounded == appSettings->interfaceStyle()),
        appSettings->previewTextFont(),
        appSettings->previewCodeFont());

    editor->setColorScheme(colorScheme);
    editor->setStyleSheet(styler.editorStyleSheet());

    // Do not call this->setStyleSheet().  Calling it more than once in a run
    // (i.e., when changing a theme) causes a crash in Qt 5.11.  Instead,
    // change the main window's style sheet via qApp.
    //
    qApp->setStyleSheet(styler.layoutStyleSheet());

    previewSplitter->setStyleSheet(styler.splitterStyleSheet());
    sidebarSplitter->setStyleSheet(styler.splitterStyleSheet());
    this->statusBar()->setStyleSheet(styler.statusBarStyleSheet());

    foreach (QWidget *w, statusBarWidgets) {
        w->setStyleSheet(styler.statusBarWidgetsStyleSheet());
    }

    findReplace->setStyleSheet(styler.findReplaceStyleSheet());
    sidebar->setStyleSheet(styler.sidebarStyleSheet());

    // Clear style sheet cache by setting to empty string before
    // setting the new style sheet.
    //
    outlineWidget->setStyleSheet("");
    outlineWidget->setStyleSheet(styler.sidebarWidgetStyleSheet());
    cheatSheetWidget->setStyleSheet("");
    cheatSheetWidget->setStyleSheet(styler.sidebarWidgetStyleSheet());
    documentStatsWidget->setStyleSheet("");
    documentStatsWidget->setStyleSheet(styler.sidebarWidgetStyleSheet());
    sessionStatsWidget->setStyleSheet("");
    sessionStatsWidget->setStyleSheet(styler.sidebarWidgetStyleSheet());

    htmlPreview->setStyleSheet(styler.htmlPreviewCss());

    adjustEditorWidth(this->width());
}

} // namespace ghostwriter
