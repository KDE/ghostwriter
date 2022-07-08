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
	AbstractDictionary* requestDictionary(const QString &language) const;

	void setIgnoreNumbers(bool ignore);
	void setIgnoreUppercase(bool ignore);
};
} // namespace ghostwriter

#endif
