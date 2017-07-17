/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
 * Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QSettings>
#include <QFontInfo>
#include <QFontDatabase>
#include <QStringList>
#include <QDebug>

#include "AppSettings.h"
#include "ExporterFactory.h"
#include "dictionary_manager.h"

#define GW_AUTOSAVE_KEY "Save/autoSave"
#define GW_BACKUP_FILE_KEY "Save/backupFile"
#define GW_REMEMBER_FILE_HISTORY_KEY "Save/rememberFileHistory"
#define GW_FONT_KEY "Style/font"
#define GW_LARGE_HEADINGS_KEY "Style/largeHeadings"
#define GW_HIGHLIGHT_LINE_BREAKS "Style/highlightLineBreaks"
#define GW_AUTO_MATCH_KEY "Typing/autoMatchEnabled"
#define GW_AUTO_MATCH_FILTER_KEY "Typing/autoMatchFilter"
#define GW_BULLET_CYCLING_KEY "Typing/bulletPointCyclingEnabled"
#define GW_UNDERLINE_ITALICS_KEY "Style/underlineInsteadOfItalics"
#define GW_FOCUS_MODE_KEY "Style/focusMode"
#define GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY "Style/hideMenuBarInFullScreenEnabled"
#define GW_THEME_KEY "Style/theme"
#define GW_EDITOR_WIDTH_KEY "Style/editorWidth"
#define GW_INTERFACE_STYLE_KEY "Style/interfaceStyle"
#define GW_BLOCKQUOTE_STYLE_KEY "Style/blockquoteStyle"
#define GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY "Style/displayTimeInFullScreen"
#define GW_TAB_WIDTH_KEY "Tabs/tabWidth"
#define GW_SPACES_FOR_TABS_KEY "Tabs/insertSpacesForTabs"
#define GW_DICTIONARY_KEY "Spelling/locale"
#define GW_LOCALE_KEY "Application/locale"
#define GW_LIVE_SPELL_CHECK_KEY "Spelling/liveSpellCheck"
#define GW_HUD_BUTTON_LAYOUT_KEY "HUD/windowButtonLayout"
#define GW_HUD_ROW_COLORS_KEY "HUD/alternateRowColors"
#define GW_DESKTOP_COMPOSITING_KEY "HUD/desktopCompositingEnabled"
#define GW_HUD_OPACITY_KEY "HUD/opacity"
#define GW_HTML_PREVIEW_OPEN_KEY "Preview/htmlPreviewOpen"
#define GW_CUSTOM_STYLE_SHEETS_KEY "Preview/customStyleSheets"
#define GW_LAST_USED_STYLE_SHEET_KEY "Preview/lastUsedStyleSheet"
#define GW_LAST_USED_EXPORTER_KEY "Preview/lastUsedExporter"

AppSettings* AppSettings::instance = NULL;

AppSettings* AppSettings::getInstance()
{
    if (NULL == instance)
    {
        instance = new AppSettings();
    }

    return instance;
}

AppSettings::~AppSettings()
{
    store();
}

void AppSettings::store()
{
    QSettings appSettings;
    appSettings.setValue(GW_AUTOSAVE_KEY, QVariant(autoSaveEnabled));
    appSettings.setValue(GW_BACKUP_FILE_KEY, QVariant(backupFileEnabled));
    appSettings.setValue(GW_FONT_KEY, QVariant(font.toString()));
    appSettings.setValue(GW_TAB_WIDTH_KEY, QVariant(tabWidth));
    appSettings.setValue(GW_SPACES_FOR_TABS_KEY, QVariant(insertSpacesForTabsEnabled));
    appSettings.setValue(GW_LARGE_HEADINGS_KEY, QVariant(largeHeadingSizesEnabled));
    appSettings.setValue(GW_HIGHLIGHT_LINE_BREAKS, QVariant(highlightLineBreaks));
    appSettings.setValue(GW_AUTO_MATCH_KEY, QVariant(autoMatchEnabled));
    appSettings.setValue(GW_AUTO_MATCH_FILTER_KEY, QVariant(autoMatchedCharFilter));
    appSettings.setValue(GW_BULLET_CYCLING_KEY, QVariant(bulletPointCyclingEnabled));

    appSettings.setValue(GW_UNDERLINE_ITALICS_KEY, QVariant(useUnderlineForEmphasis));
    appSettings.setValue(GW_FOCUS_MODE_KEY, QVariant(focusMode));
    appSettings.setValue(GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY, QVariant(hideMenuBarInFullScreenEnabled));
    appSettings.setValue(GW_REMEMBER_FILE_HISTORY_KEY, QVariant(fileHistoryEnabled));
    appSettings.setValue(GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY, QVariant(displayTimeInFullScreenEnabled));

    appSettings.setValue(GW_THEME_KEY, QVariant(themeName));
    appSettings.setValue(GW_DICTIONARY_KEY, QVariant(dictionaryLanguage));
    appSettings.setValue(GW_LOCALE_KEY, QVariant(locale));
    appSettings.setValue(GW_LIVE_SPELL_CHECK_KEY, QVariant(liveSpellCheckEnabled));
    appSettings.setValue(GW_EDITOR_WIDTH_KEY, QVariant(editorWidth));
    appSettings.setValue(GW_INTERFACE_STYLE_KEY, QVariant(interfaceStyle));
    appSettings.setValue(GW_BLOCKQUOTE_STYLE_KEY, QVariant(blockquoteStyle));
    appSettings.setValue(GW_HUD_BUTTON_LAYOUT_KEY, QVariant(hudButtonLayout));
    appSettings.setValue(GW_HUD_ROW_COLORS_KEY, QVariant(alternateHudRowColorsEnabled));
    appSettings.setValue(GW_DESKTOP_COMPOSITING_KEY, QVariant(desktopCompositingEnabled));
    appSettings.setValue(GW_HUD_OPACITY_KEY, QVariant(hudOpacity));

    appSettings.setValue(GW_HTML_PREVIEW_OPEN_KEY, QVariant(htmlPreviewVisible));
    appSettings.setValue(GW_CUSTOM_STYLE_SHEETS_KEY, QVariant(customCssFiles));
    appSettings.setValue(GW_LAST_USED_STYLE_SHEET_KEY, QVariant(currentCssFile));
    appSettings.setValue(GW_LAST_USED_EXPORTER_KEY, QVariant(currentHtmlExporter->getName()));

    appSettings.sync();
}

QString AppSettings::getThemeDirectoryPath() const
{
    return themeDirectoryPath;
}

QString AppSettings::getDictionaryPath() const
{
    return dictionaryPath;
}

QString AppSettings::getTranslationsPath() const
{
    return translationsPath;
}

bool AppSettings::getAutoSaveEnabled() const
{
    return autoSaveEnabled;
}

void AppSettings::setAutoSaveEnabled(bool enabled)
{
    autoSaveEnabled = enabled;
    emit autoSaveChanged(enabled);
}

bool AppSettings::getBackupFileEnabled() const
{
    return backupFileEnabled;
}

void AppSettings::setBackupFileEnabled(bool enabled)
{
    backupFileEnabled = enabled;
    emit backupFileChanged(enabled);
    qWarning("Backup file enabled = %d", backupFileEnabled);
}

QFont AppSettings::getFont() const
{
    return font;
}

void AppSettings::setFont(const QFont& font)
{
    this->font = font;
}

int AppSettings::getTabWidth() const
{
    return tabWidth;
}

void AppSettings::setTabWidth(int width)
{
    if ((tabWidth >= MIN_TAB_WIDTH) && (tabWidth <= MAX_TAB_WIDTH))
    {
        tabWidth = width;
        emit tabWidthChanged(width);
    }
}

bool AppSettings::getInsertSpacesForTabsEnabled() const
{
    return insertSpacesForTabsEnabled;
}

void AppSettings::setInsertSpacesForTabsEnabled(bool enabled)
{
    insertSpacesForTabsEnabled = enabled;
    emit insertSpacesForTabsChanged(enabled);
}

bool AppSettings::getUseUnderlineForEmphasis() const
{
    return useUnderlineForEmphasis;
}

void AppSettings::setUseUnderlineForEmphasis(bool enabled)
{
    useUnderlineForEmphasis = enabled;
    emit useUnderlineForEmphasisChanged(enabled);
}

bool AppSettings::getLargeHeadingSizesEnabled() const
{
    return largeHeadingSizesEnabled;
}

void AppSettings::setLargeHeadingSizesEnabled(bool enabled)
{
    largeHeadingSizesEnabled = enabled;
    emit largeHeadingSizesChanged(enabled);
}

bool AppSettings::getAutoMatchEnabled() const
{
    return autoMatchEnabled;
}

void AppSettings::setAutoMatchEnabled(bool enabled)
{
    autoMatchEnabled = enabled;
    emit autoMatchChanged(enabled);
}

bool AppSettings::getAutoMatchCharEnabled(const QChar openingCharacter) const
{
    return autoMatchedCharFilter.contains(openingCharacter);
}

void AppSettings::setAutoMatchCharEnabled(const QChar openingCharacter, bool enabled)
{
    switch (openingCharacter.toLatin1())
    {
        case '\"':
        case '\'':
        case '(':
        case '[':
        case '{':
        case '*':
        case '_':
        case '`':
        case '<':
            if (enabled)
            {
                if (!autoMatchedCharFilter.contains(openingCharacter))
                {
                    autoMatchedCharFilter.append(openingCharacter);
                }
            }
            else
            {
                autoMatchedCharFilter.remove(openingCharacter);
            }

            emit autoMatchCharChanged(openingCharacter, enabled);

            break;
        default:
            break;
    }
}

bool AppSettings::getBulletPointCyclingEnabled() const
{
    return bulletPointCyclingEnabled;
}

void AppSettings::setBulletPointCyclingEnabled(bool enabled)
{
    bulletPointCyclingEnabled = enabled;
    emit bulletPointCyclingChanged(enabled);
}

FocusMode AppSettings::getFocusMode() const
{
    return focusMode;
}

void AppSettings::setFocusMode(FocusMode focusMode)
{
    if ((focusMode >= FocusModeFirst) && (focusMode <= FocusModeLast))
    {
        this->focusMode = focusMode;
        emit focusModeChanged(focusMode);
    }
}

bool AppSettings::getHideMenuBarInFullScreenEnabled() const
{
    return hideMenuBarInFullScreenEnabled;
}

void AppSettings::setHideMenuBarInFullScreenEnabled(bool enabled)
{
    hideMenuBarInFullScreenEnabled = enabled;
    emit hideMenuBarInFullScreenChanged(enabled);
}

bool AppSettings::getFileHistoryEnabled() const
{
    return fileHistoryEnabled;
}

void AppSettings::setFileHistoryEnabled(bool enabled)
{
    fileHistoryEnabled = enabled;
    emit fileHistoryChanged(enabled);
}

bool AppSettings::getDisplayTimeInFullScreenEnabled()
{
    return displayTimeInFullScreenEnabled;
}

void AppSettings::setDisplayTimeInFullScreenEnabled(bool enabled)
{
    displayTimeInFullScreenEnabled = enabled;
    emit displayTimeInFullScreenChanged(enabled);
}

QString AppSettings::getThemeName() const
{
    return themeName;
}

void AppSettings::setThemeName(const QString& name)
{
    themeName = name;
}

QString AppSettings::getDictionaryLanguage() const
{
    return dictionaryLanguage;
}

void AppSettings::setDictionaryLanguage(const QString& language)
{
    this->dictionaryLanguage = language;
    emit dictionaryLanguageChanged(language);
}

QString AppSettings::getLocale() const
{
    return locale;
}

void AppSettings::setLocale(const QString& locale)
{
    this->locale = locale;
}

bool AppSettings::getLiveSpellCheckEnabled() const
{
    return liveSpellCheckEnabled;
}

void AppSettings::setLiveSpellCheckEnabled(bool enabled)
{
    liveSpellCheckEnabled = enabled;
    emit liveSpellCheckChanged(enabled);
}

EditorWidth AppSettings::getEditorWidth() const
{
    return editorWidth;
}

void AppSettings::setEditorWidth(EditorWidth editorWidth)
{
    if ((editorWidth >= EditorWidthFirst) && (editorWidth <= EditorWidthLast))
    {
        this->editorWidth = editorWidth;
        emit editorWidthChanged(editorWidth);
    }
}

InterfaceStyle AppSettings::getInterfaceStyle() const
{
    return interfaceStyle;
}

void AppSettings::setInterfaceStyle(InterfaceStyle style)
{
    interfaceStyle = style;
    emit interfaceStyleChanged(style);
}

BlockquoteStyle AppSettings::getBlockquoteStyle() const
{
    return blockquoteStyle;
}

void AppSettings::setBlockquoteStyle(BlockquoteStyle style)
{
    if ((style >= BlockquoteStyleFirst) && (style <= BlockquoteStyleLast))
    {
        blockquoteStyle = style;
        emit blockquoteStyleChanged(style);
    }
}

HudWindowButtonLayout AppSettings::getHudButtonLayout() const
{
    return hudButtonLayout;
}

void AppSettings::setHudButtonLayout(HudWindowButtonLayout layout)
{
    hudButtonLayout = layout;
    emit hudButtonLayoutChanged(layout);
}

bool AppSettings::getAlternateHudRowColorsEnabled() const
{
    return alternateHudRowColorsEnabled;
}

void AppSettings::setAlternateHudRowColorsEnabled(bool enabled)
{
    alternateHudRowColorsEnabled = enabled;
    emit alternateHudRowColorsChanged(enabled);
}

bool AppSettings::getDesktopCompositingEnabled() const
{
    return desktopCompositingEnabled;
}

void AppSettings::setDesktopCompositingEnabled(bool enabled)
{
    desktopCompositingEnabled = enabled;
    emit desktopCompositingChanged(enabled);
}

int AppSettings::getHudOpacity() const
{
    return hudOpacity;
}

void AppSettings::setHudOpacity(int value)
{
    hudOpacity = value;
    emit hudOpacityChanged(value);
}

bool AppSettings::getHighlightLineBreaks() const
{
    return highlightLineBreaks;
}

void AppSettings::setHighlightLineBreaks(bool enabled)
{
    highlightLineBreaks = enabled;
    emit highlightLineBreaksChanged(enabled);
}

bool AppSettings::getHtmlPreviewVisible() const
{
    return htmlPreviewVisible;
}

void AppSettings::setHtmlPreviewVisible(bool visible)
{
    htmlPreviewVisible = visible;
}

QStringList AppSettings::getCustomCssFiles() const
{
    return customCssFiles;
}

void AppSettings::setCustomCssFiles(const QStringList& cssFilePathList)
{
    customCssFiles = cssFilePathList;
    emit customCssFilesChanged(customCssFiles);
}

QString AppSettings::getCurrentCssFile() const
{
    return currentCssFile;
}

void AppSettings::setCurrentCssFile(const QString& filePath)
{
    bool emitChangeSignal = (filePath != currentCssFile);
    currentCssFile = filePath;

    // Only emit the property changed signal if it actually changed.
    if (emitChangeSignal)
    {
        emit currentCssFileChanged(filePath);
    }
}

Exporter *AppSettings::getCurrentHtmlExporter() const
{
    return currentHtmlExporter;
}

void AppSettings::setCurrentHtmlExporter(Exporter* exporter)
{
    currentHtmlExporter = exporter;
    emit currentHtmlExporterChanged(exporter);
}

AppSettings::AppSettings()
{
    QCoreApplication::setOrganizationName("ghostwriter");
    QCoreApplication::setApplicationName("ghostwriter");
    QCoreApplication::setApplicationVersion(APPVERSION);

    // The following was lifted/modded from FocusWriter.
    // See GPL license at the beginning of this file.
    //
    QString appDir = qApp->applicationDirPath();

    // Set up portable settings directories.
#if defined(Q_OS_MAC)
    QFileInfo portable(appDir + "/../../../data");
#elif defined(Q_OS_UNIX)
    QFileInfo portable(appDir + "/data");
#else
    QFileInfo portable(appDir + "/data");
#endif

    // Handle portability
    QString userDir;

    if (portable.exists() && portable.isWritable())
    {
        userDir = portable.absoluteFilePath();
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath
        (
            QSettings::IniFormat,
            QSettings::UserScope,
            userDir + "/settings"
        );

        translationsPath = appDir + "/translations";
    }
    else
    {
#ifdef Q_OS_WIN32
        // On Windows, don't ever use the registry to store settings, for the
        // sake of cleanness, ability to load configuration files on other
        // machines, and also for the user's privacy.
        //
        QSettings::setDefaultFormat(QSettings::IniFormat);
#endif
        QSettings settings;
        userDir = QFileInfo(settings.fileName()).dir().absolutePath();

        QStringList translationPaths;
        translationPaths.append(appDir + "/translations");
        translationPaths.append(appDir + "/../share/" +
            QCoreApplication::applicationName().toLower() +
            "/translations");
        translationPaths.append(appDir + "/../Resources/translations");

        foreach (const QString& path, translationPaths)
        {
            if (QFile::exists(path))
            {
                translationsPath = path;
                break;
            }
        }
    }

    QDir themeDir(userDir + "/themes");

    if (!themeDir.exists())
    {
        themeDir.mkpath(themeDir.path());
    }

    themeDirectoryPath = themeDir.absolutePath();

    QDir dictionaryDir(userDir + "/dictionaries");

    if (!dictionaryDir.exists())
    {
        dictionaryDir.mkpath(dictionaryDir.path());
    }

    dictionaryPath = dictionaryDir.absolutePath();
    DictionaryManager::setPath(dictionaryPath);

    QStringList dictdirs;
    dictdirs.append(DictionaryManager::path());

    dictionaryDir = QDir(appDir + "/dictionaries");

    if (dictionaryDir.exists())
    {
        dictdirs.append(dictionaryDir.path());
    }

    QDir::setSearchPaths("dict", dictdirs);


    // End FocusWriter lift/mod

    // Depending on the OS and Qt version, the default monospaced font returned
    // by the Monospace style hint and/or font family tends to something not
    // even monospaced, or, at best "Courier".  QTBUG-34082 seems to be what is
    // causing the issue.  Regardless, we want the prettiest monospaced font
    // available, so see which preferred fonts are available on the system
    // before before resorting to style hints.
    //
    QFontDatabase fontDb;

    QStringList fontFamilies = fontDb.families();
    QStringList preferredFonts;

#ifdef Q_OS_MAC
    preferredFonts.append("Menlo");
#endif

    preferredFonts.append("DejaVu Sans Mono");

#ifdef Q_OS_MAC
    preferredFonts.append("Monaco");
#elif defined(Q_OS_LINUX)
    preferredFonts.append("Ubuntu Mono");
    preferredFonts.append("Liberation Mono");
#elif defined(Q_OS_WIN32)
    preferredFonts.append("Consolas");
    preferredFonts.append("Lucida Console");
#endif

    preferredFonts.append("Courier New");
    preferredFonts.append("Courier");

    // Pick the first font in the list of preferredFonts that is installed
    // (i.e., in the font database) to use as the default font on the very
    // first start up of this program.
    //
    bool fontMatchFound = false;

    for (int i = 0; i < preferredFonts.size(); i++)
    {
        fontMatchFound =
            std::binary_search
            (
                fontFamilies.begin(),
                fontFamilies.end(),
                preferredFonts[i]
            );

        if (fontMatchFound)
        {
            defaultFont = QFont(preferredFonts[i]);
            break;
        }
    }

    if (!fontMatchFound)
    {
        // This case should not really happen, since the Courier family is
        // cross-platform.  This is just a precaution.
        //
        qWarning() <<
            "No fixed-width fonts were found. Using sytem default...";
        defaultFont = QFont("");
        defaultFont.setFixedPitch(true);
        defaultFont.setStyleHint(QFont::Monospace);
    }

    // Last but not least, load some basic settings from the configuration file,
    // but only those that need special validation.  Things like window
    // dimensions and file history can be handled elsewhere.
    //
    QSettings appSettings;

    autoSaveEnabled = appSettings.value(GW_AUTOSAVE_KEY, QVariant(true)).toBool();
    backupFileEnabled = appSettings.value(GW_BACKUP_FILE_KEY, QVariant(true)).toBool();

    font = defaultFont;
    font.fromString(appSettings.value(GW_FONT_KEY, QVariant(defaultFont.toString())).toString());

    tabWidth = appSettings.value(GW_TAB_WIDTH_KEY, QVariant(DEFAULT_TAB_WIDTH)).toInt();

    if ((tabWidth < MIN_TAB_WIDTH) || (tabWidth > MAX_TAB_WIDTH))
    {
        tabWidth = DEFAULT_TAB_WIDTH;
    }

    insertSpacesForTabsEnabled = appSettings.value(GW_SPACES_FOR_TABS_KEY, QVariant(false)).toBool();
    useUnderlineForEmphasis = appSettings.value(GW_UNDERLINE_ITALICS_KEY, QVariant(false)).toBool();
    largeHeadingSizesEnabled = appSettings.value(GW_LARGE_HEADINGS_KEY, QVariant(true)).toBool();
    highlightLineBreaks = appSettings.value(GW_HIGHLIGHT_LINE_BREAKS, QVariant(false)).toBool();
    autoMatchEnabled = appSettings.value(GW_AUTO_MATCH_KEY, QVariant(true)).toBool();
    autoMatchedCharFilter = appSettings.value(GW_AUTO_MATCH_FILTER_KEY, QVariant("\"\'([{*_`<")).toString();
    bulletPointCyclingEnabled = appSettings.value(GW_BULLET_CYCLING_KEY, QVariant(true)).toBool();
    focusMode = (FocusMode) appSettings.value(GW_FOCUS_MODE_KEY, QVariant(FocusModeSentence)).toInt();

    if ((focusMode < FocusModeFirst) || (focusMode > FocusModeLast))
    {
        focusMode = FocusModeSentence;
    }

    hideMenuBarInFullScreenEnabled = appSettings.value(GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY, QVariant(true)).toBool();
    fileHistoryEnabled = appSettings.value(GW_REMEMBER_FILE_HISTORY_KEY, QVariant(true)).toBool();
    displayTimeInFullScreenEnabled = appSettings.value(GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY, QVariant(true)).toBool();
    themeName = appSettings.value(GW_THEME_KEY, QVariant("Classic Light")).toString();
    dictionaryLanguage = appSettings.value(GW_DICTIONARY_KEY, QLocale().name()).toString();

    // Determine locale for dictionary language (for use in spell checking).
    QString language = DictionaryManager::instance().availableDictionary(dictionaryLanguage);

    // If we have an available dictionary, then set the default dictionary language.
    if (!language.isNull() && !language.isEmpty())
    {
        DictionaryManager::instance().setDefaultLanguage(language);
    }

    locale = appSettings.value(GW_LOCALE_KEY, QLocale().name()).toString();
    liveSpellCheckEnabled = appSettings.value(GW_LIVE_SPELL_CHECK_KEY, QVariant(true)).toBool();
    editorWidth = (EditorWidth) appSettings.value(GW_EDITOR_WIDTH_KEY, QVariant(EditorWidthMedium)).toInt();
    interfaceStyle = (InterfaceStyle) appSettings.value(GW_INTERFACE_STYLE_KEY, QVariant(InterfaceStyleRounded)).toInt();
    blockquoteStyle = (BlockquoteStyle) appSettings.value(GW_BLOCKQUOTE_STYLE_KEY, QVariant(BlockquoteStylePlain)).toInt();

    if ((editorWidth < EditorWidthFirst) || (editorWidth > EditorWidthLast))
    {
        editorWidth = EditorWidthMedium;
    }

    if ((interfaceStyle < InterfaceStyleFirst) || (interfaceStyle > InterfaceStyleLast))
    {
        interfaceStyle = InterfaceStyleRounded;
    }

    if ((blockquoteStyle < BlockquoteStyleFirst) || (blockquoteStyle > BlockquoteStyleLast))
    {
        blockquoteStyle = BlockquoteStylePlain;
    }

#ifdef Q_OS_MAC
    HudWindowButtonLayout defaultHudButtonLayout = HudWindowButtonLayoutLeft;
#else
    HudWindowButtonLayout defaultHudButtonLayout = HudWindowButtonLayoutRight;
#endif

    hudButtonLayout = (HudWindowButtonLayout) appSettings.value(GW_HUD_BUTTON_LAYOUT_KEY, QVariant(defaultHudButtonLayout)).toInt();
    alternateHudRowColorsEnabled = appSettings.value(GW_HUD_ROW_COLORS_KEY, QVariant(false)).toBool();
    desktopCompositingEnabled = appSettings.value(GW_DESKTOP_COMPOSITING_KEY, QVariant(true)).toBool();
    hudOpacity = appSettings.value(GW_HUD_OPACITY_KEY, QVariant(200)).toInt();
    htmlPreviewVisible = appSettings.value(GW_HTML_PREVIEW_OPEN_KEY, QVariant(false)).toBool();
    customCssFiles = appSettings.value(GW_CUSTOM_STYLE_SHEETS_KEY, QStringList()).toStringList();
    currentCssFile = appSettings.value(GW_LAST_USED_STYLE_SHEET_KEY).toString();

    QString exporterName = appSettings.value(GW_LAST_USED_EXPORTER_KEY).toString();
    currentHtmlExporter = ExporterFactory::getInstance()->getExporterByName(exporterName);

    if (NULL == currentHtmlExporter)
    {
        currentHtmlExporter = ExporterFactory::getInstance()->getHtmlExporters().first();
    }
}
