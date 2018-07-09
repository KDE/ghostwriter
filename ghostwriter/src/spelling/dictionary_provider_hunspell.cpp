/***********************************************************************
 *
 * Copyright (C) 2009, 2010, 2011, 2012, 2013 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) 2014-2017 wereturtle
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

#include "dictionary_provider_hunspell.h"

#include "abstract_dictionary.h"
#include "dictionary_manager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QListIterator>
#include <QRegExp>
#include <QStringList>
#include <QTextCodec>

#include <hunspell.hxx>

//-----------------------------------------------------------------------------

static bool f_ignore_numbers = false;
static bool f_ignore_uppercase = true;

//-----------------------------------------------------------------------------

namespace
{

class DictionaryHunspell : public AbstractDictionary
{
public:
	DictionaryHunspell(const QString& language);
	~DictionaryHunspell();

	bool isValid() const
	{
		return m_dictionary;
	}

	QStringRef check(const QString& string, int start_at) const;
	QStringList suggestions(const QString& word) const;

	void addToPersonal(const QString& word);
	void addToSession(const QStringList& words);
	void removeFromSession(const QStringList& words);

private:
	Hunspell* m_dictionary;
	QTextCodec* m_codec;
};

//-----------------------------------------------------------------------------

DictionaryHunspell::DictionaryHunspell(const QString& language) :
	m_dictionary(0),
	m_codec(0)
{
	// Find dictionary files
    QString aff = QFileInfo("dict:" + language + ".aff").canonicalFilePath();
	if (aff.isEmpty()) {
        aff = QFileInfo("dict:" + language + ".aff.hz").canonicalFilePath();
		aff.chop(3);
	}
    QString dic = QFileInfo("dict:" + language + ".dic").canonicalFilePath();
	if (dic.isEmpty()) {
        dic = QFileInfo("dict:" + language + ".dic.hz").canonicalFilePath();
		dic.chop(3);
	}
	if (language.isEmpty() || aff.isEmpty() || dic.isEmpty()) {
		return;
	}

	// Create dictionary
#ifndef Q_WIN32
	m_dictionary = new Hunspell(QFile::encodeName(aff).constData(), QFile::encodeName(dic).constData());
#else
	m_dictionary = new Hunspell( ("\\\\?\\" + QDir::toNativeSeparators(aff)).toUtf8().constData(),
			("\\\\?\\" + QDir::toNativeSeparators(dic)).toUtf8().constData() );
#endif
	m_codec = QTextCodec::codecForName(m_dictionary->get_dic_encoding());
	if (!m_codec) {
		delete m_dictionary;
		m_dictionary = 0;
	}
}

//-----------------------------------------------------------------------------

DictionaryHunspell::~DictionaryHunspell()
{
	delete m_dictionary;
}

//-----------------------------------------------------------------------------

QStringRef DictionaryHunspell::check(const QString& string, int start_at) const
{
    // Incorporated ghostwriter's word splitting algorithm into the original
    // FocusWriter algorithm to ensure hyphenated words are counted as one
    // word.

    bool inWord = false;
    int separatorCount = 0;
    int wordLen = 0;
    bool isNumber = false;
    bool isUppercase = f_ignore_uppercase;
    bool isWord = false;
    int index = -1;

    for (int i = start_at; i < string.length(); i++)
    {
        QChar c = string.at(i);

        if (c.isLetterOrNumber() || c.isMark())
        {
            inWord = true;
            separatorCount = 0;
            wordLen++;

            if (index < 0)
            {
                index = i;
            }

            if (c.isNumber())
            {
                isNumber = f_ignore_numbers;
            }
            else if (c.isLower())
            {
                isUppercase = false;
            }
        }
        else if (c.isSpace() && inWord)
        {
            inWord = false;
            isWord = true;

            if (separatorCount > 0)
            {
                wordLen--;
            }
        }
        else
        {
            // This is to handle things like double dashes (`--`)
            // that separate words, while still counting hyphenated
            // words as a single word.
            //
            separatorCount++;

            if (inWord)
            {
                // Only count dashes (`-`), periods (`.`) and single
                // quotation marks (`'`) as separators that can be
                // used inside a single word.
                //
                if
                (
                    (1 == separatorCount) &&
                    (QChar('-') != c) &&
                    (QChar('.') != c) &&
                    (QChar('\'') != c)
                )
                {
                    separatorCount = 0;
                    inWord = false;
                    isWord = true;
                }
                else if (separatorCount > 1)
                {
                    separatorCount = 0;
                    inWord = false;
                    isWord = true;
                    wordLen--;
                }
                else
                {
                    wordLen++;
                }
            }
        }

        if (inWord && ((string.length() - 1) == i))
        {
            isWord = true;

            if (separatorCount > 0)
            {
                wordLen--;
            }
        }

        if (isWord && (index >= 0))
        {
            if (!isUppercase && !isNumber)
            {
                QStringRef check(&string, index, wordLen);
                QString word = check.toString();

                // Replace any fancy single quotes with a "normal" single quote.
                word.replace(QChar(0x2019), QLatin1Char('\''));

                if (!m_dictionary->spell(m_codec->fromUnicode(word).constData()))
                {
                    return check;
                }
            }

            index = -1;
            wordLen = 0;
            separatorCount = 0;
            isWord = false;
            inWord = false;
            isNumber = false;
            isUppercase = f_ignore_uppercase;
        }
    }

	return QStringRef();
}

//-----------------------------------------------------------------------------

QStringList DictionaryHunspell::suggestions(const QString& word) const
{
	QStringList result;
	QString check = word;

    // Replace any fancy single quotes with a "normal" single quote.
	check.replace(QChar(0x2019), QLatin1Char('\''));

	char** suggestions = 0;
	int count = m_dictionary->suggest(&suggestions, m_codec->fromUnicode(check).constData());
	if (suggestions != 0) {
		for (int i = 0; i < count; ++i) {
            QString word = m_codec->toUnicode(suggestions[i]);
			result.append(word);
		}
		m_dictionary->free_list(&suggestions, count);
	}
	return result;
}

//-----------------------------------------------------------------------------

void DictionaryHunspell::addToPersonal(const QString& word)
{
    // Replace any fancy single quotes with a "normal" single quote.
    QString sanitized = word;
    sanitized.replace(QChar(0x2019), QLatin1Char('\''));

    DictionaryManager::instance().add(sanitized);
}

//-----------------------------------------------------------------------------

void DictionaryHunspell::addToSession(const QStringList& words)
{
	foreach (const QString& word, words) {
		m_dictionary->add(m_codec->fromUnicode(word).constData());
	}
}

//-----------------------------------------------------------------------------

void DictionaryHunspell::removeFromSession(const QStringList& words)
{
	foreach (const QString& word, words) {
		m_dictionary->remove(m_codec->fromUnicode(word).constData());
	}
}

}

//-----------------------------------------------------------------------------

DictionaryProviderHunspell::DictionaryProviderHunspell()
{
	QStringList dictdirs = QDir::searchPaths("dict");
#if !defined(Q_OS_MAC) && defined(Q_OS_UNIX)
	QStringList xdg = QString(qgetenv("XDG_DATA_DIRS")).split(QChar(':'), QString::SkipEmptyParts);
	if (xdg.isEmpty()) {
		xdg.append("/usr/local/share");
		xdg.append("/usr/share");
	}
	QStringList subdirs = QStringList() << "/hunspell" << "/myspell/dicts" << "/myspell" << "/mozilla-dicts";
	foreach (const QString& subdir, subdirs) {
		foreach (const QString& dir, xdg) {
			QString path = dir + subdir;
			if (!dictdirs.contains(path)) {
				dictdirs.append(path);
			}
		}
	}
#endif
	QDir::setSearchPaths("dict", dictdirs);
}

//-----------------------------------------------------------------------------

QStringList DictionaryProviderHunspell::availableDictionaries() const
{
	QStringList result;
	QStringList locations = QDir::searchPaths("dict");
	QListIterator<QString> i(locations);
	while (i.hasNext()) {
		QDir dir(i.next());

		QStringList dic_files = dir.entryList(QStringList() << "*.dic*", QDir::Files, QDir::Name | QDir::IgnoreCase);
		dic_files.replaceInStrings(QRegExp("\\.dic.*"), "");
		QStringList aff_files = dir.entryList(QStringList() << "*.aff*", QDir::Files);
		aff_files.replaceInStrings(QRegExp("\\.aff.*"), "");

		foreach (const QString& language, dic_files) {
			if (aff_files.contains(language) && !result.contains(language)) {
				result.append(language);
			}
		}
	}
	return result;
}

//-----------------------------------------------------------------------------

AbstractDictionary* DictionaryProviderHunspell::requestDictionary(const QString& language) const
{
	return new DictionaryHunspell(language);
}

//-----------------------------------------------------------------------------

void DictionaryProviderHunspell::setIgnoreNumbers(bool ignore)
{
	f_ignore_numbers = ignore;
}

//-----------------------------------------------------------------------------

void DictionaryProviderHunspell::setIgnoreUppercase(bool ignore)
{
	f_ignore_uppercase = ignore;
}

//-----------------------------------------------------------------------------
