/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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
#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QFontInfo>
#include <QLocale>
#include <QSettings>
#include <QStringList>

#include "appsettings.h"
#include "dictionary_manager.h"
#include "exporterfactory.h"

#ifndef APPVERSION
#define APPVERSION "v2.0.0-rc"
#endif

#define GW_AUTOSAVE_KEY "Save/autoSave"
#define GW_BACKUP_FILE_KEY "Save/backupFile"
#define GW_REMEMBER_FILE_HISTORY_KEY "Save/rememberFileHistory"
#define GW_FONT_KEY "Style/font"
#define GW_LARGE_HEADINGS_KEY "Style/largeHeadings"
#define GW_AUTO_MATCH_KEY "Typing/autoMatchEnabled"
#define GW_AUTO_MATCH_FILTER_KEY "Typing/autoMatchFilter"
#define GW_BULLET_CYCLING_KEY "Typing/bulletPointCyclingEnabled"
#define GW_UNDERLINE_ITALICS_KEY "Style/underlineInsteadOfItalics"
#define GW_FOCUS_MODE_KEY "Style/focusMode"
#define GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY "Style/hideMenuBarInFullScreenEnabled"
#define GW_THEME_KEY "Style/theme"
#define GW_DARK_MODE_KEY "Style/darkModeEnabled"
#define GW_EDITOR_WIDTH_KEY "Style/editorWidth"
#define GW_INTERFACE_STYLE_KEY "Style/interfaceStyle"
#define GW_BLOCKQUOTE_STYLE_KEY "Style/blockquoteStyle"
#define GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY "Style/displayTimeInFullScreen"
#define GW_TAB_WIDTH_KEY "Tabs/tabWidth"
#define GW_SPACES_FOR_TABS_KEY "Tabs/insertSpacesForTabs"
#define GW_DICTIONARY_KEY "Spelling/locale"
#define GW_LOCALE_KEY "Application/locale"
#define GW_LIVE_SPELL_CHECK_KEY "Spelling/liveSpellCheck"
#define GW_SIDEBAR_OPEN_KEY "Window/sidebarOpen"
#define GW_HTML_PREVIEW_OPEN_KEY "Preview/htmlPreviewOpen"
#define GW_LAST_USED_EXPORTER_KEY "Preview/lastUsedExporter"

namespace ghostwriter
{
class AppSettingsPrivate
{
public:
    static AppSettings *instance;

    AppSettingsPrivate()
    {
        ;
    }

    ~AppSettingsPrivate()
    {
        ;
    }

    bool autoMatchEnabled;
    bool autoSaveEnabled;
    bool backupFileEnabled;
    bool bulletPointCyclingEnabled;
    bool displayTimeInFullScreenEnabled;
    bool fileHistoryEnabled;
    bool hideMenuBarInFullScreenEnabled;
    bool htmlPreviewVisible;
    bool sidebarVisible;
    bool insertSpacesForTabsEnabled;
    bool largeHeadingSizesEnabled;
    bool liveSpellCheckEnabled;
    bool useUnderlineForEmphasis;
    EditorWidth editorWidth;
    Exporter *currentHtmlExporter;
    FocusMode focusMode;
    int tabWidth;
    InterfaceStyle interfaceStyle;
    bool italicizeBlockquotes;
    QFont defaultFont;
    QFont font;
    QString autoMatchedCharFilter;
    QString dictionaryLanguage;
    QString dictionaryPath;
    QString locale;
    QString themeDirectoryPath;
    QString themeName;
    bool darkModeEnabled;
    QString translationsPath;
};

AppSettings *AppSettingsPrivate::instance = NULL;

AppSettings *AppSettings::instance()
{
    if (NULL == AppSettingsPrivate::instance) {
        AppSettingsPrivate::instance = new AppSettings();
    }

    return AppSettingsPrivate::instance;
}

AppSettings::~AppSettings()
{
    store();
}

void AppSettings::store()
{
    QSettings appSettings;

    appSettings.setValue(GW_AUTO_MATCH_FILTER_KEY, QVariant(d_func()->autoMatchedCharFilter));
    appSettings.setValue(GW_AUTO_MATCH_KEY, QVariant(d_func()->autoMatchEnabled));
    appSettings.setValue(GW_AUTOSAVE_KEY, QVariant(d_func()->autoSaveEnabled));
    appSettings.setValue(GW_BACKUP_FILE_KEY, QVariant(d_func()->backupFileEnabled));
    appSettings.setValue(GW_BULLET_CYCLING_KEY, QVariant(d_func()->bulletPointCyclingEnabled));
    appSettings.setValue(GW_DICTIONARY_KEY, QVariant(d_func()->dictionaryLanguage));
    appSettings.setValue(GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY, QVariant(d_func()->displayTimeInFullScreenEnabled));
    appSettings.setValue(GW_EDITOR_WIDTH_KEY, QVariant(d_func()->editorWidth));
    appSettings.setValue(GW_FOCUS_MODE_KEY, QVariant(d_func()->focusMode));
    appSettings.setValue(GW_FONT_KEY, QVariant(d_func()->font.toString()));
    appSettings.setValue(GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY, QVariant(d_func()->hideMenuBarInFullScreenEnabled));
    appSettings.setValue(GW_INTERFACE_STYLE_KEY, QVariant(d_func()->interfaceStyle));
    appSettings.setValue(GW_BLOCKQUOTE_STYLE_KEY, QVariant(d_func()->italicizeBlockquotes));
    appSettings.setValue(GW_LARGE_HEADINGS_KEY, QVariant(d_func()->largeHeadingSizesEnabled));
    appSettings.setValue(GW_SIDEBAR_OPEN_KEY, QVariant(d_func()->sidebarVisible));
    appSettings.setValue(GW_HTML_PREVIEW_OPEN_KEY, QVariant(d_func()->htmlPreviewVisible));
    appSettings.setValue(GW_LAST_USED_EXPORTER_KEY, QVariant(d_func()->currentHtmlExporter->name()));
    appSettings.setValue(GW_LIVE_SPELL_CHECK_KEY, QVariant(d_func()->liveSpellCheckEnabled));
    appSettings.setValue(GW_LOCALE_KEY, QVariant(d_func()->locale));
    appSettings.setValue(GW_REMEMBER_FILE_HISTORY_KEY, QVariant(d_func()->fileHistoryEnabled));
    appSettings.setValue(GW_SPACES_FOR_TABS_KEY, QVariant(d_func()->insertSpacesForTabsEnabled));
    appSettings.setValue(GW_TAB_WIDTH_KEY, QVariant(d_func()->tabWidth));
    appSettings.setValue(GW_THEME_KEY, QVariant(d_func()->themeName));
    appSettings.setValue(GW_DARK_MODE_KEY, QVariant(d_func()->darkModeEnabled));
    appSettings.setValue(GW_UNDERLINE_ITALICS_KEY, QVariant(d_func()->useUnderlineForEmphasis));

    appSettings.sync();
}

QString AppSettings::themeDirectoryPath() const
{
    return d_func()->themeDirectoryPath;
}

QString AppSettings::dictionaryPath() const
{
    return d_func()->dictionaryPath;
}

QString AppSettings::translationsPath() const
{
    return d_func()->translationsPath;
}

bool AppSettings::autoSaveEnabled() const
{
    return d_func()->autoSaveEnabled;
}

void AppSettings::setAutoSaveEnabled(bool enabled)
{
    d_func()->autoSaveEnabled = enabled;
    emit autoSaveChanged(enabled);
}

bool AppSettings::backupFileEnabled() const
{
    return d_func()->backupFileEnabled;
}

void AppSettings::setBackupFileEnabled(bool enabled)
{
    d_func()->backupFileEnabled = enabled;
    emit backupFileChanged(enabled);
}

QFont AppSettings::font() const
{
    return d_func()->font;
}

void AppSettings::setFont(const QFont &font)
{
    d_func()->font = font;
}

int AppSettings::tabWidth() const
{
    return d_func()->tabWidth;
}

void AppSettings::setTabWidth(int width)
{
    if ((d_func()->tabWidth >= MIN_TAB_WIDTH) && (d_func()->tabWidth <= MAX_TAB_WIDTH)) {
        d_func()->tabWidth = width;
        emit tabWidthChanged(width);
    }
}

bool AppSettings::insertSpacesForTabsEnabled() const
{
    return d_func()->insertSpacesForTabsEnabled;
}

void AppSettings::setInsertSpacesForTabsEnabled(bool enabled)
{
    d_func()->insertSpacesForTabsEnabled = enabled;
    emit insertSpacesForTabsChanged(enabled);
}

bool AppSettings::useUnderlineForEmphasis() const
{
    return d_func()->useUnderlineForEmphasis;
}

void AppSettings::setUseUnderlineForEmphasis(bool enabled)
{
    d_func()->useUnderlineForEmphasis = enabled;
    emit useUnderlineForEmphasisChanged(enabled);
}

bool AppSettings::largeHeadingSizesEnabled() const
{
    return d_func()->largeHeadingSizesEnabled;
}

void AppSettings::setLargeHeadingSizesEnabled(bool enabled)
{
    d_func()->largeHeadingSizesEnabled = enabled;
    emit largeHeadingSizesChanged(enabled);
}

bool AppSettings::autoMatchEnabled() const
{
    return d_func()->autoMatchEnabled;
}

void AppSettings::setAutoMatchEnabled(bool enabled)
{
    d_func()->autoMatchEnabled = enabled;
    emit autoMatchChanged(enabled);
}

bool AppSettings::autoMatchCharEnabled(const QChar openingCharacter) const
{
    return d_func()->autoMatchedCharFilter.contains(openingCharacter);
}

void AppSettings::setAutoMatchCharEnabled(const QChar openingCharacter, bool enabled)
{
    switch (openingCharacter.toLatin1()) {
    case '\"':
    case '\'':
    case '(':
    case '[':
    case '{':
    case '*':
    case '_':
    case '`':
    case '<':
        if (enabled) {
            if (!d_func()->autoMatchedCharFilter.contains(openingCharacter)) {
                d_func()->autoMatchedCharFilter.append(openingCharacter);
            }
        } else {
            d_func()->autoMatchedCharFilter.remove(openingCharacter);
        }

        emit autoMatchCharChanged(openingCharacter, enabled);

        break;
    default:
        break;
    }
}

bool AppSettings::bulletPointCyclingEnabled() const
{
    return d_func()->bulletPointCyclingEnabled;
}

void AppSettings::setBulletPointCyclingEnabled(bool enabled)
{
    d_func()->bulletPointCyclingEnabled = enabled;
    emit bulletPointCyclingChanged(enabled);
}

FocusMode AppSettings::focusMode() const
{
    return d_func()->focusMode;
}

void AppSettings::setFocusMode(FocusMode focusMode)
{
    if ((focusMode >= FocusModeFirst) && (focusMode <= FocusModeLast)) {
        d_func()->focusMode = focusMode;
        emit focusModeChanged(focusMode);
    }
}

bool AppSettings::hideMenuBarInFullScreenEnabled() const
{
    return d_func()->hideMenuBarInFullScreenEnabled;
}

void AppSettings::setHideMenuBarInFullScreenEnabled(bool enabled)
{
    d_func()->hideMenuBarInFullScreenEnabled = enabled;
    emit hideMenuBarInFullScreenChanged(enabled);
}

bool AppSettings::fileHistoryEnabled() const
{
    return d_func()->fileHistoryEnabled;
}

void AppSettings::setFileHistoryEnabled(bool enabled)
{
    d_func()->fileHistoryEnabled = enabled;
    emit fileHistoryChanged(enabled);
}

bool AppSettings::displayTimeInFullScreenEnabled()
{
    return d_func()->displayTimeInFullScreenEnabled;
}

void AppSettings::setDisplayTimeInFullScreenEnabled(bool enabled)
{
    d_func()->displayTimeInFullScreenEnabled = enabled;
    emit displayTimeInFullScreenChanged(enabled);
}

QString AppSettings::themeName() const
{
    return d_func()->themeName;
}

void AppSettings::setThemeName(const QString &name)
{
    d_func()->themeName = name;
}

bool AppSettings::darkModeEnabled() const
{
    return d_func()->darkModeEnabled;
}

void AppSettings::setDarkModeEnabled(bool enabled)
{
    d_func()->darkModeEnabled = enabled;
}

QString AppSettings::dictionaryLanguage() const
{
    return d_func()->dictionaryLanguage;
}

void AppSettings::setDictionaryLanguage(const QString &language)
{
    d_func()->dictionaryLanguage = language;
    emit dictionaryLanguageChanged(language);
}

QString AppSettings::locale() const
{
    return d_func()->locale;
}

void AppSettings::setLocale(const QString &locale)
{
    d_func()->locale = locale;
}

bool AppSettings::liveSpellCheckEnabled() const
{
    return d_func()->liveSpellCheckEnabled;
}

void AppSettings::setLiveSpellCheckEnabled(bool enabled)
{
    d_func()->liveSpellCheckEnabled = enabled;
    emit liveSpellCheckChanged(enabled);
}

EditorWidth AppSettings::editorWidth() const
{
    return d_func()->editorWidth;
}

void AppSettings::setEditorWidth(EditorWidth editorWidth)
{
    if ((editorWidth >= EditorWidthFirst) && (editorWidth <= EditorWidthLast)) {
        d_func()->editorWidth = editorWidth;
        emit editorWidthChanged(editorWidth);
    }
}

InterfaceStyle AppSettings::interfaceStyle() const
{
    return d_func()->interfaceStyle;
}

void AppSettings::setInterfaceStyle(InterfaceStyle style)
{
    d_func()->interfaceStyle = style;
    emit interfaceStyleChanged(style);
}

bool AppSettings::italicizeBlockquotes() const
{
    return d_func()->italicizeBlockquotes;
}

void AppSettings::setItalicizeBlockquotes(bool enabled)
{
    d_func()->italicizeBlockquotes = enabled;
    emit italicizeBlockquotesChanged(enabled);
}

bool AppSettings::htmlPreviewVisible() const
{
    return d_func()->htmlPreviewVisible;
}

void AppSettings::setHtmlPreviewVisible(bool visible)
{
    d_func()->htmlPreviewVisible = visible;
}

bool AppSettings::sidebarVisible() const
{
    return d_func()->sidebarVisible;
}

void AppSettings::setSidebarVisible(bool visible)
{
    d_func()->sidebarVisible = visible;
}

Exporter *AppSettings::currentHtmlExporter() const
{
    return d_func()->currentHtmlExporter;
}

void AppSettings::setCurrentHtmlExporter(Exporter *exporter)
{
    d_func()->currentHtmlExporter = exporter;
    emit currentHtmlExporterChanged(exporter);
}

AppSettings::AppSettings()
    : d_ptr(new AppSettingsPrivate())
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

    if (portable.exists() && portable.isWritable()) {
        userDir = portable.absoluteFilePath();
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath
        (
            QSettings::IniFormat,
            QSettings::UserScope,
            userDir + "/settings"
        );

        d_func()->translationsPath = appDir + "/translations";
    } else {
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

        foreach (const QString &path, translationPaths) {
            if (QFile::exists(path)) {
                d_func()->translationsPath = path;
                break;
            }
        }
    }

    QDir themeDir(userDir + "/themes");

    if (!themeDir.exists()) {
        themeDir.mkpath(themeDir.path());
    }

    d_func()->themeDirectoryPath = themeDir.absolutePath();

    QDir dictionaryDir(userDir + "/dictionaries");

    if (!dictionaryDir.exists()) {
        dictionaryDir.mkpath(dictionaryDir.path());
    }

    d_func()->dictionaryPath = dictionaryDir.absolutePath();
    DictionaryManager::setPath(d_func()->dictionaryPath);

    QStringList dictdirs;
    dictdirs.append(DictionaryManager::path());

    dictionaryDir = QDir(appDir + "/dictionaries");

    if (dictionaryDir.exists()) {
        dictdirs.append(dictionaryDir.path());
    }

    QDir::setSearchPaths("dict", dictdirs);


    // End FocusWriter lift/mod

    // Set default font to be Roboto Mono (included in QRC resources)
    if
    (
        (QFontDatabase::addApplicationFont(":/resources/RobotoMono-Italic-VariableFont_wght.ttf") < 0)
        || (QFontDatabase::addApplicationFont(":/resources/RobotoMono-VariableFont_wght.ttf") < 0)
    ) {
        qWarning() << "Failed to load Roboto Mono font.";
        d_func()->defaultFont = QFont("");
        d_func()->defaultFont.setFixedPitch(true);
        d_func()->defaultFont.setStyleHint(QFont::Monospace);
    } else {
        d_func()->defaultFont = QFont("Roboto Mono");
    }

    // Last but not least, load some basic settings from the configuration file,
    // but only those that need special validation.  Things like window
    // dimensions and file history can be handled elsewhere.
    //
    QSettings appSettings;

    d_func()->autoSaveEnabled = appSettings.value(GW_AUTOSAVE_KEY, QVariant(true)).toBool();
    d_func()->backupFileEnabled = appSettings.value(GW_BACKUP_FILE_KEY, QVariant(true)).toBool();

    d_func()->font = d_func()->defaultFont;
    d_func()->font.fromString(appSettings.value(GW_FONT_KEY, QVariant(d_func()->defaultFont.toString())).toString());

    d_func()->tabWidth = appSettings.value(GW_TAB_WIDTH_KEY, QVariant(DEFAULT_TAB_WIDTH)).toInt();

    if ((d_func()->tabWidth < MIN_TAB_WIDTH) || (d_func()->tabWidth > MAX_TAB_WIDTH)) {
        d_func()->tabWidth = DEFAULT_TAB_WIDTH;
    }

    d_func()->insertSpacesForTabsEnabled = appSettings.value(GW_SPACES_FOR_TABS_KEY, QVariant(false)).toBool();
    d_func()->useUnderlineForEmphasis = appSettings.value(GW_UNDERLINE_ITALICS_KEY, QVariant(false)).toBool();
    d_func()->largeHeadingSizesEnabled = appSettings.value(GW_LARGE_HEADINGS_KEY, QVariant(true)).toBool();
    d_func()->autoMatchEnabled = appSettings.value(GW_AUTO_MATCH_KEY, QVariant(true)).toBool();
    d_func()->autoMatchedCharFilter = appSettings.value(GW_AUTO_MATCH_FILTER_KEY, QVariant("\"\'([{*_`<")).toString();
    d_func()->bulletPointCyclingEnabled = appSettings.value(GW_BULLET_CYCLING_KEY, QVariant(true)).toBool();
    d_func()->focusMode = (FocusMode) appSettings.value(GW_FOCUS_MODE_KEY, QVariant(FocusModeSentence)).toInt();

    if ((d_func()->focusMode < FocusModeFirst) || (d_func()->focusMode > FocusModeLast)) {
        d_func()->focusMode = FocusModeSentence;
    }

    d_func()->hideMenuBarInFullScreenEnabled = appSettings.value(GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY, QVariant(true)).toBool();
    d_func()->fileHistoryEnabled = appSettings.value(GW_REMEMBER_FILE_HISTORY_KEY, QVariant(true)).toBool();
    d_func()->displayTimeInFullScreenEnabled = appSettings.value(GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY, QVariant(true)).toBool();
    d_func()->themeName = appSettings.value(GW_THEME_KEY, QVariant("Classic Light")).toString();
    d_func()->darkModeEnabled = appSettings.value(GW_DARK_MODE_KEY, QVariant(true)).toBool();
    d_func()->dictionaryLanguage = appSettings.value(GW_DICTIONARY_KEY, QLocale().name()).toString();

    // Determine locale for dictionary language (for use in spell checking).
    QString language = DictionaryManager::instance().availableDictionary(d_func()->dictionaryLanguage);

    // If we have an available dictionary, then set the default dictionary language.
    if (!language.isNull() && !language.isEmpty()) {
        DictionaryManager::instance().setDefaultLanguage(language);
    }

    d_func()->locale = appSettings.value(GW_LOCALE_KEY, QLocale().name()).toString();
    d_func()->liveSpellCheckEnabled = appSettings.value(GW_LIVE_SPELL_CHECK_KEY, QVariant(true)).toBool();
    d_func()->editorWidth = (EditorWidth) appSettings.value(GW_EDITOR_WIDTH_KEY, QVariant(EditorWidthMedium)).toInt();
    d_func()->interfaceStyle = (InterfaceStyle) appSettings.value(GW_INTERFACE_STYLE_KEY, QVariant(InterfaceStyleRounded)).toInt();
    d_func()->italicizeBlockquotes = appSettings.value(GW_BLOCKQUOTE_STYLE_KEY, QVariant(false)).toBool();

    if ((d_func()->editorWidth < EditorWidthFirst) || (d_func()->editorWidth > EditorWidthLast)) {
        d_func()->editorWidth = EditorWidthMedium;
    }

    if ((d_func()->interfaceStyle < InterfaceStyleFirst) || (d_func()->interfaceStyle > InterfaceStyleLast)) {
        d_func()->interfaceStyle = InterfaceStyleRounded;
    }

    d_func()->sidebarVisible = appSettings.value(GW_SIDEBAR_OPEN_KEY, QVariant(true)).toBool();
    d_func()->htmlPreviewVisible = appSettings.value(GW_HTML_PREVIEW_OPEN_KEY, QVariant(true)).toBool();

    QString exporterName = appSettings.value(GW_LAST_USED_EXPORTER_KEY).toString();
    d_func()->currentHtmlExporter = ExporterFactory::instance()->exporterByName(exporterName);

    if (NULL == d_func()->currentHtmlExporter) {
        d_func()->currentHtmlExporter = ExporterFactory::instance()->htmlExporters().first();
    }
}
}
