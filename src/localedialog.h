/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LOCALEDIALOG_H
#define LOCALEDIALOG_H

#include <QDialog>

class QComboBox;

namespace ghostwriter
{
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
        const QString &initialLocale,
        const QString &translationsPath,
        QWidget *parent = nullptr
    );

    /**
     * Destructor.
     */
    virtual ~LocaleDialog();

    /**
     * Returns the currently selected locale in the dialog box.
     */
    QString selectedLocale() const;

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
    static QString locale
    (
        bool *ok,
        const QString &initialLocale,
        const QString &translationsPath,
        QWidget *parent = nullptr
    );

private:
    QComboBox *localeComboBox;

    /*
    * Gets a descriptive language/country string from the given
    * language code, which is in language_country or language form,
    * i.e, en_US, or en.
    */
    QString localeDescription(const QString &languageCode);

};
} // namespace ghostwriter

#endif // LOCALEDIALOG_H
