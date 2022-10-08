/*
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-FileCopyrightText: 2006 Jacob R Rideout <kde@jacobrideout.net>
   SPDX-FileCopyrightText: 2006 Martin Sandsmark <martin.sandsmark@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QAction>
#include <QContextMenuEvent>
#include <QList>
#include <QMenu>
#include <QStringList>
#include <QStringRef>
#include <QTextBlock>
#include <QTextBoundaryFinder> 
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextLayout>
#include <QTimer>
#include <QVector>

#include <Sonnet/BackgroundChecker>
#include <Sonnet/Dialog>
#include <Sonnet/GuessLanguage>
#include <Sonnet/Settings>
#include <Sonnet/Speller>

#include "spellcheckdecorator.h"
#include "spellcheckdialog.h"

namespace ghostwriter
{
class SpellCheckDecoratorPrivate
{
    Q_DECLARE_PUBLIC(SpellCheckDecorator)

public:
    struct Position {
        int start, length;
    };

    /**
     * This structure abstracts the positions of breaks in the test. As per the
     * unicode annex, both the start and end of the text are returned.
     */
    typedef QList<Position> Positions;

    SpellCheckDecoratorPrivate(SpellCheckDecorator *decorator)
    : q_ptr(decorator)
    {
        ;
    }
    ~SpellCheckDecoratorPrivate() { }

    static Sonnet::Settings *settings;

    SpellCheckDecorator *q_ptr;
    QPlainTextEdit *editor;
    Sonnet::Speller *speller;
    QColor errorColor;
    SpellCheckDialog *spellCheckDialog;

    QMenu * createContextMenu();

    QMenu * createSpellingMenu(
        const QString &misspelledWord,
        const QTextCursor &cursorForWord) const;

    QString getMisspelledWordAtCursor(QTextCursor &cursorForWord) const;

    void onContentsChanged(int position, int charsAdded, int charsRemoved);
    void spellCheckBlock(QTextBlock &block) const;
    void clearSpellCheckFormatting(QTextBlock &block) const;
    void resetLiveSpellChecking() const;
    Positions wordBreaks(const QString &text) const;
    Positions sentenceBreaks(const QString &text) const;

};

Sonnet::Settings *SpellCheckDecoratorPrivate::settings = nullptr;

SpellCheckDecorator::SpellCheckDecorator(QPlainTextEdit *editor)
: QObject(editor),
  d_ptr(new SpellCheckDecoratorPrivate(this))
{
    Q_D(SpellCheckDecorator);

    if (nullptr == d->settings) {
        d->settings = new Sonnet::Settings();
    }
    
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

    d->speller = new Sonnet::Speller();
}

SpellCheckDecorator::~SpellCheckDecorator()
{
    ;
}

QColor SpellCheckDecorator::errorColor() const
{
    Q_D(const SpellCheckDecorator);

    return d->errorColor;
}

void SpellCheckDecorator::setErrorColor(const QColor &color)
{
    Q_D(SpellCheckDecorator);

    d->errorColor = color;
    d->resetLiveSpellChecking();
}

void SpellCheckDecorator::settingsChanged()
{
    Q_D(SpellCheckDecorator);

    if (d->settings) {
        delete d->settings;
        d->settings = new Sonnet::Settings(this);
    }

    d->speller->setLanguage(d->settings->defaultLanguage());
    d->resetLiveSpellChecking();
}

void SpellCheckDecorator::startLiveSpellCheck()
{
    Q_D(SpellCheckDecorator);

    if (!d->settings->checkerEnabledByDefault()) {
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
            || !d->settings->checkerEnabledByDefault()
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

    QMenu *popupMenu = d->createContextMenu();
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

QMenu * SpellCheckDecoratorPrivate::createContextMenu()
{
    Q_Q(SpellCheckDecorator);

    // Add spell check action to the standard context menu that comes with
    // the editor.
    QMenu *popupMenu = this->editor->createStandardContextMenu();

    QAction *checkSpellingAction =
        new QAction(SpellCheckDecorator::tr("Check spelling..."), popupMenu);

    q->connect(checkSpellingAction,
        &QAction::triggered,
        q,
        [this, q]() {
            spellCheckDialog = new SpellCheckDialog(this->editor);
            spellCheckDialog->show();
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
    QMenu *spellingMenu = new QMenu(SpellCheckDecorator::tr("Spelling"));

    if (this->settings->autodetectLanguage())  {
        QString text = cursorForWord.block().text();
        int pos = cursorForWord.position() - cursorForWord.block().position();

        for (auto sentenceBreak : sentenceBreaks(text)) {
            if (pos < (sentenceBreak.start + sentenceBreak.length)) {
                QString sentence = text.mid(
                    sentenceBreak.start,
                    sentenceBreak.length);
                QString language = Sonnet::GuessLanguage().identify(sentence);

                if (!language.isNull()) {
                    this->speller->setLanguage(language);
                }

                break;
            }
        }
    }

    QStringList suggestions = this->speller->suggest(misspelledWord);

    QAction *addWordToDictionaryAction =
        new QAction(SpellCheckDecorator::tr("Add word to dictionary"), spellingMenu);

    q->connect(addWordToDictionaryAction,
        &QAction::triggered,
        [this, cursorForWord, misspelledWord]() {
            this->editor->setTextCursor(cursorForWord);
            this->speller->addToPersonal(misspelledWord);

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
            new QAction(SpellCheckDecorator::tr("No spelling suggestions found"), spellingMenu);
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
    if (!this->settings->checkerEnabledByDefault()) {
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
    QString text = block.text();

    for (auto sentenceSegment : sentenceBreaks(text)) {
        QString sentence =
            text.mid(sentenceSegment.start, sentenceSegment.length);

        if (this->settings->autodetectLanguage()) {
            QString language = Sonnet::GuessLanguage().identify(sentence);

            if (!language.isNull()) {
                this->speller->setLanguage(language);
            } else {
                this->speller->setLanguage(this->settings->defaultLanguage());
            }
        }

        for (auto wordSegment : wordBreaks(sentence)) {
            int wordStart = sentenceSegment.start + wordSegment.start;
            QString word = text.mid(wordStart, wordSegment.length);

            if (speller->isMisspelled(word)) {
                QTextCharFormat spellingErrorFormat;
                spellingErrorFormat.setUnderlineColor(this->errorColor);
                spellingErrorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

                QTextLayout::FormatRange range;
                range.start = wordStart;
                range.length = wordSegment.length;
                range.format = spellingErrorFormat;

                auto formats = block.layout()->formats();
                formats.append(range);
                block.layout()->setFormats(formats);
            }
        }
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

        if (this->settings->checkerEnabledByDefault()) {
            spellCheckBlock(block);
        }

        block = block.next();
    }
}

// Code is lifted from KDE Frameworks' Sonnet library, because we know it
// just works.  :)
SpellCheckDecoratorPrivate::Positions SpellCheckDecoratorPrivate::wordBreaks(const QString &text) const
{
    Positions breaks;

    if (text.isEmpty()) {
        return breaks;
    }

    QTextBoundaryFinder boundaryFinder(QTextBoundaryFinder::Word, text);

    while (boundaryFinder.position() < text.length()) {
        if (!(boundaryFinder.boundaryReasons().testFlag(QTextBoundaryFinder::StartOfItem))) {
            if (boundaryFinder.toNextBoundary() == -1) {
                break;
            }
            continue;
        }

        Position pos;
        pos.start = boundaryFinder.position();
        int end = boundaryFinder.toNextBoundary();
        if (end == -1) {
            break;
        }
        pos.length = end - pos.start;
        if (pos.length < 1) {
            continue;
        }
        breaks.append(pos);

        if (boundaryFinder.toNextBoundary() == -1) {
            break;
        }
    }
    return breaks;
}

// Code is lifted from KDE Frameworks' Sonnet library, because we know it
// just works.  :)
SpellCheckDecoratorPrivate::Positions
SpellCheckDecoratorPrivate::sentenceBreaks(const QString &text) const
{
    Positions breaks;

    if (text.isEmpty()) {
        return breaks;
    }

    QTextBoundaryFinder boundaryFinder(QTextBoundaryFinder::Sentence, text);

    while (boundaryFinder.position() < text.length()) {
        Position pos;
        pos.start = boundaryFinder.position();
        int end = boundaryFinder.toNextBoundary();
        if (end == -1) {
            break;
        }
        pos.length = end - pos.start;
        if (pos.length < 1) {
            continue;
        }
        breaks.append(pos);
    }
    return breaks;
}

} // namespace ghostwriter
