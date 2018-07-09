/***********************************************************************
 *
 * Copyright (C) 2012, 2013 Graeme Gott <graeme@gottcode.org>
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

#include "dictionary_provider_nsspellchecker.h"

#include "abstract_dictionary.h"
#include "dictionary_manager.h"

#include <QStringList>
#include <QVector>

#import <AppKit/NSSpellChecker.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSString.h>

//-----------------------------------------------------------------------------

static NSArray* convertList(const QStringList& words)
{
	QVector<NSString*> strings;
	foreach (const QString& word, words) {
		strings.append([NSString stringWithCharacters:reinterpret_cast<const unichar*>(word.unicode()) length:word.length()]);
	}

	NSArray* array = [NSArray arrayWithObjects:strings.constData() count:strings.size()];
	return array;
}

//-----------------------------------------------------------------------------

namespace
{

class DictionaryNSSpellChecker : public AbstractDictionary
{
public:
	DictionaryNSSpellChecker(const QString& language);
	~DictionaryNSSpellChecker();

	bool isValid() const
	{
		return true;
	}

	QStringRef check(const QString& string, int start_at) const;
	QStringList suggestions(const QString& word) const;

	void addToPersonal(const QString& word);
	void addToSession(const QStringList& words);
	void removeFromSession(const QStringList& words);

private:
	NSString* m_language;
	NSInteger m_tag;
};

//-----------------------------------------------------------------------------

DictionaryNSSpellChecker::DictionaryNSSpellChecker(const QString& language)
{
	m_language = [[NSString alloc] initWithCharacters:reinterpret_cast<const unichar*>(language.unicode()) length:language.length()];

	m_tag = [NSSpellChecker uniqueSpellDocumentTag];
}

//-----------------------------------------------------------------------------

DictionaryNSSpellChecker::~DictionaryNSSpellChecker()
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[m_language release];

	[[NSSpellChecker sharedSpellChecker] closeSpellDocumentWithTag:m_tag];

	[pool release];
}

//-----------------------------------------------------------------------------

QStringRef DictionaryNSSpellChecker::check(const QString& string, int start_at) const
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	NSString* nsstring = [NSString stringWithCharacters:reinterpret_cast<const unichar*>(string.unicode()) length:string.length()];

	QStringRef misspelled;

	NSRange range = [[NSSpellChecker sharedSpellChecker] checkSpellingOfString:nsstring
		startingAt:start_at
		language:m_language
		wrap:NO
		inSpellDocumentWithTag:m_tag
		wordCount:NULL];

	if (range.length > 0) {
		misspelled = QStringRef(&string, range.location, range.length);
	}

	[pool release];

	return misspelled;
}

//-----------------------------------------------------------------------------

QStringList DictionaryNSSpellChecker::suggestions(const QString& word) const
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	NSRange range;
	range.location = 0;
	range.length = word.length();

	NSString* nsstring = [NSString stringWithCharacters:reinterpret_cast<const unichar*>(word.unicode()) length:word.length()];

	NSArray* array;
        array = [[NSSpellChecker sharedSpellChecker] guessesForWordRange:range
            inString:nsstring
            language:m_language
            inSpellDocumentWithTag:m_tag];

	QStringList suggestions;
	if (array) {
		for (unsigned int i = 0; i < [array count]; ++i) {
			nsstring = [array objectAtIndex: i];
			suggestions += QString::fromUtf8([nsstring UTF8String]);
		}
	}

	[pool release];

	return suggestions;
}

//-----------------------------------------------------------------------------

void DictionaryNSSpellChecker::addToPersonal(const QString& word)
{
	DictionaryManager::instance().add(word);
}

//-----------------------------------------------------------------------------

void DictionaryNSSpellChecker::addToSession(const QStringList& words)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[[NSSpellChecker sharedSpellChecker] setIgnoredWords:convertList(words) inSpellDocumentWithTag:m_tag];

	[pool release];
}

//-----------------------------------------------------------------------------

void DictionaryNSSpellChecker::removeFromSession(const QStringList& words)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	QStringList session;
	NSArray* array = [[NSSpellChecker sharedSpellChecker] ignoredWordsInSpellDocumentWithTag:m_tag];
	if (array) {
		for (unsigned int i = 0; i < [array count]; ++i) {
			session += QString::fromUtf8([[array objectAtIndex: i] UTF8String]);
		}
		foreach (const QString& word, words) {
			session.removeAll(word);
		}
	}

	[[NSSpellChecker sharedSpellChecker] setIgnoredWords:convertList(session) inSpellDocumentWithTag:m_tag];

	[pool release];
}

}

//-----------------------------------------------------------------------------

QStringList DictionaryProviderNSSpellChecker::availableDictionaries() const
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	QStringList languages;
	NSArray* array = [[NSSpellChecker sharedSpellChecker] availableLanguages];
	if (array) {
		for (unsigned int i = 0; i < [array count]; ++i) {
			languages += QString::fromUtf8([[array objectAtIndex: i] UTF8String]);
		}
	}

	[pool release];

	return languages;
}

//-----------------------------------------------------------------------------

AbstractDictionary* DictionaryProviderNSSpellChecker::requestDictionary(const QString& language) const
{
	return new DictionaryNSSpellChecker(language);
}

//-----------------------------------------------------------------------------

void DictionaryProviderNSSpellChecker::setIgnoreNumbers(bool ignore)
{
	Q_UNUSED(ignore)
	// Can't tell NSSpellChecker to ignore words with numbers?
}

//-----------------------------------------------------------------------------

void DictionaryProviderNSSpellChecker::setIgnoreUppercase(bool ignore)
{
	Q_UNUSED(ignore)
	// Can't tell NSSpellChecker to ignore words in all uppercase?
}

//-----------------------------------------------------------------------------
