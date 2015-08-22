/***********************************************************************
 *
 * Copyright (C) 2008, 2009, 2010, 2012 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) 2014, 2015 wereturtle
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

#ifndef FIND_DIALOG_H
#define FIND_DIALOG_H

class Stack;

#include <QDialog>
class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QPlainTextEdit;

class FindDialog : public QDialog
{
	Q_OBJECT

public:
    FindDialog(QPlainTextEdit* document);

public slots:
	void findNext();
	void findPrevious();
	void reject();
	void showFindMode();
	void showReplaceMode();

signals:
	void findNextAvailable(bool available);

    // This signal is needed for replace all, to notify the editor that
    // the heading outline needs to be rebuilt.  (This is because doing
    // all the replace operations in one giant edit block causes only
    // one contentsChanged() signal to be emitted for everything, rather
    // than one signal for each change.  Thus, it is impossible to figure
    // out the new document positions for the headings without rebuilding
    // the outline from scratch.)
    //
    void replaceAllComplete();


protected:
	void moveEvent(QMoveEvent* event);
	void showEvent(QShowEvent* event);
    void keyPressEvent(QKeyEvent* event);

private slots:
	void find();
	void findChanged(const QString& text);
	void replace();
	void replaceAll();

private:
	void find(bool backwards);
	void showMode(bool replace);

private:
    QPlainTextEdit* m_document;

	QLineEdit* m_find_string;
	QLabel* m_replace_label;
	QLineEdit* m_replace_string;

	QCheckBox* m_ignore_case;
	QCheckBox* m_whole_words;
	QCheckBox* m_regular_expressions;
	QRadioButton* m_search_backwards;

	QPushButton* m_find_button;
	QPushButton* m_replace_button;
	QPushButton* m_replace_all_button;

	QPoint m_position;
};

#endif
