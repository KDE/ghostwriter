/***********************************************************************
 *
 * Copyright (C) 2009, 2010, 2011, 2012, 2013 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) 2014-2022 wereturtle
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

#include <string.h>
#include <optional>
#include <vector>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QListIterator>
#include <QRegularExpression>
#include <QStringList>
#include <QStringRef>
#include <QtGlobal>

#include "hunspellprovider.h"

#include "dictionary.h"
#include "dictionarymanager.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #include <QTextCodec>
#else
    #include <QStringConverter>
    #include <QStringEncoder>
    #include <QStringDecoder>
#endif

#ifdef _WIN32
    #include "3rdparty/hunspell/hunspell.hxx"
#else
    #include <hunspell.hxx>
#endif

static bool f_ignoreNumbers = false;
static bool f_ignoreUppercase = true;

namespace ghostwriter
{

class DictionaryHunspell : public Dictionary
{
public:
	DictionaryHunspell(const QString &language);
	~DictionaryHunspell();

	bool isValid() const
	{
		return (nullptr != m_dictionary);
	}

	QStringRef check(const QString &string, int startAt) const;
	QStringList suggestions(const QString &word) const;

	void addToPersonal(const QString &word);
	void addToSession(const QStringList &words);
	void removeFromSession(const QStringList &words);

private:
	Hunspell *m_dictionary;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QTextCodec *m_codec;
#else
    QStringEncoder *m_encoder;
    QStringDecoder *m_decoder;
#endif
};

DictionaryHunspell::DictionaryHunspell(const QString &language) :
	m_dictionary(nullptr),
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	m_codec(nullptr)
#else
    m_encoder(nullptr),
    m_decoder(nullptr)
#endif
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
	m_dictionary = new Hunspell( ("\\\\?\\" + QDir::toNativeSeparators(aff)).toUtf8().toStdString(),
			("\\\\?\\" + QDir::toNativeSeparators(dic)).toUtf8().constData() );
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	m_codec = QTextCodec::codecForName(m_dictionary->get_dic_encoding());

    if (!m_codec) {
        delete m_dictionary;
        m_dictionary = nullptr;
    }
#else
    std::optional<QStringConverter::Encoding> encoding =
        QStringConverter::encodingForName(m_dictionary->get_dic_encoding());

    if (!encoding.has_value()) {
        delete m_dictionary;
        m_dictionary = nullptr;
    } else {
        m_encoder = new QStringEncoder(encoding.value());
        m_decoder = new QStringDecoder(encoding.value());
    }
#endif
}

DictionaryHunspell::~DictionaryHunspell()
{
    if (nullptr != m_dictionary) {
	   delete m_dictionary;
       m_dictionary = nullptr;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (nullptr != m_encoder) {
        delete m_encoder;
        m_encoder = nullptr;
    }

    if (nullptr != m_decoder) {
        delete m_decoder;
        m_decoder = nullptr;
    }
#endif
}

QStringRef DictionaryHunspell::check(const QString &string, int startAt) const
{
    // Incorporated ghostwriter's word splitting algorithm into the original
    // FocusWriter algorithm to ensure hyphenated words are counted as one
    // word.

    bool inWord = false;
    int separatorCount = 0;
    int wordLen = 0;
    bool isNumber = false;
    bool isUppercase = f_ignoreUppercase;
    bool isWord = false;
    int index = -1;

    for (int i = startAt; i < string.length(); i++) {
        QChar c = string.at(i);

        if (c.isLetterOrNumber() || c.isMark()) {
            inWord = true;
            separatorCount = 0;
            wordLen++;

            if (index < 0) {
                index = i;
            }

            if (c.isNumber()) {
                isNumber = f_ignoreNumbers;
            }
            else if (c.isLower()) {
                isUppercase = false;
            }
        }
        else if (c.isSpace() && inWord) {
            inWord = false;
            isWord = true;

            if (separatorCount > 0) {
                wordLen--;
            }
        }
        else {
            // This is to handle things like double dashes (`--`)
            // that separate words, while still counting hyphenated
            // words as a single word.
            //
            separatorCount++;

            if (inWord) {
                // Only count dashes (`-`), periods (`.`) and single
                // quotation marks (`'`) as separators that can be
                // used inside a single word.
                //
                if ((1 == separatorCount)
                        && (QChar('-') != c)
                        && (QChar('.') != c)
                        && (QChar('\'') != c)) {
                    separatorCount = 0;
                    inWord = false;
                    isWord = true;
                }
                else if (separatorCount > 1) {
                    separatorCount = 0;
                    inWord = false;
                    isWord = true;
                    wordLen--;
                }
                else {
                    wordLen++;
                }
            }
        }

        if (inWord && ((string.length() - 1) == i)) {
            isWord = true;

            if (separatorCount > 0) {
                wordLen--;
            }
        }

        if (isWord && (index >= 0)) {
            if (!isUppercase && !isNumber) {
                QStringRef check(&string, index, wordLen);
                QString word = check.toString();

                // Replace any fancy single quotes with a "normal" single quote.
                word.replace(QChar(0x2019), QLatin1Char('\''));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                if (!m_dictionary->spell(m_codec->fromUnicode(word).toStdString())) {
                    return check;
                }
#else
                QByteArray encoded = m_encoder->encode(word);
                if (!m_dictionary->spell(encoded.toStdString())) {
                    return check;
                }
#endif
            }

            index = -1;
            wordLen = 0;
            separatorCount = 0;
            isWord = false;
            inWord = false;
            isNumber = false;
            isUppercase = f_ignoreUppercase;
        }
    }

	return QStringRef();
}

QStringList DictionaryHunspell::suggestions(const QString &word) const
{
	QStringList result;
	QString check = word;

    // Replace any fancy single quotes with a "normal" single quote.
	check.replace(QChar(0x2019), QLatin1Char('\''));

	std::vector<std::string> suggestions;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	suggestions = m_dictionary->suggest(m_codec->fromUnicode(check).toStdString());
#else
    QByteArray encoded = m_encoder->encode(word);
    suggestions = m_dictionary->suggest(encoded.toStdString());
#endif

    for (const std::string &suggestion : suggestions) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QString word = m_codec->toUnicode(suggestion.c_str());
#else
        QString word = m_decoder->decode(suggestion.c_str());
#endif
		result.append(word);
	}

	return result;
}

void DictionaryHunspell::addToPersonal(const QString &word)
{
    // Replace any fancy single quotes with a "normal" single quote.
    QString sanitized = word;
    sanitized.replace(QChar(0x2019), QLatin1Char('\''));

    DictionaryManager::instance()->add(sanitized);
}

void DictionaryHunspell::addToSession(const QStringList &words)
{
	for (const QString &word : words) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		m_dictionary->add(m_codec->fromUnicode(word).toStdString());
#else
        QByteArray encoded = m_encoder->encode(word);
		m_dictionary->add(encoded.toStdString());
#endif
	}
}

void DictionaryHunspell::removeFromSession(const QStringList &words)
{
	for (const QString &word : words) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		m_dictionary->remove(m_codec->fromUnicode(word).toStdString());
#else
        QByteArray encoded = m_encoder->encode(word);
		m_dictionary->remove(encoded.toStdString());
#endif
	}
}

HunspellProvider::HunspellProvider()
{
	QStringList dictdirs = QDir::searchPaths("dict");
#if !defined(Q_OS_MAC) && defined(Q_OS_UNIX)
	QStringList xdg = QString(qgetenv("XDG_DATA_DIRS")).split(QChar(':'), Qt::SkipEmptyParts);
	if (xdg.isEmpty()) {
		xdg.append("/usr/local/share");
		xdg.append("/usr/share");
	}
	QStringList subdirs = QStringList() << "/hunspell" << "/myspell/dicts" << "/myspell" << "/mozilla-dicts";
	for (const QString &subdir : subdirs) {
		for (const QString &dir : xdg) {
			QString path = dir + subdir;
			if (!dictdirs.contains(path)) {
				dictdirs.append(path);
			}
		}
	}
#endif
	QDir::setSearchPaths("dict", dictdirs);
}

QStringList HunspellProvider::availableDictionaries() const
{
	QStringList result;
	QStringList locations = QDir::searchPaths("dict");
	QListIterator<QString> i(locations);
	while (i.hasNext()) {
		QDir dir(i.next());

		QStringList dicFiles = dir.entryList(QStringList() << "*.dic*", QDir::Files, QDir::Name | QDir::IgnoreCase);
		dicFiles.replaceInStrings(QRegularExpression("\\.dic.*"), "");
		QStringList affFiles = dir.entryList(QStringList() << "*.aff*", QDir::Files);
		affFiles.replaceInStrings(QRegularExpression("\\.aff.*"), "");

		for (const QString &language : dicFiles) {
			if (affFiles.contains(language) && !result.contains(language)) {
				result.append(language);
			}
		}
	}
	return result;
}

Dictionary * HunspellProvider::requestDictionary(const QString &language) const
{
	return new DictionaryHunspell(language);
}

void HunspellProvider::setIgnoreNumbers(bool ignore)
{
	f_ignoreNumbers = ignore;
}

void HunspellProvider::setIgnoreUppercase(bool ignore)
{
	f_ignoreUppercase = ignore;
}

} // namespace ghostwriter
