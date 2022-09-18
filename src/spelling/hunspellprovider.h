/*
 * SPDX-FileCopyrightText: 2013 Graeme Gott <graeme@gottcode.org>
 * SPDX-FileCopyrightText: 2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef HUNSPELL_PROVIDER_H
#define HUNSPELL_PROVIDER_H

#include "dictionaryprovider.h"

#include <QString>
#include <QStringList>
#include <QStringRef>

namespace ghostwriter
{
class HunspellProvider : public DictionaryProvider
{
public:
	HunspellProvider();

	bool isValid() const
	{
		return true;
	}

	QStringList availableDictionaries() const;
	Dictionary *requestDictionary(const QString &language) const;

	void setIgnoreNumbers(bool ignore);
	void setIgnoreUppercase(bool ignore);
};
} // namespace ghostwriter

#endif
