/***********************************************************************
 *
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

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QStringList>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextLayout>
#include <QTimer>
#include <QVector>

#include "spellcheckdecorator.h"

#include "dictionary.h"
#include "dictionarymanager.h"
#include "spellchecker.h"

namespace ghostwriter
{

class SpellCheckDecoratorPrivate
{
    Q_DECLARE_PUBLIC(SpellCheckDecorator)

public:
    SpellCheckDecoratorPrivate(SpellCheckDecorator *decorator)
    : q_ptr(decorator),
      spellCheckEnabled(true),
      dictionary(DictionaryManager::instance()->requestDictionary())
    {
        ;
    }
    ~SpellCheckDecoratorPrivate() { }

    SpellCheckDecorator *q_ptr;
    QPlainTextEdit *editor;
    bool spellCheckEnabled;
    Dictionary *dictionary;
    QColor errorColor;

    QMenu * createContextMenu(const QTextCursor &cursorForWord) const;

    QMenu * createSpellingMenu(
        const QString &misspelledWord,
        const QTextCursor &cursorForWord) const;

    QString getMisspelledWordAtCursor(QTextCursor &cursorForWord) const;

    void onContentsChanged(int position, int charsAdded, int charsRemoved);
    void spellCheckBlock(QTextBlock &block) const;
    void clearSpellCheckFormatting(QTextBlock &block) const;
    void resetLiveSpellChecking() const;

};

SpellCheckDecorator::SpellCheckDecorator(QPlainTextEdit *editor)
: QObject(editor),
  d_ptr(new SpellCheckDecoratorPrivate(this))
{
    Q_D(SpellCheckDecorator);

    d->spellCheckEnabled = true;
    d->editor = editor;

    Q_ASSERT(nullptr != d->editor);

    d->editor->installEventFilter(this);
    d->editor->viewport()->installEventFilter(this);

    connect(d->editor->document(),
        static_cast<void (QTextDocument::*)(int, int, int)>(&QTextDocument::contentsChange),
        this,
        [d](int position, int charsAdded, int charsRemoved) {
            d->onContentsChanged(position, charsAdded, charsRemoved);
        }
    );
}

SpellCheckDecorator::~SpellCheckDecorator()
{
    ;
}

bool SpellCheckDecorator::liveSpellCheckEnabled() const
{
    Q_D(const SpellCheckDecorator);

    return d->spellCheckEnabled;
}

QColor SpellCheckDecorator::errorColor() const
{
    Q_D(const SpellCheckDecorator);

    return d->errorColor;
}

void SpellCheckDecorator::setLiveSpellCheckEnabled(bool enabled)
{
    Q_D(SpellCheckDecorator);

    if (d->spellCheckEnabled != enabled) {
        d->spellCheckEnabled = enabled;
        d->resetLiveSpellChecking();
    }
}

void SpellCheckDecorator::setDictionary(const QString &language)
{
    Q_D(SpellCheckDecorator);

    d->dictionary = DictionaryManager::instance()->requestDictionary(language);
}

void SpellCheckDecorator::setErrorColor(const QColor &color)
{
    Q_D(SpellCheckDecorator);

    d->errorColor = color;
    d->resetLiveSpellChecking();
}

void SpellCheckDecorator::runSpellCheck()
{
    Q_D(SpellCheckDecorator);

    SpellChecker::checkDocument(d->editor, d->dictionary);

    if (d->spellCheckEnabled) {
        d->resetLiveSpellChecking();
    }
}

void SpellCheckDecorator::startLiveSpellCheck()
{
    Q_D(SpellCheckDecorator);

    if (!d->spellCheckEnabled) {
        return;
    }

    QTextBlock block = d->editor->document()->begin();

    while (block.isValid()) {
        d->spellCheckBlock(block);
        block = block.next();
    }
}

bool SpellCheckDecorator::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(SpellCheckDecorator);
    Q_UNUSED(watched);

    if ((event->type() != QEvent::ContextMenu)
            || !d->spellCheckEnabled
            || d->editor->isReadOnly()) {
        return false;
    }

    // Check spelling of text block under mouse or at cursor position.
    QContextMenuEvent *contextEvent = static_cast<QContextMenuEvent *>(event);

    QTextCursor cursorForWord;

    // If the context menu event was triggered by pressing the menu key, use the
    // current text cursor rather than the event position to get a cursor
    // position, since the event position is the mouse position rather than the
    // text cursor position.
    if (QContextMenuEvent::Keyboard == contextEvent->reason()) {
        cursorForWord = d->editor->textCursor();
    }
    // Else process as mouse event.
    else {
        cursorForWord = d->editor->cursorForPosition(contextEvent->pos());
    }

    QMenu *popupMenu = d->createContextMenu(cursorForWord);
    QString misspelledWord = d->getMisspelledWordAtCursor(cursorForWord);

    // If the selected word is spelled correctly, use the default processing for
    // the context menu.
    if (!misspelledWord.isNull() && !misspelledWord.isEmpty()) {
        popupMenu->addMenu(d->createSpellingMenu(misspelledWord, cursorForWord));
    }

    // Show context menu
    QPoint menuPos;

    // If event was triggered by a key press, use the text cursor coordinates to
    // display the popup menu.
    if (QContextMenuEvent::Keyboard == contextEvent->reason()) {
        QRect cr = d->editor->cursorRect();
        menuPos.setX(cr.x());
        menuPos.setY(cr.y() + (cr.height() / 2));
        menuPos = d->editor->viewport()->mapToGlobal(menuPos);
    }
    // Else use the mouse coordinates from the context menu event.
    else {
        menuPos = d->editor->viewport()->mapToGlobal(contextEvent->pos());
    }

    popupMenu->exec(menuPos);

    if (nullptr != popupMenu) {
        delete popupMenu;
        popupMenu = nullptr;
    }

    return true;
}

QMenu * SpellCheckDecoratorPrivate::createContextMenu(
    const QTextCursor &cursorForWord) const
{
    Q_Q(const SpellCheckDecorator);

    // Add spell check action to the standard context menu that comes with
    // the editor.
    QMenu *popupMenu = this->editor->createStandardContextMenu();

    QAction *checkSpellingAction =
        new QAction(q->tr("Check spelling..."), popupMenu);

    q->connect(checkSpellingAction,
        &QAction::triggered,
        q,
        [this, cursorForWord]() {
            this->editor->setTextCursor(cursorForWord);
            SpellChecker::checkDocument(this->editor, this->dictionary);
        }
    );

    popupMenu->addAction(checkSpellingAction);

    return popupMenu;
}

QMenu * SpellCheckDecoratorPrivate::createSpellingMenu(
    const QString &misspelledWord,
    const QTextCursor &cursorForWord) const
{
    Q_Q(const SpellCheckDecorator);

    QMenu *spellingMenu = new QMenu(q->tr("Spelling"));
    QStringList suggestions = this->dictionary->suggestions(misspelledWord);

    QAction *addWordToDictionaryAction =
        new QAction(q->tr("Add word to dictionary"), spellingMenu);

    q->connect(addWordToDictionaryAction,
        &QAction::triggered,
        q,
        [this, cursorForWord, misspelledWord]() {
            this->editor->setTextCursor(cursorForWord);
            this->dictionary->addToPersonal(misspelledWord);

            resetLiveSpellChecking();
        }
    );

    spellingMenu->addAction(addWordToDictionaryAction);
    spellingMenu->addSeparator();

    if (!suggestions.empty()) {
        // Add suggested spellings to the popup menu.
        for (const QString &suggestion : suggestions) {
            QAction *suggestionAction = new QAction(suggestion, spellingMenu);

            // If a suggested spelling is selected from the popup menu,
            // replace the misspelled word selection with the new spelling.
            q->connect(suggestionAction,
                &QAction::triggered,
                [cursorForWord, suggestionAction]() {
                    QTextCursor cursor(cursorForWord);
                    cursor.insertText(suggestionAction->data().toString());
                }
            );

            // Need the following line because KDE Plasma 5 will insert a hidden
            // ampersand into the menu text as a keyboard accelerator.  Go off
            // of the data in the QAction rather than the text to avoid this.
            suggestionAction->setData(suggestion);

            // Add suggested spelling action to the popup menu.
            spellingMenu->addAction(suggestionAction);
        }
    } else {
        QAction *noSuggestionsAction =
            new QAction(q->tr("No spelling suggestions found"), spellingMenu);
        noSuggestionsAction->setEnabled(false);
        spellingMenu->addAction(noSuggestionsAction);
    }

    return spellingMenu;
}


QString SpellCheckDecoratorPrivate::getMisspelledWordAtCursor(
    QTextCursor &cursorForWord) const
{
    bool wordHasSpellingError = false;
    int blockPosition = cursorForWord.positionInBlock();
    int misspelledWordStartPos = 0;
    int misspelledWordLength = 0;

    // Look for formatting for the word at the provided cursor position that
    // matches the spell check error underline style.  If the word has that
    // style, then it is misspelled.
    for (const QTextLayout::FormatRange &formatRange : cursorForWord.block().layout()->formats()) {
        if ((blockPosition >= formatRange.start)
                && (blockPosition <= (formatRange.start + formatRange.length))
                && (formatRange.format.underlineStyle()
                    == QTextCharFormat::SpellCheckUnderline)) {
            misspelledWordStartPos = formatRange.start;
            misspelledWordLength = formatRange.length;
            wordHasSpellingError = true;
            break;
        }
    }

    // Return null string if word is not misspelled.
    if (!wordHasSpellingError) {
        return QString();
    }

    // Select the misspelled word.
    cursorForWord.movePosition(
        QTextCursor::PreviousCharacter,
        QTextCursor::MoveAnchor,
        blockPosition - misspelledWordStartPos
    );

    cursorForWord.movePosition(
        QTextCursor::NextCharacter,
        QTextCursor::KeepAnchor,
        misspelledWordLength
    );

    return cursorForWord.selectedText();
}

void SpellCheckDecoratorPrivate::onContentsChanged(
    int position,
    int charsAdded,
    int charsRemoved)
{
    if (!this->spellCheckEnabled) {
        return;
    }

    QTextBlock firstBlock = editor->document()->findBlock(position);
    QTextBlock lastBlock = editor->document()->findBlock(position
                            + charsAdded + charsRemoved);

    if (!firstBlock.isValid()) {
        return;
    }

    QTextBlock block = firstBlock;
    int endPosition;

    if (lastBlock.isValid()) {
        endPosition = lastBlock.position() + lastBlock.length();
    }
    else {
        endPosition = editor->document()->characterCount() - 1;
    }

    while (block.isValid() && (block.position() < endPosition)) {
        spellCheckBlock(block);
        block = block.next();
    }
}

void SpellCheckDecoratorPrivate::spellCheckBlock(QTextBlock &block) const
{
    QStringRef misspelledWord = dictionary->check(block.text(), 0);

    while (!misspelledWord.isNull()) {
        int startIndex = misspelledWord.position();
        int length = misspelledWord.length();

        QTextCharFormat spellingErrorFormat;
        spellingErrorFormat.setUnderlineColor(Qt::red);
        spellingErrorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

        QTextLayout::FormatRange range;
        range.start = startIndex;
        range.length = length;
        range.format = spellingErrorFormat;

        auto formats = block.layout()->formats();
        formats.append(range);
        block.layout()->setFormats(formats);

        startIndex += length;
        misspelledWord = dictionary->check(block.text(), startIndex);
    }
}

void SpellCheckDecoratorPrivate::clearSpellCheckFormatting(QTextBlock &block) const
{
    QVector<QTextLayout::FormatRange> formats;

    for (const QTextLayout::FormatRange &format : block.layout()->formats()) {
        if (QTextCharFormat::SpellCheckUnderline != format.format.underlineStyle()) {
            formats.append(format);
        }
    }

    block.layout()->setFormats(formats);
}

void SpellCheckDecoratorPrivate::resetLiveSpellChecking() const
{
    QTextBlock block = editor->document()->begin();

    while (block.isValid()) {
        clearSpellCheckFormatting(block);

        if (spellCheckEnabled) {
            spellCheckBlock(block);
        }

        block = block.next();
    }
}

} // namespace ghostwriter
