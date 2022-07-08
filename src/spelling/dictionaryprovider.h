/***********************************************************************
 *
 * Copyright (C) 2013 Graeme Gott <graeme@gottcode.org>
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
