/***********************************************************************
 *
 * Copyright (C) 2015 wereturtle
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

#include "Theme.h"

Theme::Theme()
    : builtInFlag(false)
{

}

Theme::Theme(const QString& name, bool builtIn)
    : name(name), builtInFlag(builtIn)
{

}

Theme::~Theme()
{

}

QString Theme::getName() const
{
    return name;
}

void Theme::setName(const QString& value)
{
    name = value;
}

bool Theme::isBuiltIn() const
{
    return builtInFlag;
}

void Theme::setBuiltIn(const bool builtIn)
{
    builtInFlag = builtIn;
}

QColor Theme::getDefaultTextColor() const
{
    return defaultTextColor;
}

void Theme::setDefaultTextColor(const QColor& value)
{
    defaultTextColor = value;
}

QColor Theme::getMarkupColor() const
{
    return markupColor;
}

void Theme::setMarkupColor(const QColor& value)
{
    markupColor = value;
}

QColor Theme::getLinkColor() const
{
    return linkColor;
}

void Theme::setLinkColor(const QColor& value)
{
    linkColor = value;
}

QColor Theme::getHeadingColor() const
{
    return headingColor;
}

void Theme::setHeadingColor(const QColor& value)
{
    headingColor = value;
}

QColor Theme::getEmphasisColor() const
{
    return emphasisColor;
}

void Theme::setEmphasisColor(const QColor& value)
{
    emphasisColor = value;
}

QColor Theme::getBlockquoteColor() const
{
    return blockquoteColor;
}

void Theme::setBlockquoteColor(const QColor& value)
{
    blockquoteColor = value;
}

QColor Theme::getCodeColor() const
{
    return codeColor;
}

void Theme::setCodeColor(const QColor& value)
{
    codeColor = value;
}

QColor Theme::getSpellingErrorColor() const
{
    return spellingErrorColor;
}

void Theme::setSpellingErrorColor(const QColor& value)
{
    spellingErrorColor = value;
}

EditorAspect Theme::getEditorAspect() const
{
    return editorAspect;
}

void Theme::setEditorAspect(const EditorAspect value)
{
    if ((value < EditorAspectFirst) || (value > EditorAspectLast))
    {
        editorAspect = EditorAspectFirst;
        qCritical() << "Theme editor aspect value of " << value
                    << " is out of range. Defaulting to value of "
                    << editorAspect;
    }
    else
    {
        editorAspect = value;
    }
}

PictureAspect Theme::getBackgroundImageAspect() const
{
    return backgroundImageAspect;
}

void Theme::setBackgroundImageAspect(const PictureAspect value)
{
    if ((value < PictureAspectFirst) || (value > PictureAspectLast))
    {
        backgroundImageAspect = PictureAspectFirst;
        qCritical() << "Theme background image aspect value of " << value
                    << " is out of range. Defaulting to value of "
                    << backgroundImageAspect;
    }
    else
    {
        backgroundImageAspect = value;
    }
}

QString Theme::getBackgroundImageUrl() const
{
    return backgroundImageUrl;
}

void Theme::setBackgroundImageUrl(const QString& value)
{
    backgroundImageUrl = value;
}

QColor Theme::getBackgroundColor() const
{
    return backgroundColor;
}

void Theme::setBackgroundColor(const QColor& value)
{
    backgroundColor = value;
}

QColor Theme::getEditorBackgroundColor() const
{
    return editorBackgroundColor;
}

void Theme::setEditorBackgroundColor(const QColor& value)
{
    editorBackgroundColor = value;
}

QColor Theme::getHudForegroundColor() const
{
    return hudForegroundColor;
}

void Theme::setHudForegroundColor(const QColor& value)
{
    hudForegroundColor = value;
}

QColor Theme::getHudBackgroundColor() const
{
    return hudBackgroundColor;
}

void Theme::setHudBackgroundColor(const QColor& value)
{
    hudBackgroundColor = value;
}

