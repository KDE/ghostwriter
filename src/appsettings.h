/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QFont>
#include <QObject>
#include <QString>
#include <QStringList>

#include "exporter.h"
#include "markdowneditortypes.h"

namespace ghostwriter
{
/**
 * Loads and stores application settings via QSettings, particularly for
 * those settings that need special range checking.  Be sure to call
 * instance on application start up to set up settings file paths before
 * using QSettings directly for other settings not managed by this class.
 */
class AppSettingsPrivate;
class AppSettings : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AppSettings)

public:
    static const int MIN_TAB_WIDTH = 1;
    static const int MAX_TAB_WIDTH = 8;
    static const int DEFAULT_TAB_WIDTH = 4;

    static AppSettings *instance();
    ~AppSettings();

    void store();

    QString themeDirectoryPath() const;
    QString translationsPath() const;
    QString draftLocation() const;

    bool autoSaveEnabled() const;
    Q_SLOT void setAutoSaveEnabled(bool enabled);
    Q_SIGNAL void autoSaveChanged(bool enabled);

    bool backupFileEnabled() const;
    Q_SLOT void setBackupFileEnabled(bool enabled);
    Q_SIGNAL void backupFileChanged(bool enabled);

    QFont editorFont() const;
    void setEditorFont(const QFont &font);

    QFont previewTextFont() const;
    void setPreviewTextFont(const QFont &font);
    Q_SIGNAL void previewTextFontChanged(const QFont &font);

    QFont previewCodeFont() const;
    void setPreviewCodeFont(const QFont &font);
    Q_SIGNAL void previewCodeFontChanged(const QFont &font);

    int tabWidth() const;
    Q_SLOT void setTabWidth(int width);
    Q_SIGNAL void tabWidthChanged(int width);

    bool insertSpacesForTabsEnabled() const;
    Q_SLOT void setInsertSpacesForTabsEnabled(bool enabled);
    Q_SIGNAL void insertSpacesForTabsChanged(bool enabled);

    bool useUnderlineForEmphasis() const;
    Q_SLOT void setUseUnderlineForEmphasis(bool enabled);
    Q_SIGNAL void useUnderlineForEmphasisChanged(bool enabled);

    bool largeHeadingSizesEnabled() const;
    Q_SLOT void setLargeHeadingSizesEnabled(bool enabled);
    Q_SIGNAL void largeHeadingSizesChanged(bool enabled);

    bool autoMatchEnabled() const;
    Q_SLOT void setAutoMatchEnabled(bool enabled);
    Q_SIGNAL void autoMatchChanged(bool enabled);

    bool autoMatchCharEnabled(const QChar openingCharacter) const;
    Q_SLOT void setAutoMatchCharEnabled(const QChar openingCharacter, bool enabled);
    Q_SIGNAL void autoMatchCharChanged(const QChar openingChar, bool enabled);

    bool bulletPointCyclingEnabled() const;
    Q_SLOT void setBulletPointCyclingEnabled(bool enabled);
    Q_SIGNAL void bulletPointCyclingChanged(bool enabled);

    FocusMode focusMode() const;
    void setFocusMode(FocusMode focusMode);
    Q_SIGNAL void focusModeChanged(FocusMode focusMode);

    bool hideMenuBarInFullScreenEnabled() const;
    Q_SLOT void setHideMenuBarInFullScreenEnabled(bool enabled);
    Q_SIGNAL void hideMenuBarInFullScreenChanged(bool enabled);

    int favoriteStatistic() const;
    Q_SLOT void setFavoriteStatistic(int value);

    bool restoreSessionEnabled() const;
    Q_SLOT void setRestoreSessionEnabled(bool enabled);

    bool fileHistoryEnabled() const;
    Q_SLOT void setFileHistoryEnabled(bool enabled);
    Q_SIGNAL void fileHistoryChanged(bool enabled);

    bool displayTimeInFullScreenEnabled();
    Q_SLOT void setDisplayTimeInFullScreenEnabled(bool enabled);
    Q_SIGNAL void displayTimeInFullScreenChanged(bool enabled);

    QString themeName() const;
    void setThemeName(const QString &name);

    bool darkModeEnabled() const;
    void setDarkModeEnabled(bool enabled);

    QString locale() const;
    void setLocale(const QString &locale);

    Q_SIGNAL void spellCheckSettingsChanged() const;

    EditorWidth editorWidth() const;
    void setEditorWidth(EditorWidth editorWidth);
    Q_SIGNAL void editorWidthChanged(EditorWidth editorWidth);

    InterfaceStyle interfaceStyle() const;
    void setInterfaceStyle(InterfaceStyle style);
    Q_SIGNAL void interfaceStyleChanged(InterfaceStyle style);

    bool italicizeBlockquotes() const;
    void setItalicizeBlockquotes(bool enabled);
    Q_SIGNAL void italicizeBlockquotesChanged(bool enabled);

    bool htmlPreviewVisible() const;
    void setHtmlPreviewVisible(bool visible);

    bool sidebarVisible() const;
    void setSidebarVisible(bool visible);

    Exporter *currentHtmlExporter() const;
    Q_SLOT void setCurrentHtmlExporter(Exporter *exporter);
    Q_SIGNAL void currentHtmlExporterChanged(Exporter *exporter);

private:
    QScopedPointer<AppSettingsPrivate> d_ptr;

    AppSettings();
};
}

#endif // APPSETTINGS_H
