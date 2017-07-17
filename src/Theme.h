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

#ifndef THEME_H
#define THEME_H

#include <QString>
#include <QColor>

enum EditorAspect
{
    EditorAspectFirst,
    EditorAspectStretch = EditorAspectFirst,
    EditorAspectCenter,
    EditorAspectLast = EditorAspectCenter
};

enum EditorCorners
{
    EditorCornersFirst,
    EditorCornersRounded = EditorCornersFirst,
    EditorCornersSquare,
    EditorCornersLast = EditorCornersSquare,
};

enum TextWidth
{
    TextWidthNarrow,
    TextWidthMedium,
    TextWidthWide
};

enum PictureAspect
{
    PictureAspectFirst,
    PictureAspectNone = PictureAspectFirst,
    PictureAspectTile,
    PictureAspectCenter,
    PictureAspectStretch,
    PictureAspectScale,
    PictureAspectZoom,
    PictureAspectLast = PictureAspectZoom,
};

/**
 * Encapsulates a theme for customizing the look and feel of the application.
 */
class Theme
{
    public:
        Theme();
        Theme(const QString& name, bool builtIn = true);
        ~Theme();

        QString getName() const;
        void setName(const QString& value);

        bool isBuiltIn() const;
        void setBuiltIn(const bool builtIn);

        QColor getDefaultTextColor() const;
        void setDefaultTextColor(const QColor& value);

        QColor getMarkupColor() const;
        void setMarkupColor(const QColor& value);

        QColor getLinkColor() const;
        void setLinkColor(const QColor& value);

        QColor getHeadingColor() const;
        void setHeadingColor(const QColor& value);

        QColor getEmphasisColor() const;
        void setEmphasisColor(const QColor& value);

        QColor getBlockquoteColor() const;
        void setBlockquoteColor(const QColor& value);

        QColor getCodeColor() const;
        void setCodeColor(const QColor& value);

        QColor getSpellingErrorColor() const;
        void setSpellingErrorColor(const QColor& value);

        EditorAspect getEditorAspect() const;
        void setEditorAspect(const EditorAspect value);

        EditorCorners getEditorCorners() const;
        void setEditorCorners(const EditorCorners value);

        PictureAspect getBackgroundImageAspect() const;
        void setBackgroundImageAspect(const PictureAspect value);

        QString getBackgroundImageUrl() const;
        void setBackgroundImageUrl(const QString& value);

        QColor getBackgroundColor() const;
        void setBackgroundColor(const QColor& value);

        QColor getEditorBackgroundColor() const;
        void setEditorBackgroundColor(const QColor& value);

        QColor getHudForegroundColor() const;
        void setHudForegroundColor(const QColor& value);

        QColor getHudBackgroundColor() const;
        void setHudBackgroundColor(const QColor& value);

    private:
        QString name;

        // true if theme is built-in, false if custom (i.e., user-created).
        bool builtInFlag;

        QColor defaultTextColor;
        QColor editorBackgroundColor;
        QColor markupColor;
        QColor linkColor;
        QColor headingColor;
        QColor emphasisColor;
        QColor blockquoteColor;
        QColor codeColor;
        QColor spellingErrorColor;

        EditorAspect editorAspect;
        EditorCorners editorCorners;
        PictureAspect backgroundImageAspect;
        QString backgroundImageUrl;
        QColor backgroundColor;

        QColor hudForegroundColor;
        QColor hudBackgroundColor;
};

#endif
