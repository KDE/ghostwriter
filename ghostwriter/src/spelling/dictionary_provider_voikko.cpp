/***********************************************************************
 *
 * Copyright (C) 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#include "dictionary_provider_voikko.h"

#include "abstract_dictionary.h"
#include "dictionary_manager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLibrary>
#include <QStringList>
#include <QStringRef>

//-----------------------------------------------------------------------------

namespace
{
	const int VOIKKO_SPELL_OK = 1;
	const int VOIKKO_OPT_IGNORE_NUMBERS = 1;
	const int VOIKKO_OPT_IGNORE_UPPERCASE = 3;

	struct VoikkoHandle;

	typedef struct VoikkoHandle* (*VoikkoInitFunction)(const char** error, const char* langcode, const char* path);
	VoikkoInitFunction voikkoInit = 0;

	typedef void (*VoikkoTerminateFunction)(struct VoikkoHandle* handle);
	VoikkoTerminateFunction voikkoTerminate = 0;

	typedef int (*VoikkoSetBooleanOptionFunction)(struct VoikkoHandle * handle, int option, int value);
	VoikkoSetBooleanOptionFunction voikkoSetBooleanOption = 0;

	typedef int (*VoikkoSpellCstrFunction)(struct VoikkoHandle* handle, const char* word);
	VoikkoSpellCstrFunction voikkoSpellCstr = 0;

	typedef char** (*VoikkoSuggestCstrFunction)(struct VoikkoHandle* handle, const char* word);
	VoikkoSuggestCstrFunction voikkoSuggestCstr = 0;

	typedef void (*VoikkoFreeCstrArrayFunction)(char** cstrArray);
	VoikkoFreeCstrArrayFunction voikkoFreeCstrArray = 0;

	typedef char** (*VoikkoListSupportedSpellingLanguagesFunction)(const char * path);
	VoikkoListSupportedSpellingLanguagesFunction voikkoListSupportedSpellingLanguages = 0;

	bool f_voikko_loaded = false;
	QList<VoikkoHandle*> f_handles;
	QByteArray f_voikko_path;
}

//-----------------------------------------------------------------------------

static bool f_ignore_numbers = false;
static bool f_ignore_uppercase = true;

//-----------------------------------------------------------------------------

namespace
{

class DictionaryVoikko : public AbstractDictionary
{
public:
	DictionaryVoikko(const QString& language);
	~DictionaryVoikko();

	bool isValid() const
	{
		return m_handle;
	}

	QStringRef check(const QString& string, int start_at) const;
	QStringList suggestions(const QString& word) const;

	void addToPersonal(const QString& word);
	void addToSession(const QStringList& words);
	void removeFromSession(const QStringList& words);

private:
	VoikkoHandle* m_handle;
};

//-----------------------------------------------------------------------------

DictionaryVoikko::DictionaryVoikko(const QString& language) :
	m_handle(0)
{
	const char* voikko_error;
	m_handle = voikkoInit(&voikko_error, language.toUtf8().constData(), f_voikko_path.constData());
	if (voikko_error) {
		qWarning("DictionaryVoikko(%s): %s", qPrintable(language), voikko_error);
	} else if (m_handle) {
		voikkoSetBooleanOption(m_handle, VOIKKO_OPT_IGNORE_NUMBERS, f_ignore_numbers);
		voikkoSetBooleanOption(m_handle, VOIKKO_OPT_IGNORE_UPPERCASE, f_ignore_uppercase);
		f_handles.append(m_handle);
	}
}

//-----------------------------------------------------------------------------

DictionaryVoikko::~DictionaryVoikko()
{
	if (m_handle) {
		f_handles.removeAll(m_handle);
		voikkoTerminate(m_handle);
	}
}

//-----------------------------------------------------------------------------

QStringRef DictionaryVoikko::check(const QString& string, int start_at) const
{
	int index = -1;
	int length = 0;
	int chars = 1;
	bool is_word = false;

	int count = string.length() - 1;
	for (int i = start_at; i <= count; ++i) {
		QChar c = string.at(i);
		if (c.isLetterOrNumber() || c.category() == QChar::Punctuation_Dash) {
			if (index == -1) {
				index = i;
				chars = 1;
				length = 0;
			}
			length += chars;
			chars = 1;
		} else if (c != 0x2019 && c != 0x0027) {
			if (index != -1) {
				is_word = true;
			}
		} else {
			chars++;
		}

		if (is_word || (i == count && index != -1)) {
			QStringRef check(&string, index, length);
			if (voikkoSpellCstr(m_handle, check.toString().toUtf8().constData()) != VOIKKO_SPELL_OK) {
				return check;
			}
			index = -1;
			is_word = false;
		}
	}

	return QStringRef();
}

//-----------------------------------------------------------------------------

QStringList DictionaryVoikko::suggestions(const QString& word) const
{
	QStringList result;
	char** suggestions = voikkoSuggestCstr(m_handle, word.toUtf8().constData());
	if (suggestions) {
		for (size_t i = 0; suggestions[i] != NULL; ++i) {
            QString word = QString::fromUtf8(suggestions[i]);
			result.append(word);
		}
		voikkoFreeCstrArray(suggestions);
	}
	return result;
}

//-----------------------------------------------------------------------------

void DictionaryVoikko::addToPersonal(const QString& word)
{
	DictionaryManager::instance().add(word);
}

//-----------------------------------------------------------------------------

void DictionaryVoikko::addToSession(const QStringList& words)
{
	Q_UNUSED(words);
	// No personal word list support in voikko?
}

//-----------------------------------------------------------------------------

void DictionaryVoikko::removeFromSession(const QStringList& words)
{
	Q_UNUSED(words);
	// No personal word list support in voikko?
}

}

//-----------------------------------------------------------------------------

DictionaryProviderVoikko::DictionaryProviderVoikko()
{
	if (f_voikko_loaded) {
		return;
	}

	QString lib = "libvoikko";
#ifdef Q_OS_WIN
	QStringList dictdirs = QDir::searchPaths("dict");
	foreach (const QString dictdir, dictdirs) {
		lib = dictdir + "/libvoikko-1.dll";
		if (QLibrary(lib).load()) {
			f_voikko_path = QFile::encodeName(QDir::toNativeSeparators(QFileInfo(lib).path()));
			break;
		}
	}
#endif
	QLibrary voikko_lib(lib);
	if (!voikko_lib.load()) {
		return;
	}

	voikkoInit = (VoikkoInitFunction) voikko_lib.resolve("voikkoInit");
	voikkoTerminate = (VoikkoTerminateFunction) voikko_lib.resolve("voikkoTerminate");
	voikkoSetBooleanOption = (VoikkoSetBooleanOptionFunction) voikko_lib.resolve("voikkoSetBooleanOption");
	voikkoSpellCstr = (VoikkoSpellCstrFunction) voikko_lib.resolve("voikkoSpellCstr");
	voikkoSuggestCstr = (VoikkoSuggestCstrFunction) voikko_lib.resolve("voikkoSuggestCstr");
	voikkoFreeCstrArray = (VoikkoFreeCstrArrayFunction) voikko_lib.resolve("voikkoFreeCstrArray");
	voikkoListSupportedSpellingLanguages = (VoikkoListSupportedSpellingLanguagesFunction) voikko_lib.resolve("voikkoListSupportedSpellingLanguages");
	f_voikko_loaded = (voikkoInit != 0)
			&& (voikkoTerminate != 0)
			&& (voikkoSetBooleanOption != 0)
			&& (voikkoSpellCstr != 0)
			&& (voikkoSuggestCstr != 0)
			&& (voikkoFreeCstrArray != 0)
			&& (voikkoListSupportedSpellingLanguages != 0);
	if (!f_voikko_loaded) {
		voikkoInit = 0;
		voikkoTerminate = 0;
		voikkoSetBooleanOption = 0;
		voikkoSpellCstr = 0;
		voikkoSuggestCstr = 0;
		voikkoFreeCstrArray = 0;
		voikkoListSupportedSpellingLanguages = 0;
	}
}

//-----------------------------------------------------------------------------

bool DictionaryProviderVoikko::isValid() const
{
	return f_voikko_loaded;
}

//-----------------------------------------------------------------------------

QStringList DictionaryProviderVoikko::availableDictionaries() const
{
	if (!f_voikko_loaded) {
		return QStringList();
	}

	QStringList result;
	char** languages = voikkoListSupportedSpellingLanguages(f_voikko_path.constData());
	if (languages) {
		for (size_t i = 0; languages[i] != NULL; ++i) {
			result.append(QString::fromUtf8(languages[i]));
		}
		voikkoFreeCstrArray(languages);
	}
	return result;
}

//-----------------------------------------------------------------------------

AbstractDictionary* DictionaryProviderVoikko::requestDictionary(const QString& language) const
{
	if (!f_voikko_loaded) {
		return 0;
	}

	return new DictionaryVoikko(language);
}

//-----------------------------------------------------------------------------

void DictionaryProviderVoikko::setIgnoreNumbers(bool ignore)
{
	f_ignore_numbers = ignore;
	foreach (VoikkoHandle* handle, f_handles) {
		voikkoSetBooleanOption(handle, VOIKKO_OPT_IGNORE_NUMBERS, ignore);
	}
}

//-----------------------------------------------------------------------------

void DictionaryProviderVoikko::setIgnoreUppercase(bool ignore)
{
	f_ignore_uppercase = ignore;
	foreach (VoikkoHandle* handle, f_handles) {
		voikkoSetBooleanOption(handle, VOIKKO_OPT_IGNORE_UPPERCASE, ignore);
	}
}

//-----------------------------------------------------------------------------
