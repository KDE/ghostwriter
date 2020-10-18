/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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
#include <QSettings>
#include <QStack>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "appsettings.h"
#include "colorscheme.h"
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
    bool loadColorsFromJsonObject(const QJsonObject &obj, ColorScheme &colors) const;
    bool loadColor(const QJsonObject &obj, const QString &key, QColor &color) const;
    bool isValidJsonObj(const QJsonValue &value) const;
    void insertColorsIntoJsonObject(QJsonObject &colorsObj, const ColorScheme &colors);
};

ThemeRepository *ThemeRepositoryPrivate::instance = NULL;

ThemeRepository *ThemeRepository::instance()
{
    if (NULL == ThemeRepositoryPrivate::instance) {
        ThemeRepositoryPrivate::instance = new ThemeRepository();
    }

    return ThemeRepositoryPrivate::instance;
}

ThemeRepository::ThemeRepository()
    : d_ptr(new ThemeRepositoryPrivate())
{
    d_func()->themeDirectoryPath = AppSettings::instance()->themeDirectoryPath();
    d_func()->themeDirectory = QDir(d_func()->themeDirectoryPath);

    if (!d_func()->themeDirectory.exists()) {
        d_func()->themeDirectory.mkpath(d_func()->themeDirectory.path());
    }

    d_func()->themeDirectory.setFilter(QDir::Files | QDir::Readable | QDir::Writable);
    d_func()->themeDirectory.setSorting(QDir::Name);
    d_func()->themeDirectory.setNameFilters(QStringList("*.json"));

    QFileInfoList themeFiles = d_func()->themeDirectory.entryInfoList();
    QMap<QString, QString> sortedThemes;

    for (int i = 0; i < themeFiles.size(); i++) {
        QString baseName = themeFiles[i].baseName();
        sortedThemes.insertMulti(baseName.toLower(), baseName);
    }

    d_func()->customThemeNames = sortedThemes.values();

    // Set up built-in themes.
    d_func()->loadClassicTheme();
    d_func()->loadPlainstractionTheme();
}

ThemeRepository::~ThemeRepository()
{
    ;
}

QStringList ThemeRepository::availableThemes() const
{
    QStringList themeNames;

    foreach (Theme theme, d_func()->builtInThemes) {
        themeNames.append(theme.name());
    }

    themeNames << d_func()->customThemeNames;
    return themeNames;
}

Theme ThemeRepository::defaultTheme() const
{
    QString err;
    return d_func()->builtInThemes[0];
}

Theme ThemeRepository::loadTheme(const QString &name, QString &err) const
{
    Theme theme = d_func()->builtInThemes[0];
    err = QString();

    for (int i = 0; i < d_func()->builtInThemes.size(); i++) {
        if (d_func()->builtInThemes[i].name() == name) {
            return d_func()->builtInThemes[i];
        }
    }

    if (d_func()->customThemeNames.contains(name)) {
        QString themeFilePath = d_func()->themeDirectoryPath + QString("/") +
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
            return d_func()->builtInThemes[0];
        }

        QJsonObject themeObject = json.object();
        QJsonValue lightColorsObj = themeObject.value("light");
        QJsonValue darkColorsObj = themeObject.value("dark");
        Theme theme;

        if (d_func()->isValidJsonObj(lightColorsObj) && d_func()->isValidJsonObj(darkColorsObj)) {
            ColorScheme lightColors;
            ColorScheme darkColors;
            bool valid = true;

            valid &= d_func()->loadColorsFromJsonObject(lightColorsObj.toObject(), lightColors);
            valid &= d_func()->loadColorsFromJsonObject(darkColorsObj.toObject(), darkColors);

            if (!valid) {
                err = tr("Invalid or missing value(s) in %1").arg(themeFileInfo.completeBaseName());
            }

            theme = Theme(name, lightColors, darkColors, false);
        } else {
            ColorScheme colors;
            bool valid = d_func()->loadColorsFromJsonObject(themeObject, colors);

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
    err = QString();

    QFile themeFile(this->themeFilePath(name));

    if (themeFile.exists() && !themeFile.remove()) {
        err = tr("Could not delete theme at %1. Please try setting the file permissions to be writeable.")
              .arg(QFileInfo(themeFile).filePath());
        return;
    }

    // Finally, remove the theme from the available themes list.
    d_func()->customThemeNames.removeOne(name);
}

void ThemeRepository::saveTheme(const QString &name, Theme &theme, QString &err)
{
    err = QString();

    // Check if theme was renamed.  If so, rename the theme's file.
    if (theme.name() != name) {
        // Check for rename to default name.
        for (int i = 0; i < d_func()->builtInThemes.size(); i++) {
            if (theme.name() == d_func()->builtInThemes[i].name()) {
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
            for (int i = 0; i < d_func()->customThemeNames.size(); i++) {
                if (name == d_func()->customThemeNames[i]) {
                    d_func()->customThemeNames[i] = theme.name();
                    std::sort(d_func()->customThemeNames.begin(), d_func()->customThemeNames.end());
                    break;
                }
            }
        }
    }

    QJsonDocument json;

    if (!theme.hasDarkColorScheme()) {
        QJsonObject colorsObj;
        d_func()->insertColorsIntoJsonObject(colorsObj, theme.lightColorScheme());
        json.setObject(colorsObj);
    } else {
        QJsonObject lightColorsObj;
        QJsonObject darkColorsObj;
        d_func()->insertColorsIntoJsonObject(lightColorsObj, theme.lightColorScheme());
        d_func()->insertColorsIntoJsonObject(darkColorsObj, theme.darkColorScheme());

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
        d_func()->customThemeNames.append(theme.name());
        std::sort(d_func()->customThemeNames.begin(), d_func()->customThemeNames.end());
    }
}

QDir ThemeRepository::themeDirectory() const
{
    return d_func()->themeDirectory;
}

QString ThemeRepository::themeFilePath(const QString &themeName) const
{
    return d_func()->themeDirectoryPath + "/" + themeName + ".json";
}

QString ThemeRepository::generateUntitledThemeName() const
{
    QString name = tr("Untitled 1");
    int count = 1;
    bool availableNameFound = false;

    while (!availableNameFound) {
        bool tryAgain = false;

        for (int i = 0; i < d_func()->customThemeNames.size(); i++) {
            if (name == d_func()->customThemeNames[i]) {
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
    lightColors.foreground = "#31363b";
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
    darkColors.selection = "#93CEE9";
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
    lightColors.foreground = "#363c42";
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
    darkColors.selection = "#1e262d";
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

    if (value.isUndefined() || !value.isString() || !QColor::isValidColor(value.toString())) {
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
