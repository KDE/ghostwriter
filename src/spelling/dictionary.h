/*
 * SPDX-FileCopyrightText: 2009-2013 Graeme Gott <graeme@gottcode.org>
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QString>
#include <QStringList>
#include <QStringRef>

namespace ghostwriter
{
class Dictionary
{
public:
	Dictionary() { }
    virtual ~Dictionary() { }

	virtual bool isValid() const = 0;
	virtual QStringRef check(const QString &string, int startAt) const = 0;
	virtual QStringList suggestions(const QString &word) const = 0;

	virtual void addToPersonal(const QString &word) = 0;
	virtual void addToSession(const QStringList &words) = 0;
	virtual void removeFromSession(const QStringList &words) = 0;
};
} // namespace ghostwriter
#endif
