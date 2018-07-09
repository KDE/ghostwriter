/***********************************************************************
 *
 * Copyright (C) 2009, 2010, 2011, 2012, 2013 Graeme Gott <graeme@gottcode.org>
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

#ifndef ABSTRACT_DICTIONARY_H
#define ABSTRACT_DICTIONARY_H

class QString;
class QStringList;
class QStringRef;

class AbstractDictionary
{
public:
	virtual ~AbstractDictionary() { }

	virtual bool isValid() const = 0;
	virtual QStringRef check(const QString& string, int start_at) const = 0;
	virtual QStringList suggestions(const QString& word) const = 0;

	virtual void addToPersonal(const QString& word) = 0;
	virtual void addToSession(const QStringList& words) = 0;
	virtual void removeFromSession(const QStringList& words) = 0;
};

#endif
