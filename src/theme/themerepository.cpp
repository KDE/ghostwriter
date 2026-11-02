/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QByteArray>
#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMetaEnum>
#include <QMultiMap>
#include <QSettings>
#include <QStack>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <editor/colorscheme.h>

#include "themerepository.h"


namespace ghostwriter
{
class ThemeRepositoryPrivate
{
public:

    static ThemeRepository *instance;

    ThemeRepositoryPrivate()
    {
        ;
    }

    ~ThemeRepositoryPrivate()
    {
        ;
    }

    QList<Theme> builtInThemes;
    QStringList customThemeNames;
    QString themeDirectoryPath;
    QDir themeDirectory;

    void loadClassicTheme();
    void loadPlainstractionTheme();
    void loadGitHubTheme();
    void loadCodebergTheme();
    bool loadColorsFromJsonObject(const QJsonObject &obj, ColorScheme &colors) const;
    bool loadColor(const QJsonObject &obj, const QString &key, QColor &color) const;
    bool isValidJsonObj(const QJsonValue &value) const;
    void insertColorsIntoJsonObject(QJsonObject &colorsObj, const ColorScheme &colors);
};

ThemeRepository::ThemeRepository(const QString &path)
    : d_ptr(new ThemeRepositoryPrivate())
{
    Q_D(ThemeRepository);
    
    d->themeDirectoryPath = path;
    d->themeDirectory = QDir(d->themeDirectoryPath);

    if (!d->themeDirectory.exists()) {
        d->themeDirectory.mkpath(d->themeDirectory.path());
    }

    d->themeDirectory.setFilter(QDir::Files | QDir::Readable | QDir::Writable);
    d->themeDirectory.setSorting(QDir::Name);
    d->themeDirectory.setNameFilters(QStringList("*.json"));

    QFileInfoList themeFiles = d->themeDirectory.entryInfoList();
    QMultiMap<QString, QString> sortedThemes;

    for (int i = 0; i < themeFiles.size(); i++) {
        QString baseName = themeFiles[i].baseName();
        sortedThemes.insert(baseName.toLower(), baseName);
    }

    d->customThemeNames = sortedThemes.values();

    // Set up built-in themes.
    d->loadClassicTheme();
    d->loadPlainstractionTheme();
    d->loadGitHubTheme();
    d->loadCodebergTheme();
}

ThemeRepository::~ThemeRepository()
{
    ;
}

QStringList ThemeRepository::availableThemes() const
{
    Q_D(const ThemeRepository);
    
    QStringList themeNames;

    for (Theme theme : d->builtInThemes) {
        themeNames.append(theme.name());
    }

    themeNames << d->customThemeNames;
    return themeNames;
}

Theme ThemeRepository::defaultTheme() const
{
    Q_D(const ThemeRepository);
    
    QString err;
    return d->builtInThemes[0];
}

Theme ThemeRepository::loadTheme(const QString &name, QString &err) const
{
    Q_D(const ThemeRepository);
    
    Theme theme = d->builtInThemes[0];
    err = QString();

    for (int i = 0; i < d->builtInThemes.size(); i++) {
        if (d->builtInThemes[i].name() == name) {
            return d->builtInThemes[i];
        }
    }

    if (d->customThemeNames.contains(name)) {
        QString themeFilePath = d->themeDirectoryPath + QString("/") +
                                name + ".json";

        QFileInfo themeFileInfo(themeFilePath);

        if (!themeFileInfo.exists() && !themeFileInfo.isFile()) {
            err = tr("The specified theme does not exist in the file system: %1")
                  .arg(themeFilePath);
            return theme;
        }

        QFile themeFile(themeFilePath);

        if (!themeFile.open(QIODevice::ReadOnly)) {
            err = tr("Could not open theme file for reading: %1")
                  .arg(themeFilePath);
            return theme;
        }

        QJsonDocument json = QJsonDocument::fromJson(themeFile.readAll());

        themeFile.close();

        if (json.isNull() || !json.isObject() || json.isEmpty()) {
            err = tr("Invalid theme format: %1")
                  .arg(themeFilePath);
            return d->builtInThemes[0];
        }

        QJsonObject themeObject = json.object();
        QJsonValue lightColorsObj = themeObject.value("light");
        QJsonValue darkColorsObj = themeObject.value("dark");
        Theme theme;

        if (d->isValidJsonObj(lightColorsObj) && d->isValidJsonObj(darkColorsObj)) {
            ColorScheme lightColors;
            ColorScheme darkColors;
            bool valid = true;

            valid &= d->loadColorsFromJsonObject(lightColorsObj.toObject(), lightColors);
            valid &= d->loadColorsFromJsonObject(darkColorsObj.toObject(), darkColors);

            if (!valid) {
                err = tr("Invalid or missing value(s) in %1").arg(themeFileInfo.completeBaseName());
            }

            theme = Theme(name, lightColors, darkColors, false);
        } else {
            ColorScheme colors;
            bool valid = d->loadColorsFromJsonObject(themeObject, colors);

            if (!valid) {
                err = tr("Invalid or missing value(s) in %1").arg(themeFileInfo.completeBaseName());
            }

            theme = Theme(name, colors);
        }

        return theme;
    } else {
        err = tr("The specified theme is not available.  Try restarting the application.  "
                 "If problem persists, please file a bug report.");
    }

    return theme;
}

void ThemeRepository::deleteTheme(const QString &name, QString &err)
{
    Q_D(ThemeRepository);
    
    err = QString();

    QFile themeFile(this->themeFilePath(name));

    if (themeFile.exists() && !themeFile.remove()) {
        err = tr("Could not delete theme at %1. Please try setting the file permissions to be writeable.")
              .arg(QFileInfo(themeFile).filePath());
        return;
    }

    // Finally, remove the theme from the available themes list.
    d->customThemeNames.removeOne(name);
}

void ThemeRepository::saveTheme(const QString &name, Theme &theme, QString &err)
{
    Q_D(ThemeRepository);
    
    err = QString();

    // Check if theme was renamed.  If so, rename the theme's file.
    if (theme.name() != name) {
        // Check for rename to default name.
        for (int i = 0; i < d->builtInThemes.size(); i++) {
            if (theme.name() == d->builtInThemes[i].name()) {
                err = tr("'%1' already exists.  Please choose another name.").arg(theme.name());
                return;
            }
        }

        QFile oldThemeFile(this->themeFilePath(name));
        QFile newThemeFile(this->themeFilePath(theme.name()));

        if (newThemeFile.exists()) {
            err = tr("'%1' theme already exists.  Please choose another name.").arg(theme.name());
            return;
        }

        if (oldThemeFile.exists()) {
            // Rename the theme file.
            if (!oldThemeFile.rename(this->themeFilePath(theme.name()))) {
                err = tr("Failed to rename theme.  Please check file permissions.");
                return;
            }

            // Now update the available themes list with the new theme name.
            for (int i = 0; i < d->customThemeNames.size(); i++) {
                if (name == d->customThemeNames[i]) {
                    d->customThemeNames[i] = theme.name();
                    std::sort(d->customThemeNames.begin(), d->customThemeNames.end());
                    break;
                }
            }
        }
    }

    QJsonDocument json;

    if (!theme.hasDarkColorScheme()) {
        QJsonObject colorsObj;
        d->insertColorsIntoJsonObject(colorsObj, theme.lightColorScheme());
        json.setObject(colorsObj);
    } else {
        QJsonObject lightColorsObj;
        QJsonObject darkColorsObj;
        d->insertColorsIntoJsonObject(lightColorsObj, theme.lightColorScheme());
        d->insertColorsIntoJsonObject(darkColorsObj, theme.darkColorScheme());

        QJsonObject colorsObj;
        colorsObj.insert("light", lightColorsObj);
        colorsObj.insert("dark", darkColorsObj);
        json.setObject(colorsObj);
    }

    QFile themeFile(this->themeFilePath(theme.name()));

    bool isNewTheme = !themeFile.exists();

    if (!themeFile.open(QIODevice::WriteOnly) || (themeFile.write(json.toJson()) < 0)) {
        err = tr("Theme could not be saved to disk.  Please check file permissions or try renaming the theme.");
        return;
    }

    themeFile.close();

    if (isNewTheme) {
        d->customThemeNames.append(theme.name());
        std::sort(d->customThemeNames.begin(), d->customThemeNames.end());
    }
}

QDir ThemeRepository::themeDirectory() const
{
    Q_D(const ThemeRepository);
    
    return d->themeDirectory;
}

QString ThemeRepository::themeFilePath(const QString &themeName) const
{
    Q_D(const ThemeRepository);
    
    return d->themeDirectoryPath + "/" + themeName + ".json";
}

QString ThemeRepository::generateUntitledThemeName() const
{
    Q_D(const ThemeRepository);
    
    QString name = tr("Untitled 1");
    int count = 1;
    bool availableNameFound = false;

    while (!availableNameFound) {
        bool tryAgain = false;

        for (int i = 0; i < d->customThemeNames.size(); i++) {
            if (name == d->customThemeNames[i]) {
                count++;
                name = tr("Untitled %1").arg(count);
                tryAgain = true;
                break;
            }
        }

        if (!tryAgain) {
            availableNameFound = true;
        }
    }

    return name;
}

void ThemeRepositoryPrivate::loadClassicTheme()
{
    ColorScheme lightColors;
    lightColors.background = "#eff0f1";
    lightColors.foreground = "#2d363a";
    lightColors.selection = "#93CEE9";
    lightColors.cursor = "#3DAEE9";
    lightColors.link = "#3DAEE9";
    lightColors.headingText = "#31363b";
    lightColors.headingMarkup = "#bdc3c7";
    lightColors.emphasisText = "#31363b";
    lightColors.emphasisMarkup = "#bdc3c7";
    lightColors.blockquoteText = "#31363b";
    lightColors.blockquoteMarkup = "#bdc3c7";
    lightColors.codeText = "#31363b";
    lightColors.codeMarkup = "#bdc3c7";
    lightColors.image = "#3DAEE9";
    lightColors.listMarkup = "#bdc3c7";
    lightColors.inlineHtml = "#bdc3c7";
    lightColors.divider = "#bdc3c7";
    lightColors.error = "#da4453";

    ColorScheme darkColors;
    darkColors.background = "#151719";
    darkColors.foreground = "#bdc3c7";
    darkColors.selection = "#074051";
    darkColors.cursor = "#3DAEE9";
    darkColors.link = "#3DAEE9";
    darkColors.headingText = "#bdc3c7";
    darkColors.headingMarkup = "#575b5f";
    darkColors.emphasisText = "#bdc3c7";
    darkColors.emphasisMarkup = "#575b5f";
    darkColors.blockquoteText = "#bdc3c7";
    darkColors.blockquoteMarkup = "#575b5f";
    darkColors.codeText = "#bdc3c7";
    darkColors.codeMarkup = "#575b5f";
    darkColors.image = "#3DAEE9";
    darkColors.listMarkup = "#575b5f";
    darkColors.inlineHtml = "#575b5f";
    darkColors.divider = "#575b5f";
    darkColors.error = "#da4453";

    Theme theme
    (
        "Classic",
        lightColors,
        darkColors,
        true
    );

    builtInThemes.append(theme);
}


void ThemeRepositoryPrivate::loadPlainstractionTheme()
{
    ColorScheme lightColors;
    lightColors.background = "#ffffff";
    lightColors.foreground = "#14191A";
    lightColors.selection = "#B8E3F0";
    lightColors.cursor = "#c23184";
    lightColors.link = "#c23184";
    lightColors.headingText = "#009bc8";
    lightColors.headingMarkup = "#c4e4f1";
    lightColors.emphasisText = "#009bc8";
    lightColors.emphasisMarkup = "#c4e4f1";
    lightColors.blockquoteText = "#009bc8";
    lightColors.blockquoteMarkup = "#c4e4f1";
    lightColors.codeText = "#009bc8";
    lightColors.codeMarkup = "#c4e4f1";
    lightColors.image = "#c23184";
    lightColors.listMarkup = "#009bc8";
    lightColors.inlineHtml = "#c4e4f1";
    lightColors.divider = "#c4e4f1";
    lightColors.error = "#c23184";

    ColorScheme darkColors;
    darkColors.background = "#1A1A1A";
    darkColors.foreground = "#D0D0D0";
    darkColors.selection = "#074051";
    darkColors.cursor = "#b31771";
    darkColors.link = "#b31771";
    darkColors.headingText = "#009bc8";
    darkColors.headingMarkup = "#074051";
    darkColors.emphasisText = "#009bc8";
    darkColors.emphasisMarkup = "#074051";
    darkColors.blockquoteText = "#009bc8";
    darkColors.blockquoteMarkup = "#074051";
    darkColors.codeText = "#009bc8";
    darkColors.codeMarkup = "#074051";
    darkColors.image = "#b31771";
    darkColors.listMarkup = "#009bc8";
    darkColors.inlineHtml = "#074051";
    darkColors.divider = "#074051";
    darkColors.error = "#b31771";

    Theme theme
    (
        "Plainstraction",
        lightColors,
        darkColors,
        true
    );


    builtInThemes.append(theme);
}

void ThemeRepositoryPrivate::loadGitHubTheme()
{
    ColorScheme lightColors;
    lightColors.background = "#ffffff";
    lightColors.foreground = "#1f2328";
    lightColors.selection = "#b6e3ff";
    lightColors.cursor = "#0969da";
    lightColors.link = "#0969da";
    lightColors.headingText = "#1f2328";
    lightColors.headingMarkup = "#818b98";
    lightColors.emphasisText = "#1f2328";
    lightColors.emphasisMarkup = "#818b98";
    lightColors.blockquoteText = "#59636e";
    lightColors.blockquoteMarkup = "#818b98";
    lightColors.codeText = "#0550ae";
    lightColors.codeMarkup = "#818b98";
    lightColors.image = "#0969da";
    lightColors.listMarkup = "#818b98";
    lightColors.inlineHtml = "#818b98";
    lightColors.divider = "#d1d9e0";
    lightColors.error = "#d1242f";

    ColorScheme darkColors;
    darkColors.background = "#0d1117";
    darkColors.foreground = "#e6edf3";
    darkColors.selection = "#1e4273";
    darkColors.cursor = "#2f81f7";
    darkColors.link = "#2f81f7";
    darkColors.headingText = "#e6edf3";
    darkColors.headingMarkup = "#6e7681";
    darkColors.emphasisText = "#e6edf3";
    darkColors.emphasisMarkup = "#6e7681";
    darkColors.blockquoteText = "#8b949e";
    darkColors.blockquoteMarkup = "#6e7681";
    darkColors.codeText = "#79c0ff";
    darkColors.codeMarkup = "#6e7681";
    darkColors.image = "#2f81f7";
    darkColors.listMarkup = "#6e7681";
    darkColors.inlineHtml = "#6e7681";
    darkColors.divider = "#30363d";
    darkColors.error = "#f85149";

    Theme theme
    (
        "GitHub",
        lightColors,
        darkColors,
        true
    );

    builtInThemes.append(theme);
}

void ThemeRepositoryPrivate::loadCodebergTheme()
{
    ColorScheme lightColors;
    lightColors.background = "#ffffff";
    lightColors.foreground = "#212529";
    lightColors.selection = "#b5ddff";
    lightColors.cursor = "#2185d0";
    lightColors.link = "#2185d0";
    lightColors.headingText = "#425f72";
    lightColors.headingMarkup = "#a9d5de";
    lightColors.emphasisText = "#425f72";
    lightColors.emphasisMarkup = "#a9d5de";
    lightColors.blockquoteText = "#425f72";
    lightColors.blockquoteMarkup = "#a9d5de";
    lightColors.codeText = "#2185d0";
    lightColors.codeMarkup = "#a9d5de";
    lightColors.image = "#2185d0";
    lightColors.listMarkup = "#2185d0";
    lightColors.inlineHtml = "#a9d5de";
    lightColors.divider = "#a9d5de";
    lightColors.error = "#db2828";

    ColorScheme darkColors;
    darkColors.background = "#2e323e";
    darkColors.foreground = "#dbe0e8";
    darkColors.selection = "#294f71";
    darkColors.cursor = "#4a9fe0";
    darkColors.link = "#4a9fe0";
    darkColors.headingText = "#a9d5de";
    darkColors.headingMarkup = "#5c6472";
    darkColors.emphasisText = "#a9d5de";
    darkColors.emphasisMarkup = "#5c6472";
    darkColors.blockquoteText = "#a9d5de";
    darkColors.blockquoteMarkup = "#5c6472";
    darkColors.codeText = "#4a9fe0";
    darkColors.codeMarkup = "#5c6472";
    darkColors.image = "#4a9fe0";
    darkColors.listMarkup = "#4a9fe0";
    darkColors.inlineHtml = "#5c6472";
    darkColors.divider = "#5c6472";
    darkColors.error = "#e05561";

    Theme theme
    (
        "Codeberg",
        lightColors,
        darkColors,
        true
    );

    builtInThemes.append(theme);
}

bool ThemeRepositoryPrivate::loadColorsFromJsonObject(const QJsonObject &obj, ColorScheme &colors) const
{
    bool valid = true;

    valid &= loadColor(obj, "foreground", colors.foreground);
    valid &= loadColor(obj, "background", colors.background);
    valid &= loadColor(obj, "selection", colors.selection);
    valid &= loadColor(obj, "cursor", colors.cursor);
    valid &= loadColor(obj, "markup", colors.emphasisMarkup);
    valid &= loadColor(obj, "accent", colors.listMarkup);
    valid &= loadColor(obj, "heading", colors.headingText);
    valid &= loadColor(obj, "emphasis", colors.emphasisText);
    valid &= loadColor(obj, "block", colors.blockquoteText);
    valid &= loadColor(obj, "link", colors.link);
    valid &= loadColor(obj, "error", colors.error);

    colors.headingMarkup = colors.emphasisMarkup;
    colors.inlineHtml = colors.emphasisMarkup;
    colors.codeMarkup = colors.emphasisMarkup;
    colors.blockquoteMarkup = colors.emphasisMarkup;
    colors.divider = colors.emphasisMarkup;
    colors.codeText = colors.blockquoteText;
    colors.image = colors.link;

    return valid;
}

bool ThemeRepositoryPrivate::loadColor(const QJsonObject &obj, const QString &key, QColor &color) const
{
    QJsonValue value = obj.value(key);

    if (value.isUndefined() || !value.isString() || !QColor::isValidColorName(value.toString())) {
        color = QColor::Invalid;
        return false;
    }

    color = value.toString();
    return true;
}

bool ThemeRepositoryPrivate::isValidJsonObj(const QJsonValue &value) const
{
    return (!value.isUndefined() && !value.isNull() && value.isObject());
}

void ThemeRepositoryPrivate::insertColorsIntoJsonObject(QJsonObject &colorsObj, const ColorScheme &colors)
{
    colorsObj.insert("foreground", colors.foreground.name());
    colorsObj.insert("background", colors.background.name());
    colorsObj.insert("selection", colors.selection.name());
    colorsObj.insert("cursor", colors.cursor.name());
    colorsObj.insert("cursor", colors.cursor.name());
    colorsObj.insert("markup", colors.emphasisMarkup.name());
    colorsObj.insert("accent", colors.listMarkup.name());
    colorsObj.insert("heading", colors.headingText.name());
    colorsObj.insert("emphasis", colors.emphasisText.name());
    colorsObj.insert("block", colors.blockquoteText.name());
    colorsObj.insert("link", colors.link.name());
    colorsObj.insert("error", colors.error.name());
}
} // namespace ghostwriter
