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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFont>

#include "MarkdownEditorTypes.h"
#include "MarkdownStyles.h"
#include "HudWindowTypes.h"
#include "Exporter.h"

/**
 * Loads and stores application settings via QSettings, particularly for
 * those settings that need special range checking.  Be sure to call
 * getInstance on application start up to set up settings file paths before
 * using QSettings directly for other settings not managed by this class.
 */
class AppSettings : public QObject
{
    Q_OBJECT

    public:
        static const int MIN_TAB_WIDTH = 1;
        static const int MAX_TAB_WIDTH = 8;
        static const int DEFAULT_TAB_WIDTH = 4;

        static AppSettings* getInstance();
        ~AppSettings();

        void store();

        QString getThemeDirectoryPath() const;
        QString getDictionaryPath() const;
        QString getTranslationsPath() const;

        bool getAutoSaveEnabled() const;
        Q_SLOT void setAutoSaveEnabled(bool enabled);
        Q_SIGNAL void autoSaveChanged(bool enabled);

        bool getBackupFileEnabled() const;
        Q_SLOT void setBackupFileEnabled(bool enabled);
        Q_SIGNAL void backupFileChanged(bool enabled);

        QFont getFont() const;
        void setFont(const QFont& font);

        int getTabWidth() const;
        Q_SLOT void setTabWidth(int width);
        Q_SIGNAL void tabWidthChanged(int width);

        bool getInsertSpacesForTabsEnabled() const;
        Q_SLOT void setInsertSpacesForTabsEnabled(bool enabled);
        Q_SIGNAL void insertSpacesForTabsChanged(bool enabled);

        bool getUseUnderlineForEmphasis() const;
        Q_SLOT void setUseUnderlineForEmphasis(bool enabled);
        Q_SIGNAL void useUnderlineForEmphasisChanged(bool enabled);

        bool getLargeHeadingSizesEnabled() const;
        Q_SLOT void setLargeHeadingSizesEnabled(bool enabled);
        Q_SIGNAL void largeHeadingSizesChanged(bool enabled);

        bool getAutoMatchEnabled() const;
        Q_SLOT void setAutoMatchEnabled(bool enabled);
        Q_SIGNAL void autoMatchChanged(bool enabled);

        bool getAutoMatchCharEnabled(const QChar openingCharacter) const;
        Q_SLOT void setAutoMatchCharEnabled(const QChar openingCharacter, bool enabled);
        Q_SIGNAL void autoMatchCharChanged(const QChar openingChar, bool enabled);

        bool getBulletPointCyclingEnabled() const;
        Q_SLOT void setBulletPointCyclingEnabled(bool enabled);
        Q_SIGNAL void bulletPointCyclingChanged(bool enabled);

        FocusMode getFocusMode() const;
        void setFocusMode(FocusMode focusMode);
        Q_SIGNAL void focusModeChanged(FocusMode focusMode);

        bool getHideMenuBarInFullScreenEnabled() const;
        Q_SLOT void setHideMenuBarInFullScreenEnabled(bool enabled);
        Q_SIGNAL void hideMenuBarInFullScreenChanged(bool enabled);

        bool getFileHistoryEnabled() const;
        Q_SLOT void setFileHistoryEnabled(bool enabled);
        Q_SIGNAL void fileHistoryChanged(bool enabled);

        bool getDisplayTimeInFullScreenEnabled();
        Q_SLOT void setDisplayTimeInFullScreenEnabled(bool enabled);
        Q_SIGNAL void displayTimeInFullScreenChanged(bool enabled);

        QString getThemeName() const;
        void setThemeName(const QString& name);

        QString getDictionaryLanguage() const;
        void setDictionaryLanguage(const QString& language);
        Q_SIGNAL void dictionaryLanguageChanged(const QString& language);

        QString getLocale() const;
        void setLocale(const QString& locale);

        bool getLiveSpellCheckEnabled() const;
        Q_SLOT void setLiveSpellCheckEnabled(bool enabled);
        Q_SIGNAL void liveSpellCheckChanged(bool enabled);

        EditorWidth getEditorWidth() const;
        void setEditorWidth(EditorWidth editorWidth);
        Q_SIGNAL void editorWidthChanged(EditorWidth editorWidth);

        InterfaceStyle getInterfaceStyle() const;
        void setInterfaceStyle(InterfaceStyle style);
        Q_SIGNAL void interfaceStyleChanged(InterfaceStyle style);

        BlockquoteStyle getBlockquoteStyle() const;
        void setBlockquoteStyle(BlockquoteStyle style);
        Q_SIGNAL void blockquoteStyleChanged(BlockquoteStyle style);

        HudWindowButtonLayout getHudButtonLayout() const;
        void setHudButtonLayout(HudWindowButtonLayout layout);
        Q_SIGNAL void hudButtonLayoutChanged(HudWindowButtonLayout layout);

        bool getAlternateHudRowColorsEnabled() const;
        Q_SLOT void setAlternateHudRowColorsEnabled(bool enabled);
        Q_SIGNAL void alternateHudRowColorsChanged(bool enabled);

        bool getDesktopCompositingEnabled() const;
        Q_SLOT void setDesktopCompositingEnabled(bool enabled);
        Q_SIGNAL void desktopCompositingChanged(bool enabled);

        int getHudOpacity() const;
        Q_SLOT void setHudOpacity(int value);
        Q_SIGNAL void hudOpacityChanged(int value);

        bool getHighlightLineBreaks() const;
        Q_SLOT void setHighlightLineBreaks(bool enabled);
        Q_SIGNAL void highlightLineBreaksChanged(bool enabled);

        bool getHtmlPreviewVisible() const;
        void setHtmlPreviewVisible(bool visible);

        QStringList getCustomCssFiles() const;
        void setCustomCssFiles(const QStringList& cssFilePathList);
        Q_SIGNAL void customCssFilesChanged(const QStringList& cssFilePathList);

        QString getCurrentCssFile() const;
        Q_SLOT void setCurrentCssFile(const QString& filePath);
        Q_SIGNAL void currentCssFileChanged(const QString& filePath);

        Exporter* getCurrentHtmlExporter() const;
        Q_SLOT void setCurrentHtmlExporter(Exporter* exporter);
        Q_SIGNAL void currentHtmlExporterChanged(Exporter* exporter);

    private:
        AppSettings();

        static AppSettings* instance;
        QString themeDirectoryPath;
        QString dictionaryPath;
        QString translationsPath;

        QFont defaultFont;
        bool autoSaveEnabled;
        bool backupFileEnabled;
        QFont font;
        int tabWidth;
        bool insertSpacesForTabsEnabled;
        bool useUnderlineForEmphasis;
        bool largeHeadingSizesEnabled;
        bool autoMatchEnabled;
        QString autoMatchedCharFilter;
        bool bulletPointCyclingEnabled;
        FocusMode focusMode;
        bool hideMenuBarInFullScreenEnabled;
        bool fileHistoryEnabled;
        bool displayTimeInFullScreenEnabled;
        QString themeName;
        QString dictionaryLanguage;
        QString locale;
        bool liveSpellCheckEnabled;
        EditorWidth editorWidth;
        InterfaceStyle interfaceStyle;
        BlockquoteStyle blockquoteStyle;
        HudWindowButtonLayout hudButtonLayout;
        bool alternateHudRowColorsEnabled;
        bool desktopCompositingEnabled;
        int hudOpacity;
        bool highlightLineBreaks;
        bool htmlPreviewVisible;
        QStringList customCssFiles;
        QString currentCssFile;
        Exporter* currentHtmlExporter;
};

#endif // APPSETTINGS_H
