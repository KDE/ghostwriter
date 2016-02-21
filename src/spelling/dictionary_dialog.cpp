/***********************************************************************
 *
 * Copyright (C) 2013 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) 2014-2016 wereturtle
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

#include "dictionary_dialog.h"

#include "dictionary_manager.h"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QDialog>

//-----------------------------------------------------------------------------

DictionaryDialog::DictionaryDialog(const QString& currentLanguage, QWidget* parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle(tr("Set Dictionary"));

	m_languages = new QListWidget(this);
    QStringList languages = DictionaryManager::instance().availableDictionaries();
	foreach (const QString& language, languages) {
        QListWidgetItem* item = new QListWidgetItem(languageName(language), m_languages);
		item->setData(Qt::UserRole, language);
        if (language == currentLanguage) {
			m_languages->setCurrentItem(item);
		}
	}
	m_languages->sortItems(Qt::AscendingOrder);
    selectedLanguage = currentLanguage;

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_languages, 1);
	layout->addSpacing(layout->margin());
	layout->addWidget(buttons);
}

//-----------------------------------------------------------------------------

void DictionaryDialog::accept()
{
	if (m_languages->count() > 0) {
        selectedLanguage = m_languages->currentItem()->data(Qt::UserRole).toString();
	}
	QDialog::accept();
}

QString DictionaryDialog::getLanguage() const
{
    return selectedLanguage;
}

//-----------------------------------------------------------------------------

QString DictionaryDialog::languageName(const QString& language)
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
