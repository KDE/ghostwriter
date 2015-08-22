/***********************************************************************
 *
 * Copyright (C) 2008, 2009, 2010, 2011, 2012, 2014 Graeme Gott <graeme@gottcode.org>
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

#include "find_dialog.h"

#include <QApplication>
#include <QCheckBox>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextDocument>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QTextCursor>
#include <QPlainTextEdit>

//-----------------------------------------------------------------------------

FindDialog::FindDialog(QPlainTextEdit* document)
    : QDialog(document->window(), Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
    m_document(document)
{
	// Create widgets
	QLabel* find_label = new QLabel(tr("Search for:"), this);
	m_find_string = new QLineEdit(this);
	m_replace_label = new QLabel(tr("Replace with:"), this);
	m_replace_string = new QLineEdit(this);
	connect(m_find_string, SIGNAL(textChanged(QString)), this, SLOT(findChanged(QString)));

	m_ignore_case = new QCheckBox(tr("Ignore case"), this);
	m_whole_words = new QCheckBox(tr("Whole words only"), this);
	m_regular_expressions = new QCheckBox(tr("Regular expressions"), this);
	connect(m_regular_expressions, SIGNAL(toggled(bool)), m_whole_words, SLOT(setDisabled(bool)));

	m_search_backwards = new QRadioButton(tr("Search up"), this);
	QRadioButton* search_forwards = new QRadioButton(tr("Search down"), this);
	search_forwards->setChecked(true);

	// Create buttons
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	m_find_button = buttons->addButton(tr("&Find"), QDialogButtonBox::ActionRole);
	m_find_button->setEnabled(false);
	connect(m_find_button, SIGNAL(clicked()), this, SLOT(find()));

	m_replace_button = buttons->addButton(tr("&Replace"), QDialogButtonBox::ActionRole);
	m_replace_button->setEnabled(false);
	connect(m_replace_button, SIGNAL(clicked()), this, SLOT(replace()));

	m_replace_all_button = buttons->addButton(tr("Replace &All"), QDialogButtonBox::ActionRole);
	m_replace_all_button->setEnabled(false);
	connect(m_replace_all_button, SIGNAL(clicked()), this, SLOT(replaceAll()));

	if (!buttons->button(QDialogButtonBox::Close)->icon().isNull()) {
		m_find_button->setIcon(QIcon::fromTheme("edit-find"));
		m_replace_button->setIcon(QIcon::fromTheme("edit-find-replace"));
	}

	// Lay out dialog
	QGridLayout* layout = new QGridLayout(this);
	layout->setColumnStretch(1, 1);
	layout->addWidget(find_label, 0, 0, Qt::AlignRight | Qt::AlignVCenter);
	layout->addWidget(m_find_string, 0, 1, 1, 2);
	layout->addWidget(m_replace_label, 1, 0, Qt::AlignRight | Qt::AlignVCenter);
	layout->addWidget(m_replace_string, 1, 1, 1, 2);
	layout->addWidget(m_ignore_case, 2, 1);
	layout->addWidget(m_whole_words, 3, 1);
	layout->addWidget(m_regular_expressions, 4, 1);
	layout->addWidget(m_search_backwards, 2, 2);
	layout->addWidget(search_forwards, 3, 2);
	layout->addWidget(buttons, 5, 0, 1, 3);
	setFixedWidth(sizeHint().width());

	// Load settings
	QSettings settings;
	m_ignore_case->setChecked(!settings.value("FindDialog/CaseSensitive", false).toBool());
	m_whole_words->setChecked(settings.value("FindDialog/WholeWords", false).toBool());
	m_regular_expressions->setChecked(settings.value("FindDialog/RegularExpressions", false).toBool());
	m_search_backwards->setChecked(settings.value("FindDialog/SearchBackwards", false).toBool());
}

//-----------------------------------------------------------------------------

void FindDialog::findNext()
{
	find(false);
}

//-----------------------------------------------------------------------------

void FindDialog::findPrevious()
{
	find(true);
}

//-----------------------------------------------------------------------------

void FindDialog::reject()
{
	QSettings settings;
	settings.setValue("FindDialog/CaseSensitive", !m_ignore_case->isChecked());
	settings.setValue("FindDialog/WholeWords", m_whole_words->isChecked());
	settings.setValue("FindDialog/RegularExpressions", m_regular_expressions->isChecked());
	settings.setValue("FindDialog/SearchBackwards", m_search_backwards->isChecked());
	QDialog::reject();
}

//-----------------------------------------------------------------------------

void FindDialog::showFindMode()
{
	setWindowTitle(tr("Find"));
	showMode(false);
}

//-----------------------------------------------------------------------------

void FindDialog::showReplaceMode()
{
	setWindowTitle(tr("Replace"));
	showMode(true);
}

//-----------------------------------------------------------------------------

void FindDialog::moveEvent(QMoveEvent* event)
{
	m_position = pos();
	QDialog::moveEvent(event);
}

//-----------------------------------------------------------------------------

void FindDialog::showEvent(QShowEvent* event)
{
	if (!m_position.isNull()) {
		QRect rect(m_position, sizeHint());
		if (QApplication::desktop()->availableGeometry(this).contains(rect)) {
			move(m_position);
		}
	}

	QDialog::showEvent(event);
}

//-----------------------------------------------------------------------------

// This method is needed to show the replace dialog if the replace
// shortcut key sequence is pressed while the find dialog is being
// displayed.
//
void FindDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Replace))
    {
        showReplaceMode();
    }
    else
    {
        QDialog::keyPressEvent(event);
    }
}

//-----------------------------------------------------------------------------

void FindDialog::find()
{
	find(m_search_backwards->isChecked());
}

//-----------------------------------------------------------------------------

void FindDialog::findChanged(const QString& text)
{
	bool enabled = !text.isEmpty();
	m_find_button->setEnabled(enabled);
	m_replace_button->setEnabled(enabled);
	m_replace_all_button->setEnabled(enabled);
	emit findNextAvailable(enabled);
}

//-----------------------------------------------------------------------------

void FindDialog::replace()
{
	QString text = m_find_string->text();
	if (text.isEmpty()) {
		return;
	}

    QPlainTextEdit* document = m_document;
	QTextCursor cursor = document->textCursor();
	Qt::CaseSensitivity cs = m_ignore_case->isChecked() ? Qt::CaseInsensitive : Qt::CaseSensitive;
	if (!m_regular_expressions->isChecked()) {
		if (QString::compare(cursor.selectedText(), text, cs) == 0) {
			cursor.insertText(m_replace_string->text());
			document->setTextCursor(cursor);
		}
	} else {
		QRegExp regex(text, cs, QRegExp::RegExp2);
		QString match = cursor.selectedText();
        if (regex.exactMatch(match)) {
            match.replace(regex, m_replace_string->text());
			cursor.insertText(match);
			document->setTextCursor(cursor);
		}
	}

	find();
}

//-----------------------------------------------------------------------------

void FindDialog::replaceAll()
{
	QString text = m_find_string->text();
	if (text.isEmpty()) {
		return;
	}
	QRegExp regex(text, !m_ignore_case->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive, QRegExp::RegExp2);

	QTextDocument::FindFlags flags;
	if (!m_ignore_case->isChecked()) {
		flags |= QTextDocument::FindCaseSensitively;
	}
	if (m_whole_words->isChecked() && !m_regular_expressions->isChecked()) {
		flags |= QTextDocument::FindWholeWords;
	}

	// Count instances
	int found = 0;
    QPlainTextEdit* document = m_document;
	QTextCursor cursor = document->textCursor();
	cursor.movePosition(QTextCursor::Start);
	if (!m_regular_expressions->isChecked()) {
		forever {
			cursor = document->document()->find(text, cursor, flags);
			if (!cursor.isNull()) {
				found++;
			} else {
				break;
			}
		}
	} else {
		forever {
			cursor = document->document()->find(regex, cursor, flags);
			if (!cursor.isNull() && cursor.hasSelection()) {
				found++;
			} else {
				break;
			}
		}
	}
	if (found) {
		if (QMessageBox::question(this, tr("Question"), tr("Replace %n instance(s)?", "", found), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
			return;
		}
	} else {
		QMessageBox::information(this, tr("Sorry"), tr("Phrase not found."));
		return;
    }

	// Replace instances
	QTextCursor start_cursor = document->textCursor();
    start_cursor.beginEditBlock();
	if (!m_regular_expressions->isChecked()) {
        forever {
			cursor = document->document()->find(text, cursor, flags);
            if (!cursor.isNull()) {
                cursor.insertText(m_replace_string->text());
			} else {
				break;
			}
		}
	} else {
        forever {
			cursor = document->document()->find(regex, cursor, flags);
			if (!cursor.isNull() && cursor.hasSelection()) {
                QString match = cursor.selectedText();
                match.replace(regex, m_replace_string->text());
                cursor.insertText(match);
			} else {
				break;
			}
		}
	}
    start_cursor.endEditBlock();
    document->setTextCursor(start_cursor);
    emit replaceAllComplete();
}

//-----------------------------------------------------------------------------

void FindDialog::find(bool backwards)
{
	QString text = m_find_string->text();
	if (text.isEmpty()) {
		return;
	}
	QRegExp regex(text, !m_ignore_case->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive, QRegExp::RegExp2);

	QTextDocument::FindFlags flags;
	if (!m_ignore_case->isChecked()) {
		flags |= QTextDocument::FindCaseSensitively;
	}
	if (m_whole_words->isChecked() && !m_regular_expressions->isChecked()) {
		flags |= QTextDocument::FindWholeWords;
	}
	if (backwards) {
		flags |= QTextDocument::FindBackward;
	}

    QPlainTextEdit* document = m_document;
    QTextCursor cursor = document->textCursor();
	if (!m_regular_expressions->isChecked()) {
        cursor = document->document()->find(text, cursor, flags);
	} else {
        cursor = document->document()->find(regex, cursor, flags);
	}
	if (cursor.isNull()) {
		cursor = document->textCursor();
		cursor.movePosition(!backwards ? QTextCursor::Start : QTextCursor::End);
		if (!m_regular_expressions->isChecked()) {
            cursor = document->document()->find(text, cursor, flags);
		} else {
            cursor = document->document()->find(regex, cursor, flags);
		}
	}

	if (!cursor.isNull()) {
        document->setTextCursor(cursor);
	} else {
		QMessageBox::information(this, tr("Sorry"), tr("Phrase not found."));
	}
}

//-----------------------------------------------------------------------------

void FindDialog::showMode(bool replace)
{
	m_replace_label->setVisible(replace);
	m_replace_string->setVisible(replace);
	m_replace_button->setVisible(replace);
	m_replace_all_button->setVisible(replace);
	setFixedHeight(sizeHint().height());

	if (!m_regular_expressions->isChecked()) {
        QString text = m_document->textCursor().selectedText().trimmed();
		text.remove(0, text.lastIndexOf(QChar(0x2029)) + 1);
		if (!text.isEmpty()) {
			m_find_string->setText(text);
		}
	}

	m_find_string->setFocus();

    QTextCursor cursor = m_document->textCursor();

    if (cursor.hasSelection())
    {
        m_find_string->setText(cursor.selectedText());
    }

    m_find_string->selectAll();

	show();
	activateWindow();
}

//-----------------------------------------------------------------------------
