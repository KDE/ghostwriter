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

#ifndef THEMEEDITORDIALOG_H
#define THEMEEDITORDIALOG_H

#include <QObject>
#include <QDialog>

#include "Theme.h"

class ColorButton;
class ImageButton;
class QSlider;
class QComboBox;
class QLineEdit;

/**
 * Theme editor dialog which allows the user modify a theme for the
 * application's look and feel.
 */
class ThemeEditorDialog : public QDialog
{
    Q_OBJECT

    public:
        /**
         * Constructor.  Takes the theme to to be edited as a parameter.
         */
        ThemeEditorDialog(const Theme& theme, QWidget* parent = NULL);

        /**
         * Destructor.
         */
        ~ThemeEditorDialog();

    signals:
        /**
         * Emitted when the user completes and accepts (i.e., clicks on
         * Apply or OK button) the theme.  The updated theme is passed in as
         * a parameter.
         */
        void themeUpdated(const Theme& theme);

    private slots:
        void accept();
        void reject();
        void apply();

    private:
        QLineEdit* themeNameEdit;
        QComboBox* editorAspectComboBox;
        QComboBox* pictureAspectComboBox;
        ColorButton* textColorButton;
        ColorButton* markupColorButton;
        ColorButton* linkColorButton;
        ColorButton* emphasisColorButton;
        ColorButton* blockTextColorButton;
        ColorButton* spellcheckColorButton;
        ColorButton* editorBackgroundColorButton;
        ColorButton* hudForegroundColorButton;
        ColorButton* hudBackgroundColorButton;
        ImageButton* backgroundImageButton;
        ColorButton* backgroundColorButton;
        QSlider* editorOpacitySlider;

        Theme theme;
        QString oldThemeName;

        bool saveTheme();
};

#endif // THEMEEDITORDIALOG_H
