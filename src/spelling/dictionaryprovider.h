/*
 * SPDX-FileCopyrightText: 2013 Graeme Gott <graeme@gottcode.org>
 * SPDX-FileCopyrightText: 2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DICTIONARY_PROVIDER_H
#define DICTIONARY_PROVIDER_H

#include <QString>
#include <QStringList>

#include "dictionary.h"

namespace ghostwriter
{
class DictionaryProvider
{
public:
	virtual ~DictionaryProvider() { }

	virtual bool isValid() const = 0;
	virtual QStringList availableDictionaries() const = 0;
	virtual Dictionary * requestDictionary(const QString &language) const = 0;

	virtual void setIgnoreNumbers(bool ignore) = 0;
	virtual void setIgnoreUppercase(bool ignore) = 0;
};
} // namespace ghostwriter

#endif
