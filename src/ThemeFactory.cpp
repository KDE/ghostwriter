/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
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

#include <QSettings>
#include <QDesktopServices>
#include <stdio.h>
#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QFileInfo>
#include <QVariant>
#include <QColor>
#include <QMap>
#include <QStack>
#include <QStringList>
#include <QObject>

#include "ThemeFactory.h"
#include "AppSettings.h"

#define GW_EDITOR_ASPECT "Editor/aspect"
#define GW_EDITOR_CORNERS "Editor/corners"
#define GW_EDITOR_BACKGROUND_COLOR "Editor/backgroundColor"
#define GW_EDITOR_BACKGROUND_OPACITY "Editor/backgroundOpacity"
#define GW_SPELLING_ERROR_COLOR "Editor/spellingErrorColor"
#define GW_DEFAULT_TEXT_COLOR "Markdown/defaultTextColor"
#define GW_MARKUP_COLOR "Markdown/markupColor"
#define GW_LINK_COLOR "Markdown/linkColor"
#define GW_IMAGE_ASPECT "Background/imageAspect"
#define GW_IMAGE_URL "Background/imageUrl"
#define GW_BACKGROUND_COLOR "Background/color"
#define GW_HUD_FOREGROUND_COLOR "HUD/foregroundColor"
#define GW_HUD_BACKGROUND_COLOR "HUD/backgroundColor"

const QString ThemeFactory::LIGHT_THEME_NAME = QString("Classic Light");
const QString ThemeFactory::DARK_THEME_NAME = QString("Classic Dark");

ThemeFactory* ThemeFactory::instance = NULL;

ThemeFactory* ThemeFactory::getInstance()
{
    if (NULL == instance)
    {
        instance = new ThemeFactory();
    }

    return instance;
}

ThemeFactory::~ThemeFactory()
{
    ;
}

void ThemeFactory::loadLightTheme(Theme& theme) const
{
    theme.name = LIGHT_THEME_NAME;
    theme.markupColorScheme = MarkdownColorScheme();

    theme.backgroundColor = QColor("#f7f7f7");
    theme.backgroundImageAspect = PictureAspectNone;
    theme.hudBackgroundColor = QColor("#252525");
    theme.hudForegroundColor = QColor("#e3e3e3");
    theme.editorAspect = EditorAspectStretch;
    theme.markupColorScheme.backgroundColor = QColor("#f7f7f7");
    theme.markupColorScheme.defaultTextColor = QColor("#252525");
    theme.markupColorScheme.markupColor = QColor("#ababab");
    theme.markupColorScheme.linkColor = QColor("#2d87c2");
    theme.markupColorScheme.spellingErrorColor = QColor("red");
}

void ThemeFactory::loadDarkTheme(Theme& theme) const
{
    theme.name = DARK_THEME_NAME;
    theme.markupColorScheme = MarkdownColorScheme();

    theme.backgroundColor = QColor("#252525");
    theme.backgroundImageAspect = PictureAspectNone;
    theme.hudBackgroundColor = QColor("#2f3c41");
    theme.hudForegroundColor = QColor("#e3e3e3");
    theme.editorAspect = EditorAspectStretch;
    theme.markupColorScheme.backgroundColor = QColor("#252525");
    theme.markupColorScheme.defaultTextColor = QColor("#b9b9b9");
    theme.markupColorScheme.markupColor = QColor("#686868");
    theme.markupColorScheme.linkColor = QColor("#50879e");
    theme.markupColorScheme.spellingErrorColor = QColor("red");
}

QStringList ThemeFactory::getAvailableThemes()
{
    return availableThemes;
}

void ThemeFactory::loadTheme(const QString& name, Theme& theme, QString& err) const
{
    err = QString();

    if (availableThemes.contains(name))
    {
        QString themeFilePath = themeDirectoryPath + QString("/") + name + QString("/theme.cfg");

        QFileInfo themeFileInfo(themeFilePath);

        if (!themeFileInfo.exists())
        {
            err = QObject::tr("The specified theme does not exist in the file system: ") + themeFilePath;
            return;
        }

        QSettings themeSettings(themeFilePath, QSettings::IniFormat);
        int editorBackgroundAlpha;
        int editorAspect;
        int backgroundImageAspect;
        int editorCorners;

        // Load and validate the theme contents.
        if
        (
            !extractIntSetting(themeSettings, GW_EDITOR_ASPECT, editorAspect, EditorAspectFirst, EditorAspectLast, err)
            || !extractColorSetting(themeSettings, GW_EDITOR_BACKGROUND_COLOR, theme.markupColorScheme.backgroundColor, err)
            || !extractIntSetting(themeSettings, GW_EDITOR_BACKGROUND_OPACITY, editorBackgroundAlpha, 0, 255, err)
            || !extractColorSetting(themeSettings, GW_SPELLING_ERROR_COLOR, theme.markupColorScheme.spellingErrorColor, err)
            || !extractIntSetting(themeSettings, GW_IMAGE_ASPECT, backgroundImageAspect, PictureAspectFirst, PictureAspectLast, err)
            || !extractColorSetting(themeSettings, GW_BACKGROUND_COLOR, theme.backgroundColor, err)
            || !extractColorSetting(themeSettings, GW_HUD_FOREGROUND_COLOR, theme.hudForegroundColor, err)
            || !extractColorSetting(themeSettings, GW_HUD_BACKGROUND_COLOR, theme.hudBackgroundColor, err)
            || !extractColorSetting(themeSettings, GW_DEFAULT_TEXT_COLOR, theme.markupColorScheme.defaultTextColor, err)
            || !extractColorSetting(themeSettings, GW_MARKUP_COLOR, theme.markupColorScheme.markupColor, err)
            || !extractColorSetting(themeSettings, GW_LINK_COLOR, theme.markupColorScheme.linkColor, err)
        )
        {
            // Return error.
            return;
        }

        theme.editorAspect = (EditorAspect) editorAspect;
        theme.backgroundImageAspect = (PictureAspect) backgroundImageAspect;

        // Check that if the editor aspect is set to Center, that the corners are set.
        if
        (
            (EditorAspectCenter == theme.editorAspect)
            && !extractIntSetting(themeSettings, GW_EDITOR_CORNERS, editorCorners, EditorCornersFirst, EditorCornersLast, err)
        )
        {
            // Default to rounded corners if not set.
            editorCorners = EditorCornersRounded;
        }

        theme.editorCorners = (EditorCorners) editorCorners;

        // Check if the picture aspect is set to be a background image (not None), that
        // the background image URL is set.
        //
        if
        (
            (PictureAspectNone != theme.backgroundImageAspect)
            && !extractStringSetting(themeSettings, GW_IMAGE_URL, theme.backgroundImageUrl, err)
        )
        {
            // Return error.
            return;
        }

        theme.name = name;
        theme.markupColorScheme.backgroundColor.setAlpha(editorBackgroundAlpha);

        QFileInfo imgFileInfo(theme.backgroundImageUrl);

        // If the background image file path is relative and located in the same directory
        // as the theme file, then fill out the full path for the the background image URL.
        //
        if (!theme.backgroundImageUrl.isNull() && !theme.backgroundImageUrl.isEmpty() && imgFileInfo.isRelative())
        {
            theme.backgroundImageUrl = getDirectoryForTheme(name).path() + QString("/") + theme.backgroundImageUrl;
        }

        // Return success.
        err = QString();
    }
    else
    {
        err = QObject::tr("The specified theme is not available.  Try restarting the application.  "
            "If problem persists, please file a bug report.");
    }
}

void ThemeFactory::deleteTheme(const QString& name, QString& err)
{
    err = QString();

    QDir themeDir = this->getDirectoryForTheme(name);

    if (themeDir.exists())
    {
        // Traverse the the directory structure depth-first, and slowly build a stack of
        // files to delete.  The reason we build a stack of these files is because
        // files in a directory must be deleted before their parent directory is
        // deleted.
        //
        QStack<QFileInfo> dirTraverseStack;
        QStack<QFileInfo> fileDeleteStack;

        dirTraverseStack.push(QFileInfo(themeDir.path()));
        fileDeleteStack.push(QFileInfo(themeDir.path()));

        // Traverse the directory structure.
        while (!dirTraverseStack.isEmpty())
        {
            QFileInfo fileInfo = dirTraverseStack.pop();

            if (fileInfo.isDir())
            {
                QDir dir(fileInfo.filePath());

                QFileInfoList fileList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

                // Add files in the current directory to the file deletion stack.
                for (QFileInfoList::iterator fileIter = fileList.begin(); fileIter != fileList.end(); fileIter++)
                {
                    fileDeleteStack.push(*fileIter);

                    // If file is a directory, add it to the traversal stack so that we
                    // visit it.
                    //
                    if (fileIter->isDir())
                    {
                        dirTraverseStack.push(*fileIter);
                    }
                }
            }
        }

        // Pop files from the file deletion stack and delete them.
        while (!fileDeleteStack.isEmpty())
        {
            bool result;
            QFileInfo fileInfo = fileDeleteStack.pop();

            if (fileInfo.isDir())
            {
                QDir dir(fileInfo.absoluteFilePath());
                result = dir.rmdir(fileInfo.absoluteFilePath());
            }
            else
            {
                result = QFile::remove(fileInfo.absoluteFilePath());
            }

            if (!result)
            {
                err = QObject::tr("Could not delete %1 from theme.  Please try setting the theme file permissions to be writeable.")
                    .arg(fileInfo.filePath());
                return;
            }
        }
    }

    // Finally, remove the theme from the available themes list.
    availableThemes.removeOne(name);
}

void ThemeFactory::saveTheme(const QString& name, Theme& theme, QString& err)
{
    err = QString();

    // Check if theme was renamed.  If so, rename the theme's directory.
    if (theme.name != name)
    {
        // Check for rename to default name.
        if ((LIGHT_THEME_NAME == theme.name) || (DARK_THEME_NAME == theme.name))
        {
            err = QObject::tr("'%1' already exists.  Please choose another name.").arg(theme.name);
            return;
        }

        QDir oldThemeDir = this->getDirectoryForTheme(name);
        QDir newThemeDir = this->getDirectoryForTheme(theme.name);

        if (newThemeDir.exists())
        {
            err = QObject::tr("'' theme already exists.  Please choose another name.").arg(theme.name);
            return;
        }

        if (oldThemeDir.exists())
        {
            QString backgroundImageUrl = theme.backgroundImageUrl;

            // If there's a background image, set the image URL to be the in the new theme directory to which we
            // are renaming the theme.
            //
            if ((PictureAspectNone != theme.backgroundImageAspect) && !theme.backgroundImageUrl.isNull() && !theme.backgroundImageUrl.isEmpty())
            {
                QFileInfo oldImgFileInfo(theme.backgroundImageUrl);

                if (oldImgFileInfo.dir() == oldThemeDir)
                {
                    backgroundImageUrl = this->getDirectoryForTheme(theme.name).path() + QString("/") + oldImgFileInfo.fileName();
                }
            }

            // Rename the theme directory.
            QDir themeLocDir(this->themeDirectoryPath);
            bool success = themeLocDir.rename(name, theme.name);

            if (!success)
            {
                err = QObject::tr("Failed to rename theme.  Please check file permissions.");
                return;
            }

            theme.backgroundImageUrl = backgroundImageUrl;

            // Now update the available themes list with the new theme name.
            for (int i = 0; i < availableThemes.size(); i++)
            {
                if (name == availableThemes[i])
                {
                    availableThemes[i] = theme.name;
                    qSort(availableThemes.begin(), availableThemes.end());
                    break;
                }
            }
        }
    }

    bool isNewTheme = false;

    if (!this->getDirectoryForTheme(theme.name).exists())
    {
        isNewTheme = true;
    }

    QString themeFilePath = themeDirectoryPath + QString("/") + theme.name + QString("/theme.cfg");

    // Load original theme settings so we can clean up the old background image later, if any.
    Theme oldTheme;
    bool oldThemeExists = true;
    this->loadTheme(theme.name, oldTheme, err);

    if (!err.isNull())
    {
        oldThemeExists = false;
        err = QString();
    }

    QSettings settings(themeFilePath, QSettings::IniFormat);

    if (!settings.isWritable())
    {
        err = QObject::tr("Theme is read-only.  Please try renaming the theme, or setting the theme file to be writable.");
        return;
    }

    settings.setValue(GW_EDITOR_ASPECT, QVariant((int) theme.editorAspect));
    settings.setValue(GW_EDITOR_CORNERS, QVariant((int) theme.editorCorners));
    settings.setValue(GW_EDITOR_BACKGROUND_COLOR, QVariant(theme.markupColorScheme.backgroundColor.name()));
    settings.setValue(GW_EDITOR_BACKGROUND_OPACITY, QVariant(theme.markupColorScheme.backgroundColor.alpha()));
    settings.setValue(GW_SPELLING_ERROR_COLOR, QVariant(theme.markupColorScheme.spellingErrorColor.name()));
    settings.setValue(GW_DEFAULT_TEXT_COLOR, QVariant(theme.markupColorScheme.defaultTextColor.name()));
    settings.setValue(GW_MARKUP_COLOR, QVariant(theme.markupColorScheme.markupColor.name()));
    settings.setValue(GW_LINK_COLOR, QVariant(theme.markupColorScheme.linkColor.name()));
    settings.setValue(GW_IMAGE_ASPECT, QVariant((int) theme.backgroundImageAspect));
    settings.setValue(GW_BACKGROUND_COLOR, QVariant(theme.backgroundColor.name()));
    settings.setValue(GW_HUD_FOREGROUND_COLOR, QVariant(theme.hudForegroundColor.name()));
    settings.setValue(GW_HUD_BACKGROUND_COLOR, QVariant(theme.hudBackgroundColor.name()));

    QDir themeDir = this->getDirectoryForTheme(theme.name);
    QFileInfo imgFileInfo(theme.backgroundImageUrl);

    // Delete any old background images.
    if (oldThemeExists && (PictureAspectNone != oldTheme.backgroundImageAspect))
    {
        QFileInfo oldImgFileInfo(oldTheme.backgroundImageUrl);

        if (oldImgFileInfo.exists() && (oldImgFileInfo.dir() == themeDir) && (imgFileInfo != oldImgFileInfo))
        {
            QFile oldImgFile(oldTheme.backgroundImageUrl);
            bool result = oldImgFile.remove();

            if (!result)
            {
                err = QObject::tr("Failed to remove old theme image.  Please check file permissions.");
                return;
            }
        }
    }

    // If there's a background image, copy the image to the theme's directory, and set the background image URL
    // to point to it.
    //
    if ((PictureAspectNone != theme.backgroundImageAspect) && !theme.backgroundImageUrl.isNull() && !theme.backgroundImageUrl.isEmpty())
    {
        QFileInfo imgCopyFileInfo(themeDir.path() + QString("/") + imgFileInfo.fileName());

        // Copy the image to the theme directory if it's not already there.
        if (themeDir != imgFileInfo.dir())
        {
            // If there's an image in the theme folder that has the same file name as the
            // source image that is being copied, then delete that image first.
            //
            if (imgCopyFileInfo.exists())
            {
                QFile imgCopyFile(imgCopyFileInfo.filePath());
                bool result = imgCopyFile.remove();

                if (!result)
                {
                    err = QObject::tr("The old theme image file could not be removed from the theme directory.  Please check file permissions.");
                    settings.setValue("Background/imageAspect", QVariant((int) PictureAspectNone));
                    return;
                }
            }

            // Now copy the new image file to the theme directory.
            QFile imgFile(theme.backgroundImageUrl);
            bool result = imgFile.copy(imgCopyFileInfo.filePath());

            if (!result)
            {
                err = QObject::tr("Theme image file could not be copied to the theme directory.  Please check file permissions.");
                settings.setValue(GW_IMAGE_ASPECT, QVariant((int) PictureAspectNone));
                return;
            }
            else
            {
                settings.setValue(GW_IMAGE_URL, QVariant(imgCopyFileInfo.fileName()));
            }
        }
        else
        {
            settings.setValue(GW_IMAGE_URL, QVariant(imgCopyFileInfo.fileName()));
        }
    }

    settings.sync();

    if (QSettings::NoError != settings.status())
    {
        err = QObject::tr("Theme could not be saved to disk.  Please check file permissions or try renaming the theme.");
        return;
    }

    if (isNewTheme)
    {
        availableThemes.append(theme.name);
        qSort(availableThemes.begin(), availableThemes.end());
    }
}

QDir ThemeFactory::getThemeDirectory() const
{
    return themeDirectory;
}

QDir ThemeFactory::getDirectoryForTheme(const QString& name) const
{
    QDir themeDir(this->themeDirectoryPath + QString("/") + name);

    return themeDir.path();
}

QString ThemeFactory::generateUntitledThemeName() const
{
    QString name = QObject::tr("Untitled 1");
    int count = 1;
    bool availableNameFound = false;

    while (!availableNameFound)
    {
        bool tryAgain = false;

        for (int i = 0; i < this->availableThemes.size(); i++)
        {
            if (name == this->availableThemes[i])
            {
                count++;
                name = QObject::tr("Untitled %1").arg(count);
                tryAgain = true;
                break;
            }
        }

        if (!tryAgain)
        {
            availableNameFound = true;
        }
    }

    return name;
}

ThemeFactory::ThemeFactory()
{
    themeDirectoryPath = AppSettings::getInstance()->getThemeDirectoryPath();
    themeDirectory = QDir(themeDirectoryPath);

    if (!themeDirectory.exists())
    {
        themeDirectory.mkpath(themeDirectory.path());
    }

    themeDirectory.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable | QDir::Writable);
    themeDirectory.setSorting(QDir::Name);

    QFileInfoList themeFiles = themeDirectory.entryInfoList();
    QMap<QString, QString> sortedThemes;

    for (int i = 0; i < themeFiles.size(); i++)
    {
        QString baseName = themeFiles[i].fileName();
        sortedThemes.insertMulti(baseName.toLower(), baseName);
    }

    availableThemes = sortedThemes.values();
}

bool ThemeFactory::extractColorSetting
(
    const QSettings& settings,
    const QString& key,
    QColor& value,
    QString& err
) const
{
    QVariant variantValue = settings.value(key);

    if (variantValue.isNull() || !variantValue.isValid() || !variantValue.canConvert(QVariant::Color))
    {
        err = QObject::tr("Invalid or missing value for %1 provided.").arg(key);
        return false;
    }

    value = variantValue.value<QColor>();
    return true;
}

bool ThemeFactory::extractIntSetting
(
    const QSettings& settings,
    const QString& key,
    int& value,
    int min,
    int max,
    QString& err
) const
{
    QVariant variantValue = settings.value(key);

    if (variantValue.isNull() || !variantValue.isValid() || !variantValue.canConvert(QVariant::Int))
    {
        err = QObject::tr("Invalid or missing value for %1 provided.").arg(key);
        return false;
    }

    value = variantValue.toInt();

    if ((value < min) || (value > max))
    {
        err = QObject::tr("Value for %1 is out of range.  Valid values are between %2 and %3, inclusive.");
        err = err.arg(key).arg(min).arg(max);
        return false;
    }

    return true;
}

bool ThemeFactory::extractStringSetting
(
    const QSettings& settings,
    const QString& key,
    QString& value,
    QString& err
) const
{
    QVariant variantValue = settings.value(key);

    if (variantValue.isNull() || !variantValue.isValid() || !variantValue.canConvert(QVariant::String))
    {
        err = QObject::tr("Invalid or missing value for %1 provided.").arg(key);
        return false;
    }

    value = variantValue.toString();
    return true;
}
