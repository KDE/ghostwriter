/***********************************************************************
 *
 * Copyright (C) 2014-2021 wereturtle
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
#include <QStandardPaths>
#include <QString>
#include <QStringList>

#include "appsettings.h"
#include "dictionarymanager.h"
#include "exporterfactory.h"

#define GW_FAVORITE_STATISTIC_KEY "Session/favoriteStatistic"
#define GW_RESTORE_SESSION_KEY "Session/restoreSession"
#define GW_REMEMBER_FILE_HISTORY_KEY "Session/rememberFileHistory"
#define GW_AUTOSAVE_KEY "Save/autoSave"
#define GW_BACKUP_FILE_KEY "Save/backupFile"
#define GW_EDITOR_FONT_KEY "Style/editorFont"
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
#define GW_PREVIEW_TEXT_FONT_KEY "Preview/textFont"
#define GW_PREVIEW_CODE_FONT_KEY "Preview/codeFont"

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

    QString firstAvailableFont(const QStringList& fontList) const;

    bool autoMatchEnabled;
    bool autoSaveEnabled;
    bool backupFileEnabled;
    QString draftLocation;
    bool bulletPointCyclingEnabled;
    bool displayTimeInFullScreenEnabled;
    int favoriteStatistic;
    bool restoreSessionEnabled;
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
    QFont editorFont;
    QFont previewTextFont;
    QFont previewCodeFont;
    QString autoMatchedCharFilter;
    QString dictionaryLanguage;
    QString dictionaryPath;
    QString locale;
    QString themeDirectoryPath;
    QString themeName;
    bool darkModeEnabled;
    QString translationsPath;
};

AppSettings *AppSettingsPrivate::instance = nullptr;

AppSettings *AppSettings::instance()
{
    if (nullptr == AppSettingsPrivate::instance) {
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
    Q_D(AppSettings);

    QSettings appSettings;

    appSettings.setValue(GW_AUTO_MATCH_FILTER_KEY, QVariant(d->autoMatchedCharFilter));
    appSettings.setValue(GW_AUTO_MATCH_KEY, QVariant(d->autoMatchEnabled));
    appSettings.setValue(GW_AUTOSAVE_KEY, QVariant(d->autoSaveEnabled));
    appSettings.setValue(GW_BACKUP_FILE_KEY, QVariant(d->backupFileEnabled));
    appSettings.setValue(GW_BULLET_CYCLING_KEY, QVariant(d->bulletPointCyclingEnabled));
    appSettings.setValue(GW_DICTIONARY_KEY, QVariant(d->dictionaryLanguage));
    appSettings.setValue(GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY, QVariant(d->displayTimeInFullScreenEnabled));
    appSettings.setValue(GW_EDITOR_WIDTH_KEY, QVariant(d->editorWidth));
    appSettings.setValue(GW_FOCUS_MODE_KEY, QVariant(d->focusMode));
    appSettings.setValue(GW_EDITOR_FONT_KEY, QVariant(d->editorFont.toString()));
    appSettings.setValue(GW_PREVIEW_TEXT_FONT_KEY,     QVariant(d->previewTextFont.toString()));
    appSettings.setValue(GW_PREVIEW_CODE_FONT_KEY, QVariant(d->previewCodeFont.toString()));
    appSettings.setValue(GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY, QVariant(d->hideMenuBarInFullScreenEnabled));
    appSettings.setValue(GW_INTERFACE_STYLE_KEY, QVariant(d->interfaceStyle));
    appSettings.setValue(GW_BLOCKQUOTE_STYLE_KEY, QVariant(d->italicizeBlockquotes));
    appSettings.setValue(GW_LARGE_HEADINGS_KEY, QVariant(d->largeHeadingSizesEnabled));
    appSettings.setValue(GW_SIDEBAR_OPEN_KEY, QVariant(d->sidebarVisible));
    appSettings.setValue(GW_HTML_PREVIEW_OPEN_KEY, QVariant(d->htmlPreviewVisible));
    appSettings.setValue(GW_LAST_USED_EXPORTER_KEY, QVariant(d->currentHtmlExporter->name()));
    appSettings.setValue(GW_LIVE_SPELL_CHECK_KEY, QVariant(d->liveSpellCheckEnabled));
    appSettings.setValue(GW_LOCALE_KEY, QVariant(d->locale));
    appSettings.setValue(GW_RESTORE_SESSION_KEY, QVariant(d->restoreSessionEnabled));
    appSettings.setValue(GW_FAVORITE_STATISTIC_KEY, QVariant(d->favoriteStatistic));
    appSettings.setValue(GW_REMEMBER_FILE_HISTORY_KEY, QVariant(d->fileHistoryEnabled));
    appSettings.setValue(GW_SPACES_FOR_TABS_KEY, QVariant(d->insertSpacesForTabsEnabled));
    appSettings.setValue(GW_TAB_WIDTH_KEY, QVariant(d->tabWidth));
    appSettings.setValue(GW_THEME_KEY, QVariant(d->themeName));
    appSettings.setValue(GW_DARK_MODE_KEY, QVariant(d->darkModeEnabled));
    appSettings.setValue(GW_UNDERLINE_ITALICS_KEY, QVariant(d->useUnderlineForEmphasis));

    appSettings.sync();
}

QString AppSettings::themeDirectoryPath() const
{
    Q_D(const AppSettings);
    
    return d->themeDirectoryPath;
}

QString AppSettings::dictionaryPath() const
{
    Q_D(const AppSettings);
    
    return d->dictionaryPath;
}

QString AppSettings::translationsPath() const
{
    Q_D(const AppSettings);
    
    return d->translationsPath;
}

bool AppSettings::autoSaveEnabled() const
{
    Q_D(const AppSettings);
    
    return d->autoSaveEnabled;
}

void AppSettings::setAutoSaveEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->autoSaveEnabled = enabled;
    emit autoSaveChanged(enabled);
}

bool AppSettings::backupFileEnabled() const
{
    Q_D(const AppSettings);
    
    return d->backupFileEnabled;
}

void AppSettings::setBackupFileEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->backupFileEnabled = enabled;
    emit backupFileChanged(enabled);
}

QString AppSettings::draftLocation() const
{
    Q_D(const AppSettings);

    return d->draftLocation;
}

QFont AppSettings::editorFont() const
{
    Q_D(const AppSettings);
    
    return d->editorFont;
}

void AppSettings::setEditorFont(const QFont &font)
{
    Q_D(AppSettings);
    
    d->editorFont = font;
}

QFont AppSettings::previewTextFont() const
{
    Q_D(const AppSettings);
    
    return d->previewTextFont;
}

void AppSettings::setPreviewTextFont(const QFont &font)
{
    Q_D(AppSettings);
    
    d->previewTextFont = font;
    emit previewTextFontChanged(font);
}

QFont AppSettings::previewCodeFont() const
{
    Q_D(const AppSettings);
    
    return d->previewCodeFont;
}

void AppSettings::setPreviewCodeFont(const QFont &font)
{
    Q_D(AppSettings);
    
    d->previewCodeFont = font;
    emit previewCodeFontChanged(font);
}

int AppSettings::tabWidth() const
{
    Q_D(const AppSettings);
    
    return d->tabWidth;
}

void AppSettings::setTabWidth(int width)
{
    Q_D(AppSettings);
    
    if ((d->tabWidth >= MIN_TAB_WIDTH) && (d->tabWidth <= MAX_TAB_WIDTH)) {
        d->tabWidth = width;
        emit tabWidthChanged(width);
    }
}

bool AppSettings::insertSpacesForTabsEnabled() const
{
    Q_D(const AppSettings);
    
    return d->insertSpacesForTabsEnabled;
}

void AppSettings::setInsertSpacesForTabsEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->insertSpacesForTabsEnabled = enabled;
    emit insertSpacesForTabsChanged(enabled);
}

bool AppSettings::useUnderlineForEmphasis() const
{
    Q_D(const AppSettings);
    
    return d->useUnderlineForEmphasis;
}

void AppSettings::setUseUnderlineForEmphasis(bool enabled)
{
    Q_D(AppSettings);
    
    d->useUnderlineForEmphasis = enabled;
    emit useUnderlineForEmphasisChanged(enabled);
}

bool AppSettings::largeHeadingSizesEnabled() const
{
    Q_D(const AppSettings);
    
    return d->largeHeadingSizesEnabled;
}

void AppSettings::setLargeHeadingSizesEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->largeHeadingSizesEnabled = enabled;
    emit largeHeadingSizesChanged(enabled);
}

bool AppSettings::autoMatchEnabled() const
{
    Q_D(const AppSettings);
    
    return d->autoMatchEnabled;
}

void AppSettings::setAutoMatchEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->autoMatchEnabled = enabled;
    emit autoMatchChanged(enabled);
}

bool AppSettings::autoMatchCharEnabled(const QChar openingCharacter) const
{
    Q_D(const AppSettings);
    
    return d->autoMatchedCharFilter.contains(openingCharacter);
}

void AppSettings::setAutoMatchCharEnabled(const QChar openingCharacter, bool enabled)
{
    Q_D(AppSettings);
    
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
            if (!d->autoMatchedCharFilter.contains(openingCharacter)) {
                d->autoMatchedCharFilter.append(openingCharacter);
            }
        } else {
            d->autoMatchedCharFilter.remove(openingCharacter);
        }

        emit autoMatchCharChanged(openingCharacter, enabled);

        break;
    default:
        break;
    }
}

bool AppSettings::bulletPointCyclingEnabled() const
{
    Q_D(const AppSettings);
    
    return d->bulletPointCyclingEnabled;
}

void AppSettings::setBulletPointCyclingEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->bulletPointCyclingEnabled = enabled;
    emit bulletPointCyclingChanged(enabled);
}

FocusMode AppSettings::focusMode() const
{
    Q_D(const AppSettings);
    
    return d->focusMode;
}

void AppSettings::setFocusMode(FocusMode focusMode)
{
    Q_D(AppSettings);
    
    if ((focusMode >= FocusModeFirst) && (focusMode <= FocusModeLast)) {
        d->focusMode = focusMode;
        emit focusModeChanged(focusMode);
    }
}

bool AppSettings::hideMenuBarInFullScreenEnabled() const
{
    Q_D(const AppSettings);
    
    return d->hideMenuBarInFullScreenEnabled;
}

void AppSettings::setHideMenuBarInFullScreenEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->hideMenuBarInFullScreenEnabled = enabled;
    emit hideMenuBarInFullScreenChanged(enabled);
}

int AppSettings::favoriteStatistic() const
{
    Q_D(const AppSettings);

    return d->favoriteStatistic;
}

void AppSettings::setFavoriteStatistic(int value)
{
    Q_D(AppSettings);

    if (value >= 0) {
        d->favoriteStatistic = value;
    }
}

bool AppSettings::restoreSessionEnabled() const
{
    Q_D(const AppSettings);

    return d->restoreSessionEnabled;
}

void AppSettings::setRestoreSessionEnabled(bool enabled)
{
    Q_D(AppSettings);

    d->restoreSessionEnabled = enabled;
}

bool AppSettings::fileHistoryEnabled() const
{
    Q_D(const AppSettings);
    
    return d->fileHistoryEnabled;
}

void AppSettings::setFileHistoryEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->fileHistoryEnabled = enabled;
    emit fileHistoryChanged(enabled);
}

bool AppSettings::displayTimeInFullScreenEnabled()
{
    Q_D(AppSettings);
    
    return d->displayTimeInFullScreenEnabled;
}

void AppSettings::setDisplayTimeInFullScreenEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->displayTimeInFullScreenEnabled = enabled;
    emit displayTimeInFullScreenChanged(enabled);
}

QString AppSettings::themeName() const
{
    Q_D(const AppSettings);
    
    return d->themeName;
}

void AppSettings::setThemeName(const QString &name)
{
    Q_D(AppSettings);
    
    d->themeName = name;
}

bool AppSettings::darkModeEnabled() const
{
    Q_D(const AppSettings);
    
    return d->darkModeEnabled;
}

void AppSettings::setDarkModeEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->darkModeEnabled = enabled;
}

QString AppSettings::dictionaryLanguage() const
{
    Q_D(const AppSettings);
    
    return d->dictionaryLanguage;
}

void AppSettings::setDictionaryLanguage(const QString &language)
{
    Q_D(AppSettings);
    
    d->dictionaryLanguage = language;
    emit dictionaryLanguageChanged(language);
}

QString AppSettings::locale() const
{
    Q_D(const AppSettings);
    
    return d->locale;
}

void AppSettings::setLocale(const QString &locale)
{
    Q_D(AppSettings);
    
    d->locale = locale;
}

bool AppSettings::liveSpellCheckEnabled() const
{
    Q_D(const AppSettings);
    
    return d->liveSpellCheckEnabled;
}

void AppSettings::setLiveSpellCheckEnabled(bool enabled)
{
    Q_D(AppSettings);
    
    d->liveSpellCheckEnabled = enabled;
    emit liveSpellCheckChanged(enabled);
}

EditorWidth AppSettings::editorWidth() const
{
    Q_D(const AppSettings);
    
    return d->editorWidth;
}

void AppSettings::setEditorWidth(EditorWidth editorWidth)
{
    Q_D(AppSettings);
    
    if ((editorWidth >= EditorWidthFirst) && (editorWidth <= EditorWidthLast)) {
        d->editorWidth = editorWidth;
        emit editorWidthChanged(editorWidth);
    }
}

InterfaceStyle AppSettings::interfaceStyle() const
{
    Q_D(const AppSettings);
    
    return d->interfaceStyle;
}

void AppSettings::setInterfaceStyle(InterfaceStyle style)
{
    Q_D(AppSettings);
    
    d->interfaceStyle = style;
    emit interfaceStyleChanged(style);
}

bool AppSettings::italicizeBlockquotes() const
{
    Q_D(const AppSettings);
    
    return d->italicizeBlockquotes;
}

void AppSettings::setItalicizeBlockquotes(bool enabled)
{
    Q_D(AppSettings);
    
    d->italicizeBlockquotes = enabled;
    emit italicizeBlockquotesChanged(enabled);
}

bool AppSettings::htmlPreviewVisible() const
{
    Q_D(const AppSettings);
    
    return d->htmlPreviewVisible;
}

void AppSettings::setHtmlPreviewVisible(bool visible)
{
    Q_D(AppSettings);
    
    d->htmlPreviewVisible = visible;
}

bool AppSettings::sidebarVisible() const
{
    Q_D(const AppSettings);
    
    return d->sidebarVisible;
}

void AppSettings::setSidebarVisible(bool visible)
{
    Q_D(AppSettings);
    
    d->sidebarVisible = visible;
}

Exporter *AppSettings::currentHtmlExporter() const
{
    Q_D(const AppSettings);
    
    return d->currentHtmlExporter;
}

void AppSettings::setCurrentHtmlExporter(Exporter *exporter)
{
    Q_D(AppSettings);
    
    d->currentHtmlExporter = exporter;
    emit currentHtmlExporterChanged(exporter);
}

AppSettings::AppSettings()
    : d_ptr(new AppSettingsPrivate())
{
    Q_D(AppSettings);
    
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

    d->draftLocation =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    if (portable.exists() && portable.isWritable()) {
        userDir = portable.absoluteFilePath();
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath
        (
            QSettings::IniFormat,
            QSettings::UserScope,
            userDir + "/settings"
        );

        d->translationsPath = appDir + "/translations";

        d->draftLocation = appDir + "/drafts";

        QDir draftDir(d->draftLocation);

        if (!draftDir.exists()) {
            draftDir.mkpath(draftDir.path());
        }
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
                d->translationsPath = path;
                break;
            }
        }
    }

    d->themeDirectoryPath = userDir + "/themes";

    QDir themeDir(d->themeDirectoryPath);

    if (!themeDir.exists()) {
        themeDir.mkpath(themeDir.path());
    }

    d->dictionaryPath = userDir + "/dictionaries";

    QDir dictionaryDir(d->dictionaryPath);

    if (!dictionaryDir.exists()) {
        dictionaryDir.mkpath(dictionaryDir.path());
    }

    DictionaryManager::setPath(d->dictionaryPath);

    QStringList dictdirs;
    dictdirs.append(DictionaryManager::path());

    dictionaryDir = QDir(appDir + "/dictionaries");

    if (dictionaryDir.exists()) {
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
    QStringList preferredMonospaceFonts;

#ifdef Q_OS_MAC
    preferredMonospaceFonts 
        << "Menlo"
        << "DejaVu Sans Mono"
        << "Monaco";
#elif defined(Q_OS_LINUX)
    preferredMonospaceFonts
        << "Noto Sans Mono"
        << "DejaVu Sans Mono"
        << "monospace";
#elif defined(Q_OS_WIN32)
    preferredMonospaceFonts
        << "DejaVu Sans Mono"
        << "Consolas"
        << "Lucida Console";
#endif
    preferredMonospaceFonts
        << "Courier New"
        << "Courier";

    QString monospaceFont = QFont(d->firstAvailableFont(preferredMonospaceFonts), 12).toString();
    QString variableFont = QFont("Noto Serif", 12).toString();

    // Last but not least, load some basic settings from the configuration file,
    // but only those that need special validation.  Things like window
    // dimensions and file history can be handled elsewhere.
    //
    QSettings appSettings;

    d->autoSaveEnabled = appSettings.value(GW_AUTOSAVE_KEY, QVariant(true)).toBool();
    d->backupFileEnabled = appSettings.value(GW_BACKUP_FILE_KEY, QVariant(true)).toBool();
    d->editorFont.fromString(appSettings.value(GW_EDITOR_FONT_KEY, QVariant(monospaceFont)).toString());
    d->previewTextFont.fromString(appSettings.value(GW_PREVIEW_TEXT_FONT_KEY, QVariant(variableFont)).toString());
    d->previewCodeFont.fromString(appSettings.value(GW_PREVIEW_CODE_FONT_KEY, QVariant(monospaceFont)).toString());
    d->tabWidth = appSettings.value(GW_TAB_WIDTH_KEY, QVariant(DEFAULT_TAB_WIDTH)).toInt();

    if ((d->tabWidth < MIN_TAB_WIDTH) || (d->tabWidth > MAX_TAB_WIDTH)) {
        d->tabWidth = DEFAULT_TAB_WIDTH;
    }

    d->insertSpacesForTabsEnabled = appSettings.value(GW_SPACES_FOR_TABS_KEY, QVariant(false)).toBool();
    d->useUnderlineForEmphasis = appSettings.value(GW_UNDERLINE_ITALICS_KEY, QVariant(false)).toBool();
    d->largeHeadingSizesEnabled = appSettings.value(GW_LARGE_HEADINGS_KEY, QVariant(true)).toBool();
    d->autoMatchEnabled = appSettings.value(GW_AUTO_MATCH_KEY, QVariant(true)).toBool();
    d->autoMatchedCharFilter = appSettings.value(GW_AUTO_MATCH_FILTER_KEY, QVariant("\"\'([{*_`<")).toString();
    d->bulletPointCyclingEnabled = appSettings.value(GW_BULLET_CYCLING_KEY, QVariant(true)).toBool();
    d->focusMode = (FocusMode) appSettings.value(GW_FOCUS_MODE_KEY, QVariant(FocusModeSentence)).toInt();

    if ((d->focusMode < FocusModeFirst) || (d->focusMode > FocusModeLast)) {
        d->focusMode = FocusModeSentence;
    }

    d->hideMenuBarInFullScreenEnabled = appSettings.value(GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY, QVariant(true)).toBool();
    d->favoriteStatistic = appSettings.value(GW_FAVORITE_STATISTIC_KEY, QVariant(0)).toInt();
    d->restoreSessionEnabled = appSettings.value(GW_RESTORE_SESSION_KEY, QVariant(true)).toBool();
    d->fileHistoryEnabled = appSettings.value(GW_REMEMBER_FILE_HISTORY_KEY, QVariant(true)).toBool();
    d->displayTimeInFullScreenEnabled = appSettings.value(GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY, QVariant(true)).toBool();
    d->themeName = appSettings.value(GW_THEME_KEY, QVariant("Classic Light")).toString();
    d->darkModeEnabled = appSettings.value(GW_DARK_MODE_KEY, QVariant(true)).toBool();
    d->dictionaryLanguage = appSettings.value(GW_DICTIONARY_KEY, QLocale().name()).toString();

    // Determine locale for dictionary language (for use in spell checking).
    QString language = DictionaryManager::instance()->availableDictionary(d->dictionaryLanguage);

    // If we have an available dictionary, then set the default dictionary language.
    if (!language.isNull() && !language.isEmpty()) {
        DictionaryManager::instance()->setDefaultLanguage(language);
    }

    d->locale = appSettings.value(GW_LOCALE_KEY, QLocale().name()).toString();
    d->liveSpellCheckEnabled = appSettings.value(GW_LIVE_SPELL_CHECK_KEY, QVariant(true)).toBool();
    d->editorWidth = (EditorWidth) appSettings.value(GW_EDITOR_WIDTH_KEY, QVariant(EditorWidthMedium)).toInt();
    d->interfaceStyle = (InterfaceStyle) appSettings.value(GW_INTERFACE_STYLE_KEY, QVariant(InterfaceStyleRounded)).toInt();
    d->italicizeBlockquotes = appSettings.value(GW_BLOCKQUOTE_STYLE_KEY, QVariant(false)).toBool();

    if ((d->editorWidth < EditorWidthFirst) || (d->editorWidth > EditorWidthLast)) {
        d->editorWidth = EditorWidthMedium;
    }

    if ((d->interfaceStyle < InterfaceStyleFirst) || (d->interfaceStyle > InterfaceStyleLast)) {
        d->interfaceStyle = InterfaceStyleRounded;
    }

    d->sidebarVisible = appSettings.value(GW_SIDEBAR_OPEN_KEY, QVariant(true)).toBool();
    d->htmlPreviewVisible = appSettings.value(GW_HTML_PREVIEW_OPEN_KEY, QVariant(true)).toBool();

    QString exporterName = appSettings.value(GW_LAST_USED_EXPORTER_KEY).toString();
    d->currentHtmlExporter = ExporterFactory::instance()->exporterByName(exporterName);

    if (nullptr == d->currentHtmlExporter) {
        d->currentHtmlExporter = ExporterFactory::instance()->htmlExporters().first();
    }
}

QString AppSettingsPrivate::firstAvailableFont(const QStringList& fontList) const
{
    QFontDatabase fontDb;
    QStringList fontFamilies = fontDb.families();

    // Pick the first font in the list of preferredFonts that is installed
    // (i.e., in the font database) to use as the default font on the very
    // first start up of this program.
    //
    bool fontMatchFound = false;

    for (int i = 0; i < fontList.size(); i++)
    {
        fontMatchFound =
            std::binary_search
            (
                fontFamilies.begin(),
                fontFamilies.end(),
                fontList[i]
            );

        if (fontMatchFound)
        {
            return fontList[i];
        }
    }

    // This case should not really happen, since the Courier family is
    // cross-platform.  This is just a precaution.
    //
    qWarning() <<
        "No preferred fonts were found. Using system default...";
    QFont systemFont = QFont("");
    systemFont.setFixedPitch(true);
    systemFont.setStyleHint(QFont::Monospace);
    return systemFont.family();
}
}
