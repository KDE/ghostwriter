/***********************************************************************
 *
 * Copyright (C) 2009, 2010, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) 2014-2022 wereturtle
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

#include "spellchecker.h"

#include <QAction>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QTextBlock>
#include <QTextLayout>

namespace ghostwriter
{

void SpellChecker::checkDocument(QPlainTextEdit *document, Dictionary *dictionary)
{
    SpellChecker *checker = new SpellChecker(document, dictionary);
	checker->m_startCursor = document->textCursor();
	checker->m_cursor = checker->m_startCursor;
	checker->m_cursor.movePosition(QTextCursor::StartOfBlock);
	checker->m_loopAvailable = checker->m_startCursor.block().previous().isValid();
	checker->show();
    checker->check();
}

void SpellChecker::reject()
{
    m_document->setTextCursor(m_startCursor);
	QDialog::reject();
}

void SpellChecker::suggestionChanged(QListWidgetItem *suggestion)
{
	if (suggestion) {
		m_suggestion->setText(suggestion->text());
	}
}

void SpellChecker::add()
{
    m_dictionary->addToPersonal(m_word);

	ignore();
}

void SpellChecker::ignore()
{
	check();
}

void SpellChecker::ignoreAll()
{
	m_ignored.append(m_word);
	ignore();
}

void SpellChecker::change()
{
	m_cursor.insertText(m_suggestion->text());
	check();
}

void SpellChecker::changeAll()
{
	QString replacement = m_suggestion->text();

	QTextCursor cursor = m_cursor;
	cursor.movePosition(QTextCursor::Start);
	forever {
		cursor = m_document->document()->find(m_word, cursor, QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords);
		if (!cursor.isNull()) {
			cursor.insertText(replacement);
		} else {
			break;
		}
	}

	check();
}

SpellChecker::SpellChecker(QPlainTextEdit *document, Dictionary *dictionary) :
	QDialog(document->parentWidget(), Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
	m_dictionary(dictionary),
    m_document(document),
	m_checkedBlocks(1),
	m_totalBlocks(document->document()->blockCount()),
	m_loopAvailable(true)
{
	setWindowTitle(tr("Check Spelling"));
	setWindowModality(Qt::WindowModal);
	setAttribute(Qt::WA_DeleteOnClose);

	// Create widgets
    m_context = new QPlainTextEdit(this);
	m_context->setReadOnly(true);
#if (QT_VERSION_MAJOR == 5) && (QT_VERSION_MINOR < 10)
    m_context->setTabStopWidth(50);
#else
    m_context->setTabStopDistance(50);
#endif
	QPushButton *addButton = new QPushButton(tr("&Add"), this);
	addButton->setAutoDefault(false);
    connect(addButton, &QPushButton::clicked, this, &SpellChecker::add);
	QPushButton *ignoreButton = new QPushButton(tr("&Ignore"), this);
	ignoreButton->setAutoDefault(false);
    connect(ignoreButton, &QPushButton::clicked, this, &SpellChecker::ignore);
	QPushButton *ignoreAllButton = new QPushButton(tr("I&gnore All"), this);
	ignoreAllButton->setAutoDefault(false);
    connect(ignoreAllButton, &QPushButton::clicked, this, &SpellChecker::ignoreAll);

	m_suggestion = new QLineEdit(this);
	QPushButton *changeButton = new QPushButton(tr("&Change"), this);
	changeButton->setAutoDefault(false);
    connect(changeButton, &QPushButton::clicked, this, &SpellChecker::change);
	QPushButton *changeAllButton = new QPushButton(tr("C&hange All"), this);
	changeAllButton->setAutoDefault(false);
    connect(changeAllButton, &QPushButton::clicked, this, &SpellChecker::changeAll);
	m_suggestions = new QListWidget(this);
    connect(m_suggestions, &QListWidget::currentItemChanged, this, &SpellChecker::suggestionChanged);

	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &SpellChecker::reject);

	// Lay out dialog
	QGridLayout *layout = new QGridLayout(this);
	layout->setContentsMargins(12, 12, 12, 12);
	layout->setSpacing(6);
	layout->setColumnMinimumWidth(2, 6);

	layout->addWidget(new QLabel(tr("Not in dictionary:"), this), 0, 0, 1, 2);
	layout->addWidget(m_context, 1, 0, 3, 2);
	layout->addWidget(addButton, 1, 3);
	layout->addWidget(ignoreButton, 2, 3);
	layout->addWidget(ignoreAllButton, 3, 3);

	layout->setRowMinimumHeight(4, 12);

	layout->addWidget(new QLabel(tr("Change to:"), this), 5, 0);
	layout->addWidget(m_suggestion, 5, 1);
	layout->addWidget(m_suggestions, 6, 0, 1, 2);
	layout->addWidget(changeButton, 5, 3);
	layout->addWidget(changeAllButton, 6, 3, Qt::AlignTop);

	layout->setRowMinimumHeight(7, 12);
	layout->addWidget(buttons, 8, 3);
}

void SpellChecker::check()
{
	setDisabled(true);

	QProgressDialog waitDialog(tr("Checking spelling..."), tr("Cancel"), 0, m_totalBlocks, this);
	waitDialog.setWindowTitle(tr("Please wait"));
	waitDialog.setValue(0);
	waitDialog.setWindowModality(Qt::WindowModal);
	bool canceled = false;

	forever {
		// Update wait dialog
		waitDialog.setValue(m_checkedBlocks);
		if (waitDialog.wasCanceled()) {
			canceled = true;
			break;
		}

		// Check current line
		QTextBlock block = m_cursor.block();
		QStringRef word =  m_dictionary->check(block.text(), m_cursor.position() - block.position());
		if (word.isNull()) {
			if (block.next().isValid()) {
                m_cursor.movePosition(QTextCursor::NextBlock);
				++m_checkedBlocks;
				if (m_checkedBlocks < m_totalBlocks) {
					continue;
				} else {
					break;
				}
			} else if (m_loopAvailable) {
				waitDialog.reset();
				if (QMessageBox::question(this, QString(), tr("Continue checking at beginning of file?"),
						QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
					m_loopAvailable = false;
                    m_cursor.movePosition(QTextCursor::Start);
					waitDialog.setRange(0, m_totalBlocks);
					continue;
				} else {
					canceled = true;
					break;
				}
			} else {
				break;
			}
		}

		// Select misspelled word
		m_cursor.setPosition(word.position() + block.position());
		m_cursor.setPosition(m_cursor.position() + word.length(), QTextCursor::KeepAnchor);
		m_word = m_cursor.selectedText();

		if (!m_ignored.contains(m_word)) {
			waitDialog.close();
			setEnabled(true);

            // Show misspelled word in context
            m_context->setPlainText(m_cursor.selectedText());

            QTextCursor contextCursor(m_context->textCursor());

            QTextCursor before = m_cursor;
            before.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor, 10);
            contextCursor.setPosition(0);
            contextCursor.insertText(before.selectedText());

            int wordStart = before.selectedText().length();

            QTextCursor after = m_cursor;
            after.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor, 10);
            contextCursor.setPosition(m_context->document()->characterCount());
            contextCursor.insertText(after.selectedText());

            QTextBlock block = m_context->document()->findBlock(wordStart);
            wordStart -= block.position();

            QTextCharFormat spellingErrorFormat;
            spellingErrorFormat.setForeground(Qt::red);

            QTextLayout::FormatRange range;
            range.start = wordStart;
            range.length = m_word.length();
            range.format = spellingErrorFormat;

            auto formats = block.layout()->formats();
            formats.append(range);
            block.layout()->setFormats(formats);

			// Show suggestions
			m_suggestion->clear();
			m_suggestions->clear();
			QStringList words = m_dictionary->suggestions(m_word);
			if (!words.isEmpty()) {
				for (const QString &word : words) {
					m_suggestions->addItem(word);
				}
				m_suggestions->setCurrentRow(0);
			}

			// Stop checking words
			m_document->setTextCursor(m_cursor);
			m_suggestion->setFocus();
			return;
		}
	}

	// Inform user of completed spell check
	waitDialog.close();

	if (!canceled) {
		QMessageBox::information(this, QString(), tr("Spell check complete."));
	}

	reject();
}

} // namespace ghostwriter
