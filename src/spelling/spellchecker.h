/***********************************************************************
 *
 * Copyright (C) 2009, 2010, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QAction>
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextCursor>

#include "dictionary.h"

namespace ghostwriter
{
class SpellChecker : public QDialog
{
	Q_OBJECT

public:
    static void checkDocument(QPlainTextEdit *document, Dictionary *dictionary);

public slots:
	virtual void reject();

private slots:
	void suggestionChanged(QListWidgetItem *suggestion);
	void add();
	void ignore();
	void ignoreAll();
	void change();
	void changeAll();

private:
    SpellChecker(QPlainTextEdit *document, Dictionary *dictionary);
	void check();

private:
	Dictionary *m_dictionary;

    QPlainTextEdit *m_document;
    QPlainTextEdit *m_context;
	QLineEdit *m_suggestion;
	QListWidget *m_suggestions;
	QTextCursor m_cursor;
	QTextCursor m_startCursor;

	int m_checkedBlocks;
	int m_totalBlocks;
	bool m_loopAvailable;

	QString m_word;
	QStringList m_ignored;
};
} // namespace ghostwriter

#endif
