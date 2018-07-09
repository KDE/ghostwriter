/***********************************************************************
 *
 * Copyright (C) 2016 wereturtle
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

#ifndef LOCALEDIALOG_H
#define LOCALEDIALOG_H

#include <QDialog>

class QComboBox;

/**
 * Displays a dialog with supported languages for the applications
 * from which the user may choose.
 */
class LocaleDialog : public QDialog
{
    Q_OBJECT

    public:
        /**
         * Constructor.  Parameters are the initially selected locale string
         * (in language_country or language form, i.e, en_US, or en) and the
         * translations directory path where the application's *.qm translation
         * files are stored.
         */
        explicit LocaleDialog
        (
            const QString& initialLocale,
            const QString& translationsPath,
            QWidget *parent = 0
        );

        /**
         * Destructor.
         */
        virtual ~LocaleDialog();

        /**
         * Returns the currently selected locale in the dialog box.
         */
        QString getSelectedLocale() const;

        /**
         * Convenience method to display this dialog.  Returns the locale
         * selected by the user, in language_country or language form, i.e.,
         * en_US or en.  If the user selects the Cancel button, ok will be
         * set to false.
         *
         * Parameters are the initially selected locale string
         * (in language_country or language form, i.e, en_US, or en) and the
         * translations directory path where the application's *.qm translation
         * files are stored.
         */
        static QString getLocale
        (
            bool* ok,
            const QString& initialLocale,
            const QString& translationsPath,
            QWidget* parent = 0
        );

    private:
        QComboBox* localeComboBox;

        /*
         * Gets a descriptive language/country string from the given
         * language code, which is in language_country or language form,
         * i.e, en_US, or en.
         */
        QString getLocaleDescription(const QString& languageCode);

};

#endif // LOCALEDIALOG_H
