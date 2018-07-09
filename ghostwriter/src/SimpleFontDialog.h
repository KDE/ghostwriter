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

#ifndef SIMPLEFONTDIALOG_H
#define SIMPLEFONTDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QFont>

class QLineEdit;

/**
 * A simplified font dialog displaying only the font family and font size as
 * options for the user to choose.  This class will also render a preview
 * of the font.
 */
class SimpleFontDialog : public QDialog
{
    Q_OBJECT

    public:
        /**
         * Constructor.
         */
        SimpleFontDialog(QWidget* parent = 0);

        /**
         * Constructor.  Takes initial font to display as a parameter.
         */
        SimpleFontDialog(const QFont& initial, QWidget* parent = 0);

        /**
         * Destructor.
         */
        ~SimpleFontDialog();

        /**
         * Gets the currently selected font.  Call this method after exec()
         * returns.
         */
        QFont getSelectedFont() const;

        /**
         * Convenience method to create the dialog and extract the font in
         * one easy call.  Takes intial font to display as a parameter.
         */
        static QFont getFont
        (
            bool* ok,
            const QFont& initial,
            QWidget* parent = 0
        );

        /**
         * Convenience method to create the dialog and extract the font in
         * one easy call.
         */
        static QFont getFont(bool* ok, QWidget* parent = 0);

    private slots:
        void onFontFamilyChanged(const QFont& font);
        void onFontSizeChanged(const QString& sizeText);

    private:
        QFont font;
        QLineEdit* fontPreview;
};

#endif // SIMPLEFONTDIALOG_H
