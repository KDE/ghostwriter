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

class SimpleFontDialog : public QDialog
{
    Q_OBJECT

    public:
        SimpleFontDialog(QWidget* parent = 0);
        SimpleFontDialog(const QFont& initial, QWidget* parent = 0);
        ~SimpleFontDialog();

        QFont getSelectedFont() const;

        static QFont getFont
        (
            bool* ok,
            const QFont& initial,
            QWidget* parent = 0
        );

        static QFont getFont(bool* ok, QWidget* parent = 0);

    private slots:
        void onFontFamilyChanged(const QString& familyText);
        void onFontSizeChanged(const QString& sizeText);

    private:
        QFont font;
        QLineEdit* fontPreview;
};

#endif // SIMPLEFONTDIALOG_H
