/*
 * SPDX-FileCopyrightText: 2009-2013 Graeme Gott <graeme@gottcode.org>
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DICTIONARY_MANAGER_H
#define DICTIONARY_MANAGER_H

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QStringRef>

#include "dictionary.h"
#include "dictionaryprovider.h"

namespace ghostwriter
{
class DictionaryManager : public QObject
{
	Q_OBJECT

public:
	static DictionaryManager * instance();

	QStringList availableDictionaries() const;
	QString availableDictionary(const QString &language) const;
	QString defaultLanguage() const;
	QStringList personal() const;

	void add(const QString &word);
	void addProviders();
	Dictionary * requestDictionary(const QString &language = QString());
	void setDefaultLanguage(const QString &language);
	void setIgnoreNumbers(bool ignore);
	void setIgnoreUppercase(bool ignore);
	void setPersonal(const QStringList &words);

	static QString installedPath();
	static QString path();
	static void setPath(const QString &path);

signals:
	void changed();

private:
	DictionaryManager();
	~DictionaryManager();

	void addProvider(DictionaryProvider *provider);
	Dictionary * requestDictionaryData(const QString &language);

private:
	QList<DictionaryProvider*> m_providers;
	QHash<QString, Dictionary*> m_dictionaries;
	Dictionary *m_defaultDictionary;

	QString m_defaultLanguage;
	QStringList m_personal;

	static QString m_path;
};

inline QString DictionaryManager::defaultLanguage() const
{
	return m_defaultLanguage;
}

inline QString DictionaryManager::path()
{
	return m_path;
}

inline QStringList DictionaryManager::personal() const
{
	return m_personal;
}
} // namespace ghostwriter
#endif
