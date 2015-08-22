/***********************************************************************
 *
 * Copyright (C) 2010, 2011, 2012, 2013 Graeme Gott <graeme@gottcode.org>
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

#ifndef FOCUSWRITER_LOCALE_DIALOG_H
#define FOCUSWRITER_LOCALE_DIALOG_H

#include <QDialog>
class QComboBox;

/**
 * Dialog to set application language.
 *
 * This class handles setting the application language when the application is
 * launched, as well as allowing the user to choose a different language for
 * future launches.
 */
class LocaleDialog : public QDialog
{
	Q_OBJECT

public:
	/**
	 * Construct a dialog to choose application language.
	 *
	 * @param parent the parent widget of the dialog
	 */
	LocaleDialog(QWidget* parent = 0);

	/**
	 * Load the stored language into the application; defaults to system language.
	 *
	 * @param appname application name to prepend to translation filenames
	 * @param datadirs locations to search for directory containing translations
	 */
	static void loadTranslator(const QString& appname, const QStringList& datadirs = QStringList());

	/**
	 * Fetch native language name for QLocale name.
	 *
	 * @param language QLocale name to look up
	 * @return translated language name
	 */
	static QString languageName(const QString& language);

public slots:
	/** Override parent function to store application language. */
	void accept();

private:
	/**
	 * Fetch list of application translations.
	 *
	 * @return list of QLocale names
	 */
	static QStringList findTranslations();

private:
	QComboBox* m_translations; /**< list of found translations */

	static QString m_current; /**< stored application language */
	static QString m_path; /**< location of translations; found in loadTranslator() */
	static QString m_appname; /**< application name passed to loadTranslator() */
};

#endif // FOCUSWRITER_LOCALE_DIALOG_H
