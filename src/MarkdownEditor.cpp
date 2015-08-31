/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
 * Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) Dmitry Shachnev 2012
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

#include <QTextStream>
#include <QString>
#include <QMimeData>
#include <QScrollBar>
#include <QSyntaxHighlighter>
#include <QTextBoundaryFinder>
#include <QHeaderView>
#include <QMenu>
#include <QChar>
#include <QTimer>
#include <QColor>
#include <QApplication>
#include <QDesktopWidget>
#include <QUrl>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <math.h>

#include "ColorHelper.h"
#include "Theme.h"
#include "ThemeFactory.h"
#include "MarkdownEditor.h"
#include "TextBlockData.h"
#include "MarkdownStates.h"
#include "MarkdownParser.h"
#include "ThemeFactory.h"
#include "GraphicsFadeEffect.h"
#include "spelling/dictionary_ref.h"
#include "spelling/dictionary_manager.h"
#include "spelling/spell_checker.h"


const int HEADING_LEVEL_ROLE = Qt::UserRole + 1;
const int DOCUMENT_POS_ROLE = HEADING_LEVEL_ROLE + 1;

MarkdownEditor::MarkdownEditor
(
    TextDocument* textDocument,
    MarkdownHighlighter* highlighter,
    QWidget* parent
)
    : QPlainTextEdit(parent),
        textDocument(textDocument),
        highlighter(highlighter),
        dictionary(DictionaryManager::instance().requestDictionary()),
        mouseButtonDown(false)
{
    setDocument(textDocument);
    setAcceptDrops(true);

    preferredLayout = new QGridLayout();
    preferredLayout->setSpacing(0);
    preferredLayout->setMargin(0);
    preferredLayout->setContentsMargins(0, 0, 0, 0);
    preferredLayout->addWidget(this, 0, 0);

    blockquoteRegex.setPattern("^ {0,3}(>\\s*)+");
    numberedListRegex.setPattern("^\\s*([0-9]+)[.)]\\s+");
    bulletListRegex.setPattern("^\\s*[+*-]\\s+");
    taskListRegex.setPattern("^\\s*[-] \\[([x ])\\]\\s+");

    this->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    setCursorWidth(2);
    setCenterOnScroll(true);
    ensureCursorVisible();
    spellCheckEnabled = false;
    installEventFilter(this);
    viewport()->installEventFilter(this);
    wordCount = 0;
    lastBlockCount = 1;
    focusMode = FocusModeDisabled;
    insertSpacesForTabs = false;
    setTabulationWidth(4);
    editorWidth = EditorWidthMedium;

    markupPairs.insert('"', '"');
    markupPairs.insert('\'', '\'');
    markupPairs.insert('(', ')');
    markupPairs.insert('[', ']');
    markupPairs.insert('{', '}');
    markupPairs.insert('*', '*');
    markupPairs.insert('_', '_');
    markupPairs.insert('`', '`');
    markupPairs.insert('<', '>');

    connect(this->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChanged(int,int,int)));
    connect(this->document(), SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

    addWordToDictionaryAction = new QAction(tr("Add word to dictionary"), this);
    checkSpellingAction = new QAction(tr("Check spelling..."), this);

    typingPausedSignalSent = true;
    typingHasPaused = true;

    typingTimer = new QTimer(this);
    connect
    (
        typingTimer,
        SIGNAL(timeout()),
        this,
        SLOT(checkIfTypingPaused())
    );
    typingTimer->start(1000);

    Theme theme;
    ThemeFactory::getInstance()->loadLightTheme(theme);
    setColorScheme(theme.markupColorScheme);

    fadeEffect = new GraphicsFadeEffect(this);
    fadeEffect->setFadeHeight(this->fontMetrics().height());
    viewport()->setGraphicsEffect(fadeEffect);
}

MarkdownEditor::~MarkdownEditor()
{

}

void MarkdownEditor::setDictionary(const DictionaryRef& dictionary)
{
    this->dictionary = dictionary;
    this->highlighter->setDictionary(dictionary);
}

QLayout* MarkdownEditor::getPreferredLayout()
{
    return preferredLayout;
}

FocusMode MarkdownEditor::getFocusMode()
{
    return focusMode;
}

void MarkdownEditor::setFocusMode(FocusMode mode)
{
    focusMode = mode;

    if (FocusModeDisabled != mode)
    {
        connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(focusText()));
        connect(this, SIGNAL(selectionChanged()), this, SLOT(focusText()));
        connect(this, SIGNAL(textChanged()), this, SLOT(focusText()));
        focusText();
    }
    else
    {
        disconnect(this, SIGNAL(cursorPositionChanged()), this, SLOT(focusText()));
        disconnect(this, SIGNAL(selectionChanged()), this, SLOT(focusText()));
        disconnect(this, SIGNAL(textChanged()), this, SLOT(focusText()));
        this->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    }
}

void MarkdownEditor::setColorScheme(const MarkdownColorScheme& cs)
{
    colorScheme = cs;
    highlighter->setColorScheme(cs);

    QColor fadedForegroundColor = cs.defaultTextColor;
    fadedForegroundColor.setAlpha(100);

    fadeColor = QBrush(fadedForegroundColor);
    focusText();
}

void MarkdownEditor::setAspect(EditorAspect aspect)
{
    this->aspect = aspect;
}

void MarkdownEditor::setFont(const QString& family, double pointSize)
{
    QFont font(family, pointSize);
    QPlainTextEdit::setFont(font);
    highlighter->setFont(family, pointSize);
    setTabulationWidth(tabWidth);
    fadeEffect->setFadeHeight(this->fontMetrics().height());
}

void MarkdownEditor::setupPaperMargins(int width)
{
    if (EditorWidthFull == editorWidth)
    {
        preferredLayout->setContentsMargins(0, 0, 0, 0);
        setViewportMargins(0, 0, 0, 0);

        return;
    }

    int screenWidth = QApplication::desktop()->screenGeometry().width();
    int proposedEditorWidth = width;
    int margin = 0;

    switch (editorWidth)
    {
        case EditorWidthNarrow:
            proposedEditorWidth = screenWidth / 3;
            break;
        case EditorWidthMedium:
            proposedEditorWidth = screenWidth / 2;
            break;
        case EditorWidthWide:
            proposedEditorWidth = 2 * (screenWidth / 3);
            break;
        default:
            break;
    }

    if (proposedEditorWidth <= width)
    {
        margin = (width - proposedEditorWidth) / 2;
    }

    if (EditorAspectStretch == aspect)
    {
        preferredLayout->setContentsMargins(0, 0, 0, 0);
        setViewportMargins(margin, 0, margin, 0);
    }
    else
    {
        preferredLayout->setContentsMargins(margin, 20, margin, 20);
        setViewportMargins(10, 10, 10, 10);
    }
}

void MarkdownEditor::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void MarkdownEditor::dropEvent(QDropEvent* e)
{
    if (e->mimeData()->hasUrls() && (e->mimeData()->urls().size() == 1))
    {
        e->acceptProposedAction();

        QUrl url = e->mimeData()->urls().first();
        QString path = url.toLocalFile();
        bool isRelativePath = false;

        QFileInfo fileInfo(path);
        QString fileExtension = fileInfo.suffix().toLower();

        // If the file extension indicates an image type, then insert an
        // image link into the text.
        if
        (
            (fileExtension == "jpg") ||
            (fileExtension == "jpeg") ||
            (fileExtension == "gif") ||
            (fileExtension == "bmp") ||
            (fileExtension == "png") ||
            (fileExtension == "tif") ||
            (fileExtension == "tiff") ||
            (fileExtension == "svg")
        )
        {
            if (!textDocument->isNew())
            {
                QFileInfo docInfo(textDocument->getFilePath());

                if (docInfo.exists())
                {
                    path = docInfo.dir().relativeFilePath(path);
                    isRelativePath = true;
                }
            }

            if (!isRelativePath)
            {
                path = url.toString();
            }

            textCursor().insertText(QString("![](%1)").arg(path));

            // We have to call the super class so that clean up occurs,
            // otherwise the editor's cursor will freeze.  We also have to use
            // a dummy drop event with dummy MIME data, otherwise the parent
            // class will insert the file path into the document.
            //
            QMimeData* dummyMimeData = new QMimeData();
            dummyMimeData->setText("");
            QDropEvent* dummyEvent =
                new QDropEvent
                (
                    e->pos(),
                    e->possibleActions(),
                    dummyMimeData,
                    e->mouseButtons(),
                    e->keyboardModifiers()
                );
            QPlainTextEdit::dropEvent(dummyEvent);

            delete dummyEvent;
            delete dummyMimeData;
        }
        // Else insert URL path as normal, using the parent class.
        else
        {
            QPlainTextEdit::dropEvent(e);
        }
    }
}

/*
 * This method contains a code snippet that was lifted and modified from ReText
 */
void MarkdownEditor::keyPressEvent(QKeyEvent* e)
{
    int key = e->key();

    QTextCursor cursor(this->textCursor());

    switch (key)
    {
        case Qt::Key_Return:
            if (!cursor.hasSelection())
            {
                if (e->modifiers() & Qt::ShiftModifier)
                {
                    // Insert Markdown-style line break
                    cursor.insertText("  ");
                }

                if (e->modifiers() & Qt::ControlModifier)
                {
                    cursor.insertText("\n");
                }
                else
                {
                    handleCarriageReturn();
                }
            }
            else
            {
                QPlainTextEdit::keyPressEvent(e);
            }
            break;
        case Qt::Key_Backspace:
            if (!handleBackspaceKey())
            {
                QPlainTextEdit::keyPressEvent(e);
            }
            break;
        case Qt::Key_Tab:
            handleIndent();
            break;
        case Qt::Key_Backtab:
            handleUnindent();
            break;
        case Qt::Key_Greater:
            if (cursor.hasSelection())
            {
                if (e->modifiers() & Qt::ControlModifier)
                {
                    removeBlockquote();
                }
                else
                {
                    insertBlockquote();
                }
            }
            else
            {
                QPlainTextEdit::keyPressEvent(e);
            }
            break;
        case Qt::Key_D:
            if (!(e->modifiers() & Qt::ControlModifier) || !toggleTaskComplete())
            {
                QPlainTextEdit::keyPressEvent(e);
            }
            break;
        default:
            if
            (
                (e->text().size() == 1) &&
                this->markupPairs.contains(e->text().at(0))
            )
            {
                if (!insertPairedCharacters(e->text().at(0)))
                {
                    QPlainTextEdit::keyPressEvent(e);
                }
            }
            else if (e->matches(QKeySequence::Cut))
            {
                cut();
            }
            else if (e->matches(QKeySequence::Bold))
            {
                bold();
            }
            else if (e->matches(QKeySequence::Italic))
            {
                italic();
            }
            else if (e->matches(QKeySequence::Redo) || e->matches(QKeySequence::Undo))
            {
                // Strange things can happen to the word count when an undo or
                // redo operation involving a change in the document's text
                // block count occurs.  Ensure that the editor's word count is
                // refreshed after such operations.
                //
                QPlainTextEdit::keyPressEvent(e);
                refreshWordCount();
            }
            else
            {
                QPlainTextEdit::keyPressEvent(e);
            }
            break;
    }
}

bool MarkdownEditor::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        mouseButtonDown = true;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        mouseButtonDown = false;
    }
    else if (event->type() == QEvent::MouseButtonDblClick)
    {
        mouseButtonDown = true;
    }

    if (event->type() != QEvent::ContextMenu || !spellCheckEnabled || this->isReadOnly())
    {
        return QPlainTextEdit::eventFilter(watched, event);
    }
    else
    {
        // Check spelling of text block under mouse
        QContextMenuEvent* contextEvent = static_cast<QContextMenuEvent*>(event);
        cursorForWord = cursorForPosition(contextEvent->pos());

        QTextCharFormat::UnderlineStyle spellingErrorUnderlineStyle =
            (QTextCharFormat::UnderlineStyle)
            QApplication::style()->styleHint
            (
                QStyle::SH_SpellCheckUnderlineStyle
            );

        // Get the formatting for the cursor position under the mouse,
        // and see if it has the spell check error underline style.
        //
        bool wordHasSpellingError = false;
        int blockPosition = cursorForWord.positionInBlock();
        QList<QTextLayout::FormatRange> formatList =
                textCursor().block().layout()->additionalFormats();
        int mispelledWordStartPos = 0;
        int mispelledWordLength = 0;

        for (int i = 0; i < formatList.length(); i++)
        {
            QTextLayout::FormatRange formatRange = formatList[i];

            if
            (
                (blockPosition >= formatRange.start)
                && (blockPosition <= (formatRange.start + formatRange.length))
                && (formatRange.format.underlineStyle() == spellingErrorUnderlineStyle)
            )
            {
                mispelledWordStartPos = formatRange.start;
                mispelledWordLength = formatRange.length;
                wordHasSpellingError = true;
                break;
            }
        }

        // The word under the mouse is spelled correctly, so use the default
        // processing for the context menu and return.
        //
        if (!wordHasSpellingError)
        {
            return QPlainTextEdit::eventFilter(watched, event);
        }

        // Select the misspelled word.
        cursorForWord.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, blockPosition - mispelledWordStartPos);
        cursorForWord.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, mispelledWordLength);

        wordUnderMouse = cursorForWord.selectedText();
        QStringList suggestions = dictionary.suggestions(wordUnderMouse);
        QMenu* popupMenu = createStandardContextMenu();
        QAction* firstAction = popupMenu->actions().first();

        spellingActions.clear();

        if (!suggestions.empty())
        {
            for (int i = 0; i < suggestions.size(); i++)
            {
                QAction* suggestionAction = new QAction(suggestions[i], this);
                spellingActions.append(suggestionAction);
                popupMenu->insertAction(firstAction, suggestionAction);
            }
        }
        else
        {
            QAction* noSuggestionsAction =
                new QAction(tr("No spelling suggestions found"), this);
            noSuggestionsAction->setEnabled(false);
            spellingActions.append(noSuggestionsAction);
            popupMenu->insertAction(firstAction, noSuggestionsAction);
        }

        popupMenu->insertSeparator(firstAction);
        popupMenu->insertAction(firstAction, addWordToDictionaryAction);
        popupMenu->insertSeparator(firstAction);
        popupMenu->insertAction(firstAction, checkSpellingAction);
        popupMenu->insertSeparator(firstAction);

        // Show menu
        connect(popupMenu, SIGNAL(triggered(QAction*)), this, SLOT(suggestSpelling(QAction*)));
        popupMenu->exec(contextEvent->globalPos());
        delete popupMenu;

        for (int i = 0; i < spellingActions.size(); i++)
        {
            delete spellingActions[i];
        }

        spellingActions.clear();

        return true;
    }
}

void MarkdownEditor::navigateDocument(const int pos)
{
    QTextCursor cursor = this->textCursor();
    cursor.setPosition(pos);
    this->setTextCursor(cursor);
    this->activateWindow();
}

void MarkdownEditor::cut()
{
    QPlainTextEdit::cut();
}

void MarkdownEditor::bold()
{
    insertFormattingMarkup("**");
}

void MarkdownEditor::italic()
{
    insertFormattingMarkup("*");
}

void MarkdownEditor::setEnableLargeHeadingSizes(bool enable)
{
    highlighter->setEnableLargeHeadingSizes(enable);
}

void MarkdownEditor::setUseUnderlineForEmphasis(bool enable)
{
    highlighter->setUseUnderlineForEmphasis(enable);
}

void MarkdownEditor::setInsertSpacesForTabs(bool enable)
{
    insertSpacesForTabs = enable;
}

void MarkdownEditor::setTabulationWidth(int width)
{
    QFontMetrics fontMetrics(font());
    tabWidth = width;
    this->setTabStopWidth(fontMetrics.width(QChar(' ')) * tabWidth);
}

void MarkdownEditor::setEditorWidth(EditorWidth width)
{
    editorWidth = width;
}

void MarkdownEditor::runSpellChecker()
{
    if (this->spellCheckEnabled)
    {
        SpellChecker::checkDocument(this, highlighter, dictionary);
    }
    else
    {
        SpellChecker::checkDocument(this, NULL, dictionary);
    }
}

void MarkdownEditor::setSpellCheckEnabled(const bool enabled)
{
    spellCheckEnabled = enabled;
    highlighter->setSpellCheckEnabled(enabled);
}

void MarkdownEditor::suggestSpelling(QAction* action)
{
    if (action == addWordToDictionaryAction)
    {
        this->setTextCursor(cursorForWord);
        dictionary.addToPersonal(wordUnderMouse);
        this->highlighter->rehighlight();
    }
    else if (action == checkSpellingAction)
    {
        this->setTextCursor(cursorForWord);
        SpellChecker::checkDocument(this, highlighter, dictionary);
    }
    else if (spellingActions.contains(action))
    {
        cursorForWord.insertText(action->text());
    }
}

void MarkdownEditor::onTextChanged(int position, int charsRemoved, int charsAdded)
{
    Q_UNUSED(charsRemoved)

    int startIndex = position;

    if (startIndex < 0)
    {
        startIndex = 0;
    }

    int endIndex = position + charsAdded - 1;

    if (endIndex < startIndex || endIndex >= document()->characterCount())
    {
        endIndex = startIndex;
    }

    // Update the word counts of affected blocks.  Note that there is no need to
    // check for changes to section headings, since the Highlighter class will
    // take care of this for us.
    //
    QTextBlock startBlock = document()->findBlock(startIndex);
    QTextBlock endBlock = document()->findBlock(endIndex);

    QTextBlock block = startBlock;

    updateBlockWordCount(block);

    while (block != endBlock)
    {
        block = block.next();
        updateBlockWordCount(block);
    }

    emit wordCountChanged(wordCount);

    if (typingHasPaused)
    {
        typingHasPaused = false;
        typingPausedSignalSent = false;
        emit typingResumed();
    }
}

void MarkdownEditor::onBlockCountChanged(int newBlockCount)
{
    // If one or more blocks was deleted from the document, update
    // both the word count and the section headings from scratch.
    //
    if (newBlockCount < lastBlockCount)
    {
        refreshWordCount();
    }

    lastBlockCount = newBlockCount;
}

void MarkdownEditor::onSelectionChanged()
{
    QTextCursor cursor = this->textCursor();

    // If text is selected, do a word count of the selected text.
    if (cursor.hasSelection())
    {
        int selectionWordCount = this->countWords(cursor.selectedText());
        emit wordCountChanged(selectionWordCount);
    }
    // Else signal a word count of the full document.
    else
    {
        emit wordCountChanged(wordCount);
    }
}

void MarkdownEditor::focusText()
{
    if (FocusModeDisabled != focusMode)
    {
        QTextEdit::ExtraSelection beforeFadedSelection;
        QTextEdit::ExtraSelection afterFadedSelection;
        beforeFadedSelection.format.setForeground(fadeColor);
        beforeFadedSelection.cursor = this->textCursor();
        afterFadedSelection.format.setForeground(fadeColor);
        afterFadedSelection.cursor = this->textCursor();

        bool canFadePrevious = false;

        QList<QTextEdit::ExtraSelection> selections;

        switch (focusMode)
        {
            case FocusModeCurrentLine: // Current line
                beforeFadedSelection.cursor.movePosition(QTextCursor::StartOfLine);
                canFadePrevious = beforeFadedSelection.cursor.movePosition(QTextCursor::Up);
                beforeFadedSelection.cursor.movePosition(QTextCursor::EndOfLine);
                beforeFadedSelection.cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);

                if (canFadePrevious)
                {
                    selections.append(beforeFadedSelection);
                }

                afterFadedSelection.cursor.movePosition(QTextCursor::EndOfLine);
                afterFadedSelection.cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                selections.append(afterFadedSelection);
                break;

            case FocusModeThreeLines: // Current line and previous two lines
                beforeFadedSelection.cursor.movePosition(QTextCursor::StartOfLine);
                canFadePrevious = beforeFadedSelection.cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 2);
                beforeFadedSelection.cursor.movePosition(QTextCursor::EndOfLine);
                beforeFadedSelection.cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);

                if (canFadePrevious)
                {
                    selections.append(beforeFadedSelection);
                }

                afterFadedSelection.cursor.movePosition(QTextCursor::Down);
                afterFadedSelection.cursor.movePosition(QTextCursor::EndOfLine);
                afterFadedSelection.cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                selections.append(afterFadedSelection);
                break;

            case FocusModeParagraph: // Current paragraph
                canFadePrevious = beforeFadedSelection.cursor.movePosition(QTextCursor::StartOfBlock);
                beforeFadedSelection.cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
                selections.append(beforeFadedSelection);
                afterFadedSelection.cursor.movePosition(QTextCursor::EndOfBlock);
                afterFadedSelection.cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                selections.append(afterFadedSelection);
                break;

            case FocusModeSentence: // Current sentence
            {
                QTextBoundaryFinder boundaryFinder(QTextBoundaryFinder::Sentence, this->textCursor().block().text());
                int currentPos = this->textCursor().positionInBlock();
                int lastSentencePos = 0;
                int nextSentencePos = 0;

                boundaryFinder.setPosition(currentPos);
                lastSentencePos = boundaryFinder.toPreviousBoundary();
                boundaryFinder.setPosition(currentPos);
                nextSentencePos = boundaryFinder.toNextBoundary();

                if (lastSentencePos < 0)
                {
                    beforeFadedSelection.cursor.movePosition(QTextCursor::StartOfBlock);
                }
                else
                {
                    beforeFadedSelection.cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, currentPos - lastSentencePos);
                }

                beforeFadedSelection.cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
                selections.append(beforeFadedSelection);

                if (nextSentencePos < 0)
                {
                    afterFadedSelection.cursor.movePosition(QTextCursor::EndOfBlock);
                }
                else
                {
                    afterFadedSelection.cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nextSentencePos - currentPos);
                }

                afterFadedSelection.cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                selections.append(afterFadedSelection);

                break;
            }

            default:
                break;
        }

        this->setExtraSelections(selections);
    }
}

void MarkdownEditor::checkIfTypingPaused()
{
    if (typingHasPaused && !typingPausedSignalSent)
    {
        typingPausedSignalSent = true;
        emit typingPaused();
    }

    typingHasPaused = true;
}

void MarkdownEditor::spellCheckFinished(int result)
{
    Q_UNUSED(result)

    highlighter->rehighlight();
}

void MarkdownEditor::onCursorPositionChanged()
{
    if (!mouseButtonDown)
    {
        QRect cursor = this->cursorRect();
        QRect viewport = this->viewport()->rect();
        int bottom = viewport.bottom() - this->fontMetrics().height();

        if
        (
            (focusMode != FocusModeDisabled) ||
            (cursor.bottom() >= bottom) ||
            (cursor.top() <= viewport.top())
        )
        {
            centerCursor();
        }
    }

    emit cursorPositionChanged(this->textCursor().position());
}

void MarkdownEditor::refreshWordCount()
{
    // For each block, update the word count.
    QTextBlock block = document()->begin();
    wordCount = 0;

    TextBlockData* blockData = (TextBlockData*) block.userData();

    if (NULL != blockData)
    {
        wordCount += blockData->wordCount;
    }

    while (block != document()->end())
    {
        block = block.next();
        blockData = (TextBlockData*) block.userData();

        if (NULL != blockData)
        {
            wordCount += blockData->wordCount;
        }
    }

    emit wordCountChanged(wordCount);
}

void MarkdownEditor::updateBlockWordCount(QTextBlock& block)
{
    TextBlockData* blockData = (TextBlockData*) block.userData();

    if (NULL == blockData)
    {
        blockData = new TextBlockData();
        block.setUserData(blockData);
    }

    int oldWordCount = blockData->wordCount;
    blockData->wordCount = countWords(block.text());
    wordCount += blockData->wordCount - oldWordCount;
}

void MarkdownEditor::handleCarriageReturn()
{
    QString autoInsertText;

    switch (textCursor().block().userState())
    {
        case MarkdownStateNumberedList:
        {
            autoInsertText = getPriorMarkdownBlockItemStart(numberedListRegex);
            QStringList capture = numberedListRegex.capturedTexts();

            // Auto-increment the list number.
            if (capture.size() == 2)
            {
                QRegExp numberRegex("\\d+");
                int number = capture.at(1).toInt();
                number++;
                autoInsertText =
                    autoInsertText.replace
                    (
                        numberRegex,
                        QString("%1").arg(number)
                    );
            }
            break;
        }
        case MarkdownStateBulletPointList:
            // Check for GFM task list before checking for bullet point.
            autoInsertText = getPriorMarkdownBlockItemStart(taskListRegex);

            // If the string is empty, then it wasn't a GFM task list item.
            // Treat it as a normal bullet point.
            //
            if (autoInsertText.isEmpty())
            {
                autoInsertText = getPriorMarkdownBlockItemStart(bulletListRegex);
            }
            else // string not empty - GFM task list item
            {
                // In case the previous line had a completed task with an X
                // checking it off, make sure a completed task isn't added as
                // the new task (remove the x and replace with a space).
                //
                autoInsertText = autoInsertText.replace('x', ' ');
            }
            break;
        case MarkdownStateBlockquote:
            autoInsertText = getPriorMarkdownBlockItemStart(blockquoteRegex);
            break;
        default:
            autoInsertText = getPriorIndentation();
            break;
    }

    QTextCursor cursor = this->textCursor();
    cursor.insertText(tr("\n") + autoInsertText);
    this->ensureCursorVisible();
}

bool MarkdownEditor::handleBackspaceKey()
{
    QTextCursor cursor = textCursor();

    if (cursor.hasSelection())
    {
        return false;
    }

    int backtrackIndex = -1;

    switch (cursor.block().userState())
    {
        case MarkdownStateNumberedList:
        {
            if (numberedListRegex.exactMatch(textCursor().block().text()))
            {
                backtrackIndex = cursor.block().text().indexOf(QRegExp("\\d"));
            }
            break;
        }
        case MarkdownStateBulletPointList:
            if
            (
                bulletListRegex.exactMatch(cursor.block().text())
                || taskListRegex.exactMatch(cursor.block().text())
            )
            {
                backtrackIndex = cursor.block().text().indexOf(QRegExp("[+*-]"));
            }
            break;
        case MarkdownStateBlockquote:
            if (blockquoteRegex.exactMatch(cursor.block().text()))
            {
                backtrackIndex = cursor.block().text().lastIndexOf('>');
            }
            break;
        default:
            break;
    }

    if (backtrackIndex >= 0)
    {
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition
        (
            QTextCursor::Right,
            QTextCursor::MoveAnchor,
            backtrackIndex
        );

        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        return true;
    }

    return false;
}

// Algorithm lifted from ReText.
void MarkdownEditor::handleIndent()
{
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection())
    {
        QTextBlock block = this->document()->findBlock(cursor.selectionStart());
        QTextBlock end = this->document()->findBlock(cursor.selectionEnd()).next();

        cursor.beginEditBlock();

        while (block != end)
        {
            cursor.setPosition(block.position());

            if (this->insertSpacesForTabs)
            {
                QString indentText = "";

                for (int i = 0; i < tabWidth; i++)
                {
                    indentText += QString(" ");
                }

                cursor.insertText(indentText);
            }
            else
            {
                cursor.insertText("\t");
            }

            block = block.next();
        }

        cursor.endEditBlock();
    }
    else
    {
        int indent = tabWidth;
        QString indentText = "";

        cursor.beginEditBlock();

        switch (cursor.block().userState())
        {
            case MarkdownStateNumberedList:
                if (numberedListRegex.exactMatch(cursor.block().text()))
                {
                    cursor.movePosition(QTextCursor::StartOfBlock);
                }
                break;
            case MarkdownStateBulletPointList:
            {
                if (bulletListRegex.exactMatch(cursor.block().text()))
                {
                    QChar oldBulletPoint = cursor.block().text().trimmed().at(0);
                    QChar newBulletPoint;

                    if (oldBulletPoint == '*')
                    {
                        newBulletPoint = '-';
                    }
                    else if (oldBulletPoint == '-')
                    {
                        newBulletPoint = '+';
                    }
                    else
                    {
                        newBulletPoint = '*';
                    }

                    cursor.movePosition(QTextCursor::StartOfBlock);
                    cursor.movePosition
                    (
                        QTextCursor::EndOfBlock,
                        QTextCursor::KeepAnchor
                    );

                    QString replacementText = cursor.selectedText();
                    replacementText =
                        replacementText.replace
                        (
                            oldBulletPoint,
                            newBulletPoint
                        );
                    cursor.insertText(replacementText);
                    cursor.movePosition(QTextCursor::StartOfBlock);
                }
                else if (taskListRegex.exactMatch(cursor.block().text()))
                {
                    cursor.movePosition(QTextCursor::StartOfBlock);
                }

                break;
            }
            default:
                indent = tabWidth - (cursor.positionInBlock() % tabWidth);
                break;
        }

        if (this->insertSpacesForTabs)
        {
            for (int i = 0; i < indent; i++)
            {
                indentText += QString(" ");
            }
        }
        else
        {
            indentText = "\t";
        }

        cursor.insertText(indentText);
        cursor.endEditBlock();
    }
}

// Algorithm lifted from ReText.
void MarkdownEditor::handleUnindent()
{
    QTextCursor cursor = this->textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection())
    {
        block = this->document()->findBlock(cursor.selectionStart());
        end = this->document()->findBlock(cursor.selectionEnd()).next();
    }
    else
    {
        block = cursor.block();
        end = block.next();
    }

    cursor.beginEditBlock();

    while (block != end)
    {
        cursor.setPosition(block.position());

        if (this->document()->characterAt(cursor.position()) == '\t')
        {
            cursor.deleteChar();
        }
        else
        {
            int pos = 0;

            while
            (
                (this->document()->characterAt(cursor.position()) == ' ')
                && (pos < tabWidth)
            )
            {
                pos += 1;
                cursor.deleteChar();
            }
        }

        block = block.next();
    }

    if
    (
        (MarkdownStateBulletPointList == cursor.block().userState())
        && (bulletListRegex.exactMatch(cursor.block().text()))
    )
    {
        QChar oldBulletPoint = cursor.block().text().trimmed().at(0);
        QChar newBulletPoint;

        if (oldBulletPoint == '*')
        {
            newBulletPoint = '+';
        }
        else if (oldBulletPoint == '-')
        {
            newBulletPoint = '*';
        }
        else
        {
            newBulletPoint = '-';
        }

        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition
        (
            QTextCursor::EndOfBlock,
            QTextCursor::KeepAnchor
        );

        QString replacementText = cursor.selectedText();
        replacementText =
            replacementText.replace
            (
                oldBulletPoint,
                newBulletPoint
            );
        cursor.insertText(replacementText);
    }


    cursor.endEditBlock();
}

// Algorithm lifted from ReText.
void MarkdownEditor::insertBlockquote()
{
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection())
    {
        QTextBlock block = this->document()->findBlock(cursor.selectionStart());
        QTextBlock end = this->document()->findBlock(cursor.selectionEnd()).next();

        cursor.beginEditBlock();

        while (block != end)
        {
            cursor.setPosition(block.position());

            QString text = block.text();

            if (text.indexOf(blockquoteRegex) >= 0)
            {
                cursor.insertText("> ");
            }
            else
            {
                cursor.insertText("> ");
            }

            block = block.next();
        }

        cursor.endEditBlock();
    }
}

// Algorithm lifted from ReText.
void MarkdownEditor::removeBlockquote()
{
    QTextCursor cursor = this->textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection())
    {
        block = this->document()->findBlock(cursor.selectionStart());
        end = this->document()->findBlock(cursor.selectionEnd()).next();
    }
    else
    {
        block = cursor.block();
        end = block.next();
    }

    cursor.beginEditBlock();

    while (block != end)
    {
        cursor.setPosition(block.position());

        if (this->document()->characterAt(cursor.position()) == '>')
        {
            cursor.deleteChar();

            // Delete any spaces that follow the '>' character, to clean up the
            // paragraph.
            //
            while (this->document()->characterAt(cursor.position()) == ' ')
            {
                cursor.deleteChar();
            }
        }

        block = block.next();
    }

    cursor.endEditBlock();
}

bool MarkdownEditor::insertPairedCharacters(const QChar firstChar)
{
    if (markupPairs.contains(firstChar))
    {
        QChar lastChar = markupPairs.value(firstChar);
        QTextCursor cursor = this->textCursor();
        QTextBlock block;
        QTextBlock end;

        if (cursor.hasSelection())
        {
            block = this->document()->findBlock(cursor.selectionStart());
            end = this->document()->findBlock(cursor.selectionEnd());

            if (block == end)
            {
                cursor.setPosition(cursor.selectionStart());
                cursor.beginEditBlock();
                cursor.insertText(firstChar);
                cursor.setPosition(textCursor().selectionEnd());
                cursor.insertText(lastChar);
                cursor.endEditBlock();
                cursor = textCursor();

                cursor.setPosition(cursor.selectionStart());
                cursor.setPosition
                (
                    textCursor().selectionEnd() - 1,
                    QTextCursor::KeepAnchor
                );
                setTextCursor(cursor);
                return true;
            }
        }
    }

    return false;
}

void MarkdownEditor::insertFormattingMarkup(const QString& markup)
{
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection())
    {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd() + markup.length();
        QTextCursor c = cursor;
        c.beginEditBlock();
        c.setPosition(start);
        c.insertText(markup);
        c.setPosition(end);
        c.insertText(markup);
        c.endEditBlock();
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::QTextCursor::KeepAnchor, markup.length());
        this->setTextCursor(cursor);
    }
    else
    {
        // Insert markup twice (for opening and closing around the cursor),
        // and then move the cursor to be between the pair.
        //
        cursor.beginEditBlock();
        cursor.insertText(markup);
        cursor.insertText(markup);
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, markup.length());
        cursor.endEditBlock();
        this->setTextCursor(cursor);
    }
}

bool MarkdownEditor::toggleTaskComplete()
{
    QTextCursor cursor = textCursor();

    if
    (
        (cursor.block().userState() == MarkdownStateBulletPointList)
        && (cursor.block().text().indexOf(taskListRegex) == 0)
    )
    {
        QStringList capture = taskListRegex.capturedTexts();

        if (capture.size() == 2)
        {
            QChar value = capture.at(1)[0];
            QChar replacement;
            int index = cursor.block().text().indexOf("- [");

            if (index >= 0)
            {
                index += 3;
            }

            if (value == 'x')
            {
                replacement = ' ';
            }
            else
            {
                replacement = 'x';
            }

            cursor.beginEditBlock();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition
            (
                QTextCursor::Right,
                QTextCursor::MoveAnchor,
                index
            );

            cursor.deleteChar();
            cursor.insertText(replacement);
            cursor.endEditBlock();
            return true;
        }
    }

    return false;
}

QString MarkdownEditor::getPriorIndentation()
{
    QString indent = tr("");
    QTextCursor cursor = this->textCursor();
    QTextBlock block = cursor.block();

    QString text = block.text();

    for (int i = 0; i < text.length(); i++)
    {
        if (text[i].isSpace())
        {
            indent += text[i];
        }
        else
        {
            return indent;
        }
    }

    return indent;
}

QString MarkdownEditor::getPriorMarkdownBlockItemStart(QRegExp& itemRegex)
{
    QTextCursor cursor = this->textCursor();
    QTextBlock block = cursor.block();

    QString text = block.text();

    if (itemRegex.indexIn(text, 0) >= 0)
    {
        return text.left(itemRegex.matchedLength());
    }

    return QString("");
}

int MarkdownEditor::countWords(const QString& text)
{
    int count = 0;
    bool inWord = false;
    int separatorCount = 0;

    for (int i = 0; i < text.length(); i++)
    {
        if (text[i].isLetterOrNumber())
        {
            inWord = true;
            separatorCount = 0;
        }
        else if (text[i].isSpace() && inWord)
        {
            inWord = false;
            count++;
            separatorCount = 0;
        }
        else
        {
            // This is to handle things like double dashes (`--`)
            // that separate words, while still counting hyphenated
            // words as a single word.
            //
            separatorCount++;

            if (separatorCount > 1)
            {
                separatorCount = 0;
                inWord = false;
                count++;
            }
        }
    }

    if (inWord)
    {
        count++;
    }

    return count;
}
