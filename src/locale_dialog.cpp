/***********************************************************************
 *
 * Copyright (C) 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#include "locale_dialog.h"

#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QLabel>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

QString LocaleDialog::m_current;
QString LocaleDialog::m_path;
QString LocaleDialog::m_appname;

//-----------------------------------------------------------------------------

LocaleDialog::LocaleDialog(QWidget* parent) :
	QDialog(parent, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
	QString title = parent ? parent->window()->windowTitle() : QString();
	setWindowTitle(!title.isEmpty() ? title : QCoreApplication::applicationName());

	QLabel* text = new QLabel(tr("Select application language:"), this);

	m_translations = new QComboBox(this);
	m_translations->addItem(tr("<System Language>"), "");
	QString translation;
	QStringList translations = findTranslations();
	foreach (translation, translations) {
		if (translation.startsWith("qt")) {
			continue;
		}
		translation.remove(m_appname);
		m_translations->addItem(languageName(translation), translation);
	}
	int index = qMax(0, m_translations->findData(m_current));
	m_translations->setCurrentIndex(index);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->addWidget(text);
	layout->addWidget(m_translations);
	layout->addWidget(buttons);
}

//-----------------------------------------------------------------------------

void LocaleDialog::loadTranslator(const QString& name, const QStringList& datadirs)
{
	m_appname = name;

	// Find translator path
	QStringList paths = datadirs;
	if (paths.isEmpty()) {
		QString appdir = QCoreApplication::applicationDirPath();
		paths.append(appdir);
		paths.append(appdir + "/../share/" + QCoreApplication::applicationName().toLower());
		paths.append(appdir + "/../Resources");
	}
	foreach (const QString& path, paths) {
		if (QFile::exists(path + "/translations/")) {
			m_path = path + "/translations/";
			break;
		}
	}

	// Find current locale
	m_current = QSettings().value("Locale/Language").toString();
	QString current = !m_current.isEmpty() ? m_current : QLocale::system().name();
	QStringList translations = findTranslations();
	if (!translations.contains(m_appname + current)) {
		current = current.left(2);
		if (!translations.contains(m_appname + current)) {
			current.clear();
		}
	}
	if (!current.isEmpty()) {
		QLocale::setDefault(current);
	} else {
		current = "en";
	}

	// Load translators
	static QTranslator qt_translator;
	if (translations.contains("qt_" + current) || translations.contains("qt_" + current.left(2))) {
		qt_translator.load("qt_" + current, m_path);
	} else {
		qt_translator.load("qt_" + current, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	}
	QCoreApplication::installTranslator(&qt_translator);

	static QTranslator translator;
	translator.load(m_appname + current, m_path);
	QCoreApplication::installTranslator(&translator);

#if (QT_VERSION >= (QT_VERSION_CHECK(5,0,0)))
	// Work around bug in Qt 5 where text direction is not loaded
	QApplication::setLayoutDirection(QLocale(current).textDirection());
#endif
}

//-----------------------------------------------------------------------------

QString LocaleDialog::languageName(const QString& language)
{
	QString lang_code = language.left(5);
	QLocale locale(lang_code);
	QString name;
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
	if (lang_code.length() > 2) {
		if (locale.name() == lang_code) {
			name = locale.nativeLanguageName() + " (" + locale.nativeCountryName() + ")";
		} else {
			name = locale.nativeLanguageName() + " (" + language + ")";
		}
	} else {
		name = locale.nativeLanguageName();
	}
	if (locale.textDirection() == Qt::RightToLeft) {
		name.prepend(QChar(0x202b));
	}
#else
	if (lang_code.length() > 2) {
		if (locale.name() == lang_code) {
			name = QLocale::languageToString(locale.language()) + " (" + QLocale::countryToString(locale.country()) + ")";
		} else {
			name = QLocale::languageToString(locale.language()) + " (" + language + ")";
		}
	} else {
		name = QLocale::languageToString(locale.language());
	}
#endif
	return name;
}

//-----------------------------------------------------------------------------

QStringList LocaleDialog::findTranslations()
{
	QStringList result = QDir(m_path, "*.qm").entryList(QDir::Files);
	result.replaceInStrings(".qm", "");
	return result;
}

//-----------------------------------------------------------------------------

void LocaleDialog::accept()
{
	int current = m_translations->findData(m_current);
	if (current == m_translations->currentIndex()) {
		return reject();
	}
	QDialog::accept();

	m_current = m_translations->itemData(m_translations->currentIndex()).toString();
	QSettings().setValue("Locale/Language", m_current);
	QMessageBox::information(this, tr("Note"), tr("Please restart this application for the change in language to take effect."), QMessageBox::Ok);
}

//-----------------------------------------------------------------------------
