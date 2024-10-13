/*
 * SPDX-FileCopyrightText: 2014-2024 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-FileCopyrightText: 2014 Aurélien Gâteau <agateau@kde.org>
 * SPDX-FileCopyrightText: 2015 Alex Merry <alex.merry@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
#include <QTranslator>

#include "../export/exporterfactory.h"

#include "appsettings.h"

namespace ghostwriter
{
namespace constants
{
constexpr auto GW_FAVORITE_STATISTIC_KEY{"Session/favoriteStatistic"};
constexpr auto GW_RESTORE_SESSION_KEY{"Session/restoreSession"};
constexpr auto GW_REMEMBER_FILE_HISTORY_KEY{"Session/rememberFileHistory"};
constexpr auto GW_AUTOSAVE_KEY{"Save/autoSave"};
constexpr auto GW_BACKUP_FILE_KEY{"Save/backupFile"};
constexpr auto GW_EDITOR_FONT_KEY{"Style/editorFont"};
constexpr auto GW_LARGE_HEADINGS_KEY{"Style/largeHeadings"};
constexpr auto GW_AUTO_MATCH_KEY{"Typing/autoMatchEnabled"};
constexpr auto GW_AUTO_MATCH_FILTER_KEY{"Typing/autoMatchFilter"};
constexpr auto GW_BULLET_CYCLING_KEY{"Typing/bulletPointCyclingEnabled"};
constexpr auto GW_UNDERLINE_ITALICS_KEY{"Style/underlineInsteadOfItalics"};
constexpr auto GW_FOCUS_MODE_KEY{"Style/focusMode"};
constexpr auto GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY{"Style/hideMenuBarInFullScreenEnabled"};
constexpr auto GW_THEME_KEY{"Style/theme"};
constexpr auto GW_DARK_MODE_KEY{"Style/darkModeEnabled"};
constexpr auto GW_EDITOR_WIDTH_KEY{"Style/editorWidth"};
constexpr auto GW_INTERFACE_STYLE_KEY{"Style/interfaceStyle"};
constexpr auto GW_BLOCKQUOTE_STYLE_KEY{"Style/blockquoteStyle"};
constexpr auto GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY{"Style/displayTimeInFullScreen"};
constexpr auto GW_UNBREAKABLE_SPACE{"style/showUnbreakableSpace"};
constexpr auto GW_TAB_WIDTH_KEY{"Tabs/tabWidth"};
constexpr auto GW_SPACES_FOR_TABS_KEY{"Tabs/insertSpacesForTabs"};
constexpr auto GW_LOCALE_KEY{"Application/locale"};
constexpr auto GW_LIVE_SPELL_CHECK_KEY{"Spelling/liveSpellCheck"};
constexpr auto GW_SIDEBAR_OPEN_KEY{"Window/sidebarOpen"};
constexpr auto GW_HTML_PREVIEW_OPEN_KEY{"Preview/htmlPreviewOpen"};
constexpr auto GW_LAST_USED_EXPORTER_KEY{"Preview/lastUsedExporter"};
constexpr auto GW_LAST_USED_EXPORTER_PARAMS_KEY{"Preview/lastUsedExporterParams"};
constexpr auto GW_PREVIEW_TEXT_FONT_KEY{"Preview/textFont"};
constexpr auto GW_PREVIEW_CODE_FONT_KEY{"Preview/codeFont"};
constexpr auto GW_BACKUP_LOCATION_KEY{"Backup/location"};
}

class AppSettingsPrivate
{
public:
    static AppSettings *instance;

    AppSettingsPrivate()
        : currentHtmlExporter(nullptr)
    {
        ;
    }

    ~AppSettingsPrivate()
    {
        ;
    }

    QString initialLocale() const;
    QString firstAvailableFont(const QStringList& fontList) const;

    bool autoMatchEnabled;
    bool autoSaveEnabled;
    bool backupFileEnabled;
    QString draftLocation;
    QString backupLocation;
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
    QString locale;
    QStringList translationList;
    QString themeDirectoryPath;
    QString themeName;
    bool darkModeEnabled;
    bool showUnbreakableSpaceEnabled;
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

    appSettings.setValue(constants::GW_AUTO_MATCH_FILTER_KEY, QVariant(d->autoMatchedCharFilter));
    appSettings.setValue(constants::GW_AUTO_MATCH_KEY, QVariant(d->autoMatchEnabled));
    appSettings.setValue(constants::GW_AUTOSAVE_KEY, QVariant(d->autoSaveEnabled));
    appSettings.setValue(constants::GW_BACKUP_FILE_KEY, QVariant(d->backupFileEnabled));
    appSettings.setValue(constants::GW_BULLET_CYCLING_KEY, QVariant(d->bulletPointCyclingEnabled));
    appSettings.setValue(constants::GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY, QVariant(d->displayTimeInFullScreenEnabled));
    appSettings.setValue(constants::GW_EDITOR_WIDTH_KEY, QVariant(d->editorWidth));
    appSettings.setValue(constants::GW_FOCUS_MODE_KEY, QVariant(d->focusMode));
    appSettings.setValue(constants::GW_EDITOR_FONT_KEY, QVariant(d->editorFont.toString()));
    appSettings.setValue(constants::GW_PREVIEW_TEXT_FONT_KEY, QVariant(d->previewTextFont.toString()));
    appSettings.setValue(constants::GW_PREVIEW_CODE_FONT_KEY, QVariant(d->previewCodeFont.toString()));
    appSettings.setValue(constants::GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY, QVariant(d->hideMenuBarInFullScreenEnabled));
    appSettings.setValue(constants::GW_INTERFACE_STYLE_KEY, QVariant(d->interfaceStyle));
    appSettings.setValue(constants::GW_BLOCKQUOTE_STYLE_KEY, QVariant(d->italicizeBlockquotes));
    appSettings.setValue(constants::GW_LARGE_HEADINGS_KEY, QVariant(d->largeHeadingSizesEnabled));
    appSettings.setValue(constants::GW_SIDEBAR_OPEN_KEY, QVariant(d->sidebarVisible));
    appSettings.setValue(constants::GW_HTML_PREVIEW_OPEN_KEY, QVariant(d->htmlPreviewVisible));
    appSettings.setValue(constants::GW_LAST_USED_EXPORTER_KEY, QVariant(d->currentHtmlExporter->name()));
    appSettings.setValue(constants::GW_LAST_USED_EXPORTER_PARAMS_KEY, QVariant(d->currentHtmlExporter->options()));
    appSettings.setValue(constants::GW_LIVE_SPELL_CHECK_KEY, QVariant(d->liveSpellCheckEnabled));
    appSettings.setValue(constants::GW_LOCALE_KEY, QVariant(d->locale));
    appSettings.setValue(constants::GW_RESTORE_SESSION_KEY, QVariant(d->restoreSessionEnabled));
    appSettings.setValue(constants::GW_FAVORITE_STATISTIC_KEY, QVariant(d->favoriteStatistic));
    appSettings.setValue(constants::GW_REMEMBER_FILE_HISTORY_KEY, QVariant(d->fileHistoryEnabled));
    appSettings.setValue(constants::GW_SPACES_FOR_TABS_KEY, QVariant(d->insertSpacesForTabsEnabled));
    appSettings.setValue(constants::GW_TAB_WIDTH_KEY, QVariant(d->tabWidth));
    appSettings.setValue(constants::GW_THEME_KEY, QVariant(d->themeName));
    appSettings.setValue(constants::GW_DARK_MODE_KEY, QVariant(d->darkModeEnabled));
    appSettings.setValue(constants::GW_UNDERLINE_ITALICS_KEY, QVariant(d->useUnderlineForEmphasis));
    appSettings.setValue(constants::GW_UNBREAKABLE_SPACE, QVariant(d->showUnbreakableSpaceEnabled));
    appSettings.setValue(constants::GW_BACKUP_LOCATION_KEY, QVariant(d->backupLocation));

    appSettings.sync();
}

QString AppSettings::themeDirectoryPath() const
{
    Q_D(const AppSettings);
    
    return d->themeDirectoryPath;
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

void AppSettings::setBackupLocation(QString backupLocation)
{
    Q_D(AppSettings);

    d->backupLocation = backupLocation;
    emit backupLocationChanged(backupLocation);
}

QString AppSettings::backupLocation() const
{
    Q_D(const AppSettings);

    return d->backupLocation;
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

QString AppSettings::locale() const
{
    Q_D(const AppSettings);
    
    return d->locale;
}

bool AppSettings::setLocale(const QString &locale)
{
    Q_D(AppSettings);

    // The following code is lifted from the autogenerated ECMQmLoader code.
    QString subPath = QStringLiteral("locale/") + locale
        + QStringLiteral("/LC_MESSAGES/ghostwriter_qt.qm");

    const QString fullPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, subPath);

    if (fullPath.isEmpty()) {
        return false;
    }

    QTranslator *translator = new QTranslator(qApp);

    if (!translator->load(fullPath)) {
        delete translator;
        return false;
    }

    static QTranslator *currentTranslator = nullptr;
    
    if (nullptr != currentTranslator) {
        qApp->removeTranslator(currentTranslator);
        delete currentTranslator;
    }

    if (!qApp->installTranslator(translator)) {
        delete translator;
        return false;
    }

    currentTranslator = translator;
    d->locale = locale;
    return true;
}

QStringList AppSettings::availableTranslations() const
{
    Q_D(const AppSettings);

    return d->translationList;
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

bool AppSettings::showUnbreakableSpaceEnabled()
{
    const Q_D(AppSettings);

    return d->showUnbreakableSpaceEnabled;
}

void AppSettings::setShowUnbreakableSpaceEnabled(bool enabled)
{
    Q_D(AppSettings);

    d->showUnbreakableSpaceEnabled = enabled;
    emit showUnbreakableSpaceEnabledChanged(d->showUnbreakableSpaceEnabled);
}

AppSettings::AppSettings()
    : d_ptr(new AppSettingsPrivate())
{
    Q_D(AppSettings);

    // Determine available translation locales.
    QStringList dataPaths =
        QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

    for (auto path : dataPaths) {
        QDir localeDir(path + QStringLiteral("/locale"));
        auto localeDirs = localeDir.entryList(QDir::Dirs, QDir::Name);

        for (auto locale : localeDirs) {
            QString translationFilePath(path
                + QStringLiteral("/locale/")
                + locale
                + QStringLiteral("/LC_MESSAGES/ghostwriter_qt.qm"));

            if (QFileInfo(translationFilePath).exists()) {
                d->translationList.append(locale);
            }
        }
    }

    d->draftLocation =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    
    d->themeDirectoryPath =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + "/themes";

#ifdef Q_OS_WIN32
        // On Windows, don't ever use the registry to store settings, for the
        // sake of cleanness, ability to load configuration files on other
        // machines, and also for the user's privacy.
        //
        QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    QDir themeDir(d->themeDirectoryPath);

    if (!themeDir.exists()) {
        themeDir.mkpath(themeDir.path());
    }

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

    d->autoSaveEnabled = appSettings.value(constants::GW_AUTOSAVE_KEY, QVariant(true)).toBool();
    d->backupFileEnabled = appSettings.value(constants::GW_BACKUP_FILE_KEY, QVariant(true)).toBool();
    d->editorFont.fromString(appSettings.value(constants::GW_EDITOR_FONT_KEY, QVariant(monospaceFont)).toString());
    d->previewTextFont.fromString(appSettings.value(constants::GW_PREVIEW_TEXT_FONT_KEY, QVariant(variableFont)).toString());
    d->previewCodeFont.fromString(appSettings.value(constants::GW_PREVIEW_CODE_FONT_KEY, QVariant(monospaceFont)).toString());
    d->tabWidth = appSettings.value(constants::GW_TAB_WIDTH_KEY, QVariant(DEFAULT_TAB_WIDTH)).toInt();

    QString defaultBackupLocation = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QDir::separator() + "backups" + QDir::separator();

    d->backupLocation = appSettings.value(constants::GW_BACKUP_LOCATION_KEY, defaultBackupLocation).toString();

    if (d->backupLocation.isEmpty()) {
        d->backupLocation = defaultBackupLocation;
    }

    QFileInfo backupDirInfo(d->backupLocation);
    d->backupLocation = backupDirInfo.absoluteFilePath();

    if (backupDirInfo.exists()) {
        if (!backupDirInfo.isDir()) {
            qCritical() << "Backup file location must be a directory:" << d->backupLocation;
            d->backupLocation = defaultBackupLocation;
        } else if (!backupDirInfo.isWritable()) {
            qCritical() << "Backup file location is not writeable:" << d->backupLocation;
            d->backupLocation = defaultBackupLocation;
        }
    } else {
        QDir backupDir(d->backupLocation);

        if (!backupDir.mkpath(d->backupLocation)) {
            qCritical() << "Could not create backup file directory:" << d->backupLocation;
            d->backupLocation = defaultBackupLocation;
        }
    }

    qInfo() << "Backup files will be stored in" << d->backupLocation;

    if ((d->tabWidth < MIN_TAB_WIDTH) || (d->tabWidth > MAX_TAB_WIDTH)) {
        d->tabWidth = DEFAULT_TAB_WIDTH;
    }

    d->insertSpacesForTabsEnabled = appSettings.value(constants::GW_SPACES_FOR_TABS_KEY, QVariant(false)).toBool();
    d->useUnderlineForEmphasis = appSettings.value(constants::GW_UNDERLINE_ITALICS_KEY, QVariant(false)).toBool();
    d->largeHeadingSizesEnabled = appSettings.value(constants::GW_LARGE_HEADINGS_KEY, QVariant(true)).toBool();
    d->autoMatchEnabled = appSettings.value(constants::GW_AUTO_MATCH_KEY, QVariant(true)).toBool();
    d->autoMatchedCharFilter = appSettings.value(constants::GW_AUTO_MATCH_FILTER_KEY, QVariant("\"\'([{*_`<")).toString();
    d->bulletPointCyclingEnabled = appSettings.value(constants::GW_BULLET_CYCLING_KEY, QVariant(true)).toBool();
    d->focusMode = (FocusMode)appSettings.value(constants::GW_FOCUS_MODE_KEY, QVariant(FocusModeSentence)).toInt();

    if ((d->focusMode < FocusModeFirst) || (d->focusMode > FocusModeLast)) {
        d->focusMode = FocusModeSentence;
    }

    d->hideMenuBarInFullScreenEnabled = appSettings.value(constants::GW_HIDE_MENU_BAR_IN_FULL_SCREEN_KEY, QVariant(true)).toBool();
    d->showUnbreakableSpaceEnabled = appSettings.value(constants::GW_UNBREAKABLE_SPACE, QVariant(false)).toBool();
    d->favoriteStatistic = appSettings.value(constants::GW_FAVORITE_STATISTIC_KEY, QVariant(0)).toInt();
    d->restoreSessionEnabled = appSettings.value(constants::GW_RESTORE_SESSION_KEY, QVariant(true)).toBool();
    d->fileHistoryEnabled = appSettings.value(constants::GW_REMEMBER_FILE_HISTORY_KEY, QVariant(true)).toBool();
    d->displayTimeInFullScreenEnabled = appSettings.value(constants::GW_DISPLAY_TIME_IN_FULL_SCREEN_KEY, QVariant(true)).toBool();
    d->themeName = appSettings.value(constants::GW_THEME_KEY, QVariant("Classic Light")).toString();
    d->darkModeEnabled = appSettings.value(constants::GW_DARK_MODE_KEY, QVariant(true)).toBool();

    d->locale = appSettings.value(constants::GW_LOCALE_KEY, d->initialLocale()).toString();
    setLocale(d->locale);
    d->liveSpellCheckEnabled = appSettings.value(constants::GW_LIVE_SPELL_CHECK_KEY, QVariant(true)).toBool();
    d->editorWidth = (EditorWidth)appSettings.value(constants::GW_EDITOR_WIDTH_KEY, QVariant(EditorWidthMedium)).toInt();
    d->interfaceStyle = (InterfaceStyle)appSettings.value(constants::GW_INTERFACE_STYLE_KEY, QVariant(InterfaceStyleRounded)).toInt();
    d->italicizeBlockquotes = appSettings.value(constants::GW_BLOCKQUOTE_STYLE_KEY, QVariant(false)).toBool();

    if ((d->editorWidth < EditorWidthFirst) || (d->editorWidth > EditorWidthLast)) {
        d->editorWidth = EditorWidthMedium;
    }

    if ((d->interfaceStyle < InterfaceStyleFirst) || (d->interfaceStyle > InterfaceStyleLast)) {
        d->interfaceStyle = InterfaceStyleRounded;
    }

    d->sidebarVisible = appSettings.value(constants::GW_SIDEBAR_OPEN_KEY, QVariant(true)).toBool();
    d->htmlPreviewVisible = appSettings.value(constants::GW_HTML_PREVIEW_OPEN_KEY, QVariant(true)).toBool();

    QString exporterName = appSettings.value(constants::GW_LAST_USED_EXPORTER_KEY).toString();

    d->currentHtmlExporter = nullptr;

    if (!exporterName.isEmpty()) {
        d->currentHtmlExporter = ExporterFactory::instance()->exporterByName(exporterName);

        if (d->currentHtmlExporter) {
            auto lastExportOptions = appSettings.value(constants::GW_LAST_USED_EXPORTER_PARAMS_KEY).toString();

            if (!lastExportOptions.isEmpty()) {
                d->currentHtmlExporter->setOptions(lastExportOptions);
            }
        }
    }

    if (!d->currentHtmlExporter) {
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

QString AppSettingsPrivate::initialLocale() const
{
    QString languageCode = QLocale::system().name();
    QString languages = qEnvironmentVariable("LANGUAGE");

    if (!languages.isNull() && !languages.isEmpty()) {
        QStringList languageList = languages.split(':');

        if (!languageList.isEmpty()) {
            languageCode = languageList.first();
        }
    }

    return languageCode;
}
}
