/*
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <algorithm>

#include <QString>
#include <QTextBlock>
#include <QTextCursor>

#include <Sonnet/BackgroundChecker>
#include <Sonnet/Dialog>

#include "../messageboxhelper.h"
#include "spellcheckdialog.h"

namespace ghostwriter
{
typedef QPair<int, int> TextRange;

class SpellCheckDialogPrivate
{
    Q_DECLARE_PUBLIC(SpellCheckDialog)

public:

    SpellCheckDialogPrivate(SpellCheckDialog *q_ptr)
        : q_ptr(q_ptr) { }
    ~SpellCheckDialogPrivate() { }

    SpellCheckDialog *q_ptr;

    QPlainTextEdit *editor;
    QTextCursor cursor;
    int startPosition;
    bool wrappedAround;

    TextRange initialSelection;
    
    void spellCheckDone(const QString &newBuffer);
    void misspelling(const QString &word, int start);
    void replace(const QString &oldWord, int start, const QString &newWord);
};

SpellCheckDialog::SpellCheckDialog(QPlainTextEdit *editor)
    : Sonnet::Dialog(new Sonnet::BackgroundChecker(editor), editor),
      d_ptr(new SpellCheckDialogPrivate(this))
{
    Q_D(SpellCheckDialog);

    this->setAttribute(Qt::WA_DeleteOnClose);

    QString buffer;

    d->wrappedAround = false;
    d->editor = editor;
    d->cursor = d->editor->textCursor();
    d->initialSelection.first = d->cursor.selectionStart();
    d->initialSelection.second = d->cursor.selectionEnd();

    if (!d->cursor.hasSelection()) {
        d->startPosition = 0;
        buffer = d->editor->toPlainText();
    } else {
        int start = d->cursor.selectionStart();
        int end = d->cursor.selectionEnd();

        if (start > end) {
            std::swap(start, end);
        }

        d->cursor.setPosition(start);
        d->cursor.movePosition(QTextCursor::StartOfWord);
        start = d->cursor.position();

        d->cursor.setPosition(end);
        d->cursor.movePosition(QTextCursor::EndOfWord);
        end = d->cursor.position();

        d->cursor.setPosition(start);
        d->cursor.movePosition(
            QTextCursor::NextCharacter,
            QTextCursor::KeepAnchor,
            (end - start + 1));

        buffer = d->cursor.selectedText();
        d->startPosition = start;
    }

    this->connect(
        this,
        &Sonnet::Dialog::spellCheckDone,
        [d](const QString &newBuffer) {
            d->spellCheckDone(newBuffer);
        }
    );

    this->connect(
        this,
        &Sonnet::Dialog::misspelling,
        [d](const QString &word, int start) {
            d->misspelling(word, start);
        }
    );

    this->connect(
        this,
        &Sonnet::Dialog::replace,
        [d](const QString &oldWord, int start, const QString &newWord) {
            d->replace(oldWord, start, newWord);
        }
    );

    this->setBuffer(buffer);
}

SpellCheckDialog::~SpellCheckDialog()
{
    ;
}

void SpellCheckDialogPrivate::spellCheckDone(const QString &newBuffer)
{
    Q_Q(SpellCheckDialog);

    Q_UNUSED(newBuffer)

    // Set the cursor to its original position before spell check began.
    this->cursor.setPosition(this->initialSelection.first);

    // Check if text was selected before spell check began.  If so, ensure
    // text is selected again.
    int rangeLen = std::abs(this->initialSelection.second
        - this->initialSelection.first);
    QTextCursor::MoveOperation op = QTextCursor::NextCharacter;

    if (this->initialSelection.first > this->initialSelection.second) {
        op = QTextCursor::PreviousCharacter;
    }

    if (rangeLen > 0) {
        this->cursor.movePosition(op, QTextCursor::KeepAnchor, rangeLen);
    }

    this->editor->setTextCursor(this->cursor);

    MessageBoxHelper::information(
        this->editor,
        QString(),
        SpellCheckDialog::tr("Spell check is complete!")
    );
}

void SpellCheckDialogPrivate::misspelling(const QString &word, int start)
{
    // Select the misspelled word in the editor.
    this->cursor.setPosition(this->startPosition + start);
    this->cursor.movePosition(
        QTextCursor::NextCharacter,
        QTextCursor::KeepAnchor,
        word.length());
    this->editor->setTextCursor(this->cursor);
}

void SpellCheckDialogPrivate::replace(
    const QString &oldWord,
    int start,
    const QString &newWord)
{
    int wordPos = this->startPosition + start;

    this->cursor.setPosition(wordPos);
    this->cursor.movePosition(
        QTextCursor::NextCharacter,
        QTextCursor::KeepAnchor,
        oldWord.length());

    // In case dialog is not modal, ensure the misspelled word was not changed
    // by the user already.
    if (oldWord == this->cursor.selectedText()) {
        this->cursor.insertText(newWord);

        // Recalculate original text cursor position and/or selection now that
        // the misspelled word is replaced.
        int len = std::min(oldWord.length(), newWord.length());
        int tail = (wordPos + len);
        int offset = newWord.length() - oldWord.length();

        if (initialSelection.first > tail) {
            initialSelection.first += offset;
        }
        
        if (initialSelection.second > tail) {
            initialSelection.second += offset;
        }
    }
}

} // namespace ghostwriter