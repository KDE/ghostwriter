/*
 * SPDX-FileCopyrightText: 2013 Graeme Gott <graeme@gottcode.org>
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef NSSPELLCHECKER_PROVIDER_H
#define NSSPELLCHECKER_PROVIDER_H

#include "dictionaryprovider.h"

#include <QString>
#include <QStringList>

namespace ghostwriter
{
class NSSpellCheckerProvider : public DictionaryProvider
{
public:
	bool isValid() const
	{
		return true;
	}

	QStringList availableDictionaries() const;
	Dictionary* requestDictionary(const QString &language) const;

	void setIgnoreNumbers(bool ignore);
	void setIgnoreUppercase(bool ignore);
};
} // namespace ghostwriter

#endif
