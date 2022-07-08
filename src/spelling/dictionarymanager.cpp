/***********************************************************************
 *
 * Copyright (C) 2009, 2010, 2011, 2012, 2013 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) 2022 wereturtle
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

#include "dictionarymanager.h"

#ifndef Q_OS_MAC
#include "hunspellprovider.h"
#else
#include "nsspellcheckerprovider.h"
#endif

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

#include <algorithm>

namespace ghostwriter
{

bool compareWords(const QString &s1, const QString &s2)
{
	return s1.localeAwareCompare(s2) < 0;
}

class DictionaryFallback : public Dictionary
{
public:
	static Dictionary * instance()
	{
		static DictionaryFallback fallback;
		static Dictionary *fallback_ptr = &fallback;
		return fallback_ptr;
	}

	bool isValid() const
	{
		return true;
	}

	QStringRef check(const QString &string, int startAt) const
	{
		Q_UNUSED(string);
		Q_UNUSED(startAt);
		return QStringRef();
	}

	QStringList suggestions(const QString &word) const
	{
		Q_UNUSED(word);
		return QStringList();
	}

	void addToPersonal(const QString &word)
	{
		Q_UNUSED(word);
	}

	void addToSession(const QStringList &words)
	{
		Q_UNUSED(words);
	}

	void removeFromSession(const QStringList &words)
	{
		Q_UNUSED(words);
	}

private:
	DictionaryFallback()
	{
	}
};

QString DictionaryManager::m_path;

DictionaryManager* DictionaryManager::instance()
{
	static DictionaryManager manager;
	return &manager;
}

QStringList DictionaryManager::availableDictionaries() const
{
	QStringList result;

	for (DictionaryProvider* provider : m_providers) {
		result += provider->availableDictionaries();
	}

	result.sort();
	result.removeDuplicates();
	return result;
}

QString DictionaryManager::availableDictionary(const QString &language) const
{
	QStringList languages = availableDictionaries();

	if (!languages.isEmpty() && !languages.contains(language)) {
		int close = languages.indexOf(QRegularExpression(language.left(2) + ".*"));
		return (close != -1) ? languages.at(close) : (languages.contains("en_US") ? "en_US" : languages.first());
	} else {
		return language;
	}
}

void DictionaryManager::add(const QString& word)
{
	QStringList words = personal();

    if (words.contains(word)) {
		return;
	}

	words.append(word);
	setPersonal(words);
}

void DictionaryManager::addProviders()
{
#ifndef Q_OS_MAC
	bool hasHunspell = false;

	for (DictionaryProvider *provider : m_providers) {
		if (dynamic_cast<HunspellProvider*>(provider) != NULL) {
			hasHunspell = true;
		}
	}

	if (!hasHunspell) {
		addProvider(new HunspellProvider);
	}
#else
	bool hasNsSpellChecker = false;

	for (DictionaryProvider *provider : m_providers) {
		if (dynamic_cast<NSSpellCheckerProvider*>(provider) != NULL) {
			hasNsSpellChecker = true;
		}
	}

	if (!hasNsSpellChecker) {
		addProvider(new NSSpellCheckerProvider);
	}
#endif
}

Dictionary * DictionaryManager::requestDictionary(const QString &language)
{
	if (language.isEmpty()) {
		// Fetch shared default dictionary
		if (!m_defaultDictionary) {
			m_defaultDictionary = requestDictionaryData(m_defaultLanguage);
		}
		return m_defaultDictionary;
	} else {
		// Fetch specific dictionary
		return requestDictionaryData(language);
	}
}

void DictionaryManager::setDefaultLanguage(const QString &language)
{
	if (language == m_defaultLanguage) {
		return;
	}

	m_defaultLanguage = language;
	m_defaultDictionary = requestDictionaryData(m_defaultLanguage);

	// Re-check documents
	emit changed();
}

void DictionaryManager::setIgnoreNumbers(bool ignore)
{
	for (DictionaryProvider *provider : m_providers) {
		provider->setIgnoreNumbers(ignore);
	}

	// Re-check documents
	emit changed();
}

void DictionaryManager::setIgnoreUppercase(bool ignore)
{
	for (DictionaryProvider *provider : m_providers) {
		provider->setIgnoreUppercase(ignore);
	}

	// Re-check documents
	emit changed();
}

QString DictionaryManager::installedPath()
{
#ifndef Q_OS_MAC
	return m_path;
#else
	return QDir::homePath() + "/Library/Spelling/";
#endif
}

void DictionaryManager::setPath(const QString& path)
{
	m_path = path;
}

void DictionaryManager::setPersonal(const QStringList &words)
{
	// Check if new
    QStringList personal = words;
	std::sort(personal.begin(), personal.end(), compareWords);

	if (personal == m_personal) {
		return;
	}

	// Remove current personal dictionary
	foreach (Dictionary* dictionary, m_dictionaries) {
		dictionary->removeFromSession(m_personal);
	}

	// Update and store personal dictionary
	m_personal = personal;
	QFile file(m_path + "/personal");
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		stream.setCodec("UTF-8");
#else
		stream.setEncoding(QStringConverter::Utf8);
#endif
		for (const QString &word : m_personal) {
			stream << word << "\n";
		}
	}

	// Add personal dictionary
	for (Dictionary *dictionary : m_dictionaries) {
		dictionary->addToSession(m_personal);
	}

	// Re-check documents
	emit changed();
}

DictionaryManager::DictionaryManager()
{
	addProviders();

	// Load personal dictionary
	QFile file(m_path + "/personal");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		stream.setCodec("UTF-8");
#else
		stream.setEncoding(QStringConverter::Utf8);
#endif
		while (!stream.atEnd()) {
			m_personal.append(stream.readLine());
		}

		std::sort(m_personal.begin(), m_personal.end(), compareWords);
	}
}

DictionaryManager::~DictionaryManager()
{
	foreach (Dictionary* dictionary, m_dictionaries) {
		delete dictionary;
	}

	m_dictionaries.clear();

	qDeleteAll(m_providers);
	m_providers.clear();
}

void DictionaryManager::addProvider(DictionaryProvider *provider)
{
	if (provider->isValid()) {
		m_providers.append(provider);
	} else {
		delete provider;
		provider = 0;
	}
}

Dictionary* DictionaryManager::requestDictionaryData(const QString &language)
{
	if (!m_dictionaries.contains(language)) {
		Dictionary *dictionary = 0;
		for (DictionaryProvider *provider : m_providers) {
			dictionary = provider->requestDictionary(language);
			if (dictionary && dictionary->isValid()) {
				break;
			} else {
				delete dictionary;
				dictionary = nullptr;
			}
		}

		if (!dictionary) {
			return DictionaryFallback::instance();
		}

		dictionary->addToSession(m_personal);
		m_dictionaries[language] = dictionary;
	}

	return m_dictionaries[language];
}
} // namespace ghostwriter
