/***********************************************************************
 *
 * Copyright (C) 2014-2017 wereturtle
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
#include <QFileInfo>
#include <QDir>

#include "ColorHelper.h"
#include "GraphicsFadeEffect.h"
#include "MarkdownEditor.h"
#include "MarkdownStates.h"
#include "MarkdownTokenizer.h"
#include "MarkdownHighlighter.h"
#include "spelling/dictionary_ref.h"
#include "spelling/dictionary_manager.h"
#include "spelling/spell_checker.h"

#define GW_TEXT_FADE_FACTOR 1.5

MarkdownEditor::MarkdownEditor
(
    TextDocument* textDocument,
    QWidget* parent
)
    : QPlainTextEdit(parent),
        textDocument(textDocument),
        dictionary(DictionaryManager::instance().requestDictionary()),
        autoMatchEnabled(true),
        bulletPointCyclingEnabled(true),
        mouseButtonDown(false)
{
    setDocument(textDocument);

    highlighter = new MarkdownHighlighter(this);

    setAcceptDrops(true);

    preferredLayout = new QGridLayout();
    preferredLayout->setSpacing(0);
    preferredLayout->setMargin(0);
    preferredLayout->setContentsMargins(0, 0, 0, 0);
    preferredLayout->addWidget(this, 0, 0);

    blockquoteRegex.setPattern("^ {0,3}(>\\s*)+");
    numberedListRegex.setPattern("^\\s*([0-9]+)[.)]\\s+");
    bulletListRegex.setPattern("^\\s*[+*-]\\s+");
    taskListRegex.setPattern("^\\s*[-*+] \\[([x ])\\]\\s+");

    this->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Make sure QPlainTextEdit does not draw a cursor.  (We'll paint it manually.)
    setCursorWidth(0);
    
    setCenterOnScroll(true);
    ensureCursorVisible();
    spellCheckEnabled = false;
    installEventFilter(this);
    viewport()->installEventFilter(this);
    hemingwayModeEnabled = false;
    focusMode = FocusModeDisabled;
    insertSpacesForTabs = false;
    setTabulationWidth(4);
    editorWidth = EditorWidthMedium;
    editorCorners = InterfaceStyleRounded;

    markupPairs.insert('"', '"');
    markupPairs.insert('\'', '\'');
    markupPairs.insert('(', ')');
    markupPairs.insert('[', ']');
    markupPairs.insert('{', '}');
    markupPairs.insert('*', '*');
    markupPairs.insert('_', '_');
    markupPairs.insert('`', '`');
    markupPairs.insert('<', '>');

    // Set automatching for the above markup pairs to be
    // enabled by default.
    //
    autoMatchFilter.insert('"', true);
    autoMatchFilter.insert('\'', true);
    autoMatchFilter.insert('(', true);
    autoMatchFilter.insert('[', true);
    autoMatchFilter.insert('{', true);
    autoMatchFilter.insert('*', true);
    autoMatchFilter.insert('_', true);
    autoMatchFilter.insert('`', true);
    autoMatchFilter.insert('<', true);

    nonEmptyMarkupPairs.insert('*', '*');
    nonEmptyMarkupPairs.insert('_', '_');
    nonEmptyMarkupPairs.insert('<', '>');

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    connect(this->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onContentsChanged(int,int,int)));
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

    setColorScheme
    (
        QColor(Qt::black),
        QColor(Qt::white),
        QColor(Qt::black),
        QColor(Qt::blue),
        QColor(Qt::black),
        QColor(Qt::black),
        QColor(Qt::black),
        QColor(Qt::black),
        QColor(Qt::red)
    );

    fadeEffect = new GraphicsFadeEffect(this);
    fadeEffect->setFadeHeight(this->fontMetrics().height() * GW_TEXT_FADE_FACTOR);

#if QT_VERSION >= 0x050600
    qreal dpr = devicePixelRatio();

    if (dpr <= 1.0)
    {
        viewport()->setGraphicsEffect(fadeEffect);
    }
#else
    viewport()->setGraphicsEffect(fadeEffect);
#endif

    
    textCursorVisible = true;
    
    cursorBlinkTimer = new QTimer(this);
    connect(cursorBlinkTimer, SIGNAL(timeout()), this, SLOT(toggleCursorBlink()));
    cursorBlinkTimer->start(500);
}

MarkdownEditor::~MarkdownEditor()
{

}

void MarkdownEditor::paintEvent(QPaintEvent* event)
{
    QPainter painter(viewport());
    QRect viewportRect = viewport()->rect();
    painter.fillRect(viewportRect, Qt::transparent);

    QPointF offset(contentOffset());
    QTextBlock block = firstVisibleBlock();

    bool firstVisible = true;

    QRectF blockAreaRect; // Code or block quote rect.
    bool inBlockArea = false;
    BlockType blockType = BlockTypeNone;
    bool clipTop = false;
    bool drawBlock = false;
    int dy = 0;
    bool done = false;

    int cornerRadius = 5;

    if (InterfaceStyleSquare == editorCorners)
    {
        cornerRadius = 0;
    }

    // Draw text block area backgrounds for code blocks and block quotes.
    // The backgrounds are drawn per each block area (consisting of multiple
    // text blocks or lines), rather than one rectangle area per text block/
    // line in case there are margins between each text block.  This way,
    // the background will extend to cover the margins between text blocks
    // as well.
    //
    // NOTE: Algorithm for looping through text blocks is a partial lift from
    //       Qt's QPlainTextEdit paintEvent() code. Please refer to the
    //       LGPL v. 3 license for the original Qt code.
    //
    while (block.isValid() && !done)
    {
        QRectF r = blockBoundingRect(block).translated(offset);

        // If the block begins a new text block area...
        if (!inBlockArea && atBlockAreaStart(block, blockType))
        {
            blockAreaRect = r;
            dy = 0;
            inBlockArea = true;

            BlockType prevType;

            // If this is the first visible block within the viewport
            // and if the previous block is part of the text block area,
            // then the rectangle to draw for the block area will have
            // its top clipped by the viewport and will need to be
            // drawn specially.
            //
            if
            (
                firstVisible
                && atBlockAreaStart(block.previous(), prevType)
                && (blockType == prevType)
            )
            {
                clipTop = true;
            }
        }
        // Else if the block ends a text block area...
        else if (inBlockArea && atBlockAreaEnd(block, blockType))
        {
            drawBlock = true;
            inBlockArea = false;
            blockAreaRect.setHeight(dy);
        }

        // If the block is at the end of the document and ends a text
        // block area...
        //
        if (inBlockArea && (block == this->document()->lastBlock()))
        {
            drawBlock = true;
            inBlockArea = false;
            dy += r.height();
            blockAreaRect.setHeight(dy);
        }

        offset.ry() += r.height();
        dy += r.height();

        // If this is the last text block visible within the viewport...
        if (offset.y() > viewportRect.height())
        {
            if (inBlockArea)
            {
                blockAreaRect.setHeight(dy);
                drawBlock = true;
            }

            // Finished drawing.
            done = true;
        }

        if (drawBlock)
        {
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(blockColor));

            // If the first visible block is "clipped" such that the previous block
            // is part of the text block area, then only draw a rectangle with the
            // bottom corners rounded, and with the top corners square to reflect
            // that the first visible block is part of a larger block of text.
            //
            if (clipTop)
            {
                QPainterPath path;
                path.setFillRule(Qt::WindingFill);
                path.addRoundedRect(blockAreaRect, cornerRadius, cornerRadius);
                qreal adjustedHeight = blockAreaRect.height() / 2;
                path.addRect(blockAreaRect.adjusted(0, 0, 0, -adjustedHeight));
                painter.drawPath(path.simplified());
                clipTop = false;
            }
            // Else draw the entire rectangle with all corners rounded.
            else
            {
                painter.drawRoundedRect(blockAreaRect, cornerRadius, cornerRadius);
            }

            drawBlock = false;
        }

        block = block.next();
        firstVisible = false;
    }

    painter.end();

    // Draw the visible editor text.
    QPlainTextEdit::paintEvent(event);

    // Draw the text cursor/caret.
    if (textCursorVisible)
    {
        // Get the cursor rect so that we have the ideal height for it,
        // and then set it to be 2 pixels wide.  (The width will be zero,
        // because we set it to be taht in the constructor so that 
        // QPlainTextEdit will not draw another cursor underneath this one.)
        QRect r = cursorRect();
        r.setWidth(2);
        
        QPainter painter(viewport());
        painter.fillRect(r, QBrush(cursorColor));
        painter.end();
    }
}

void MarkdownEditor::setDictionary(const QString& language)
{
    dictionary = DictionaryManager::instance().requestDictionary(language);
    highlighter->setDictionary(dictionary);
}

QLayout* MarkdownEditor::getPreferredLayout()
{
    return preferredLayout;
}

bool MarkdownEditor::getHemingwayModeEnabled() const
{
    return hemingwayModeEnabled;
}

/**
 * Sets whether Hemingway mode is enabled.
 */
void MarkdownEditor::setHemingWayModeEnabled(bool enabled)
{
    hemingwayModeEnabled = enabled;
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

void MarkdownEditor::setColorScheme
(
    const QColor& defaultTextColor,
    const QColor& backgroundColor,
    const QColor& markupColor,
    const QColor& linkColor,
    const QColor& headingColor,
    const QColor& emphasisColor,
    const QColor& blockquoteColor,
    const QColor& codeColor,
    const QColor& spellingErrorColor
)
{
    highlighter->setColorScheme
    (
        defaultTextColor,
        backgroundColor,
        markupColor,
        linkColor,
        headingColor,
        emphasisColor,
        blockquoteColor,
        codeColor,
        spellingErrorColor
    );
    
    this->cursorColor = linkColor;

    blockColor = defaultTextColor;

    int blockAlpha = 20;

    if (backgroundColor.alpha() < 255)
    {
        blockAlpha = 18;
    }
    else if (ColorHelper::getLuminance(blockColor) < 0.5)
    {
        blockAlpha = 10;
    }

    blockColor.setAlpha(blockAlpha);

    QColor fadedForegroundColor = defaultTextColor;
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

    fadeEffect->setFadeHeight(this->fontMetrics().height() * GW_TEXT_FADE_FACTOR);
}

void MarkdownEditor::setShowTabsAndSpacesEnabled(bool enabled)
{
    QTextOption option = textDocument->defaultTextOption();

    if (enabled)
    {
        option.setFlags(option.flags() | QTextOption::ShowTabsAndSpaces);
    }
    else
    {
        option.setFlags(option.flags() & ~QTextOption::ShowTabsAndSpaces);
    }

    textDocument->setDefaultTextOption(option);
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
        setViewportMargins(margin, 20, margin, 0);
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

void MarkdownEditor::dragMoveEvent(QDragMoveEvent* e)
{
    e->acceptProposedAction();
}

void MarkdownEditor::dragLeaveEvent(QDragLeaveEvent* e)
{
    e->accept();
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

        QTextCursor dropCursor = cursorForPosition(e->pos());

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

            dropCursor.insertText(QString("![](%1)").arg(path));

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
        case Qt::Key_Delete:
            if (!hemingwayModeEnabled)
            {
                QPlainTextEdit::keyPressEvent(e);
            }
            break;
        case Qt::Key_Backspace:
            if (!hemingwayModeEnabled)
            {
                if (!handleBackspaceKey())
                {
                    QPlainTextEdit::keyPressEvent(e);
                }
            }
            break;
        case Qt::Key_Tab:
            if (!handleWhitespaceInEmptyMatch('\t'))
            {
                indentText();
            }
            break;
        case Qt::Key_Backtab:
            unindentText();
            break;
        case Qt::Key_Space:
            if (!handleWhitespaceInEmptyMatch(' '))
            {
                QPlainTextEdit::keyPressEvent(e);
            }
            break;
        default:
            if (e->text().size() == 1)
            {
                QChar ch = e->text().at(0);

                if (!handleEndPairCharacterTyped(ch) && !insertPairedCharacters(ch))
                {
                    QPlainTextEdit::keyPressEvent(e);
                }
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

        // If the context menu event was triggered by pressing the menu key,
        // use the current text cursor rather than the event position to get
        // a cursor position, since the event position is the mouse position
        // rather than the text cursor position.
        //
        if (QContextMenuEvent::Keyboard == contextEvent->reason())
        {
            cursorForWord = this->textCursor();
        }
        // Else process as mouse event.
        //
        else
        {
            cursorForWord = cursorForPosition(contextEvent->pos());
        }

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
                cursorForWord.block().layout()->additionalFormats();
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
        cursorForWord.movePosition
        (
            QTextCursor::PreviousCharacter,
            QTextCursor::MoveAnchor,
            blockPosition - mispelledWordStartPos
        );
        cursorForWord.movePosition
        (
            QTextCursor::NextCharacter,
            QTextCursor::KeepAnchor,
            mispelledWordLength
        );

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

                // Need the following line because KDE Plasma 5 will insert a hidden ampersand
                // into the menu text as a keyboard accelerator.  Go off of the data in the
                // QAction rather than the text to avoid this.
                //
                suggestionAction->setData(suggestions[i]);

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

        QPoint menuPos;

        // If event was triggered by a key press, use the text cursor
        // coordinates to display the popup menu.
        //
        if (QContextMenuEvent::Keyboard == contextEvent->reason())
        {
            QRect cr = this->cursorRect();
            menuPos.setX(cr.x());
            menuPos.setY(cr.y() + (cr.height() / 2));
            menuPos = viewport()->mapToGlobal(menuPos);
        }
        // Else use the mouse coordinates from the context menu event.
        //
        else
        {
            menuPos = viewport()->mapToGlobal(contextEvent->pos());
        }

        popupMenu->exec(menuPos);

        delete popupMenu;

        for (int i = 0; i < spellingActions.size(); i++)
        {
            delete spellingActions[i];
        }

        spellingActions.clear();

        return true;
    }
}

void MarkdownEditor::wheelEvent(QWheelEvent *e)
{
    Qt::KeyboardModifiers modifier = e->modifiers();

#if QT_VERSION >= 0x050000
    int numDegrees = 0;

    QPoint angleDelta = e->angleDelta();

    if (!angleDelta.isNull())
    {
        numDegrees = angleDelta.y();
    }
#else
    int numDegrees = e->delta();
#endif

    if ((Qt::ControlModifier == modifier) && (0 != numDegrees))
    {
        int fontSize = this->font().pointSize();

        if (numDegrees > 0)
        {
            fontSize += 1;
        }
        else
        {
            fontSize -= 1;
        }

        // check for negative value
        if (fontSize <= 0)
        {
            fontSize = 1;
        }

        setFont(this->font().family(), fontSize);
        emit fontSizeChanged(fontSize);
    }
    else
    {
        QPlainTextEdit::wheelEvent(e);
    }
}

void MarkdownEditor::navigateDocument(const int pos)
{
    QTextCursor cursor = this->textCursor();
    cursor.setPosition(pos);
    this->setTextCursor(cursor);
    this->activateWindow();
}

void MarkdownEditor::bold()
{
    insertFormattingMarkup("**");
}

void MarkdownEditor::italic()
{
    insertFormattingMarkup("*");
}

void MarkdownEditor::strikethrough()
{
    insertFormattingMarkup("~~");
}

void MarkdownEditor::insertComment()
{
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection())
    {
        QString text = cursor.selectedText();
        text = QString("<!-- " + text + " -->");
        cursor.insertText(text);
    }
    else
    {
        cursor.insertText("<!--  -->");
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 4);
        this->setTextCursor(cursor);
    }
}

void MarkdownEditor::createBulletListWithAsteriskMarker()
{
    insertPrefixForBlocks("* ");
}

void MarkdownEditor::createBulletListWithMinusMarker()
{
    insertPrefixForBlocks("- ");
}

void MarkdownEditor::createBulletListWithPlusMarker()
{
    insertPrefixForBlocks("+ ");
}

void MarkdownEditor::createNumberedListWithPeriodMarker()
{
    createNumberedList('.');
}

void MarkdownEditor::createNumberedListWithParenthesisMarker()
{
    createNumberedList(')');
}

void MarkdownEditor::createTaskList()
{
    insertPrefixForBlocks("- [ ] ");
}

void MarkdownEditor::createBlockquote()
{
    insertPrefixForBlocks("> ");
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

// Algorithm lifted from ReText.
void MarkdownEditor::indentText()
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
                    QStringList capture = numberedListRegex.capturedTexts();

                    // Restart numbering for the nested list.
                    if (capture.size() == 2)
                    {
                        QRegExp numberRegex("\\d+");

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
                                numberRegex,
                                "1"
                            );

                        cursor.insertText(replacementText);
                        cursor.movePosition(QTextCursor::StartOfBlock);
                    }
                }
                break;
            case MarkdownStateBulletPointList:
            {
                if (bulletListRegex.exactMatch(cursor.block().text()))
                {
                    if (bulletPointCyclingEnabled)
                    {
                        QChar oldBulletPoint = cursor.block().text().trimmed().at(0);
                        QChar newBulletPoint = oldBulletPoint;
                        {
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
void MarkdownEditor::unindentText()
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
        && bulletPointCyclingEnabled
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

bool MarkdownEditor::toggleTaskComplete()
{
    QTextCursor cursor = textCursor();
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

    //cursor.beginEditBlock();

    while (block != end)
    {
        if
        (
            (block.userState() == MarkdownStateTaskList)
            && (block.text().indexOf(taskListRegex) == 0)
        )
        {
            QStringList capture = taskListRegex.capturedTexts();

            if (capture.size() == 2)
            {
                QChar value = capture.at(1)[0];
                QChar replacement;
                int index = block.text().indexOf(" [");

                if (index >= 0)
                {
                    index += 2;
                }

                if (value == 'x')
                {
                    replacement = ' ';
                }
                else
                {
                    replacement = 'x';
                }

                cursor.setPosition(block.position());
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.movePosition
                (
                    QTextCursor::Right,
                    QTextCursor::MoveAnchor,
                    index
                );

                cursor.deleteChar();
                cursor.insertText(replacement);
            }
        }

        block = block.next();
    }

    //cursor.endEditBlock();
    return true;
}

void MarkdownEditor::setEnableLargeHeadingSizes(bool enable)
{
    highlighter->setEnableLargeHeadingSizes(enable);
}

void MarkdownEditor::setBlockquoteStyle(const BlockquoteStyle style)
{
    highlighter->setBlockquoteStyle(style);
}

void MarkdownEditor::setHighlightLineBreaks(bool enable)
{
    highlighter->setHighlightLineBreaks(enable);
}

void MarkdownEditor::setAutoMatchEnabled(bool enable)
{
    autoMatchEnabled = enable;
}

void MarkdownEditor::setAutoMatchEnabled(const QChar openingCharacter, bool enabled)
{
    autoMatchFilter.insert(openingCharacter, enabled);
}

void MarkdownEditor::setBulletPointCyclingEnabled(bool enable)
{
    bulletPointCyclingEnabled = enable;
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

void MarkdownEditor::setEditorCorners(InterfaceStyle corners)
{
    editorCorners = corners;
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

void MarkdownEditor::increaseFontSize()
{
    int fontSize = this->font().pointSize() + 1;

    setFont(this->font().family(), fontSize);
    emit fontSizeChanged(fontSize);

}

void MarkdownEditor::decreaseFontSize()
{
    int fontSize = this->font().pointSize() - 1;

    // check for negative value
    if (fontSize <= 0)
    {
        fontSize = 1;
    }

    setFont(this->font().family(), fontSize);
    emit fontSizeChanged(fontSize);
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
        cursorForWord.insertText(action->data().toString());
    }
}

void MarkdownEditor::onContentsChanged(int position, int charsAdded, int charsRemoved)
{
    Q_UNUSED(position)
    Q_UNUSED(charsAdded)
    Q_UNUSED(charsRemoved)

    // Don't use the textChanged() or contentsChanged() (no parameters) signals
    // for checking if the typingResumed() signal needs to be emitted.  These
    // two signals are emitted even when the text formatting changes (i.e.,
    // when the QSyntaxHighlighter formats the text). Instead, use QTextDocument's
    // onContentsChanged(int, int, int) signal, which is only emitted when the
    // document text actually changes.
    //
    if (typingHasPaused)
    {
        typingHasPaused = false;
        typingPausedSignalSent = false;
        emit typingResumed();
    }
}

void MarkdownEditor::onSelectionChanged()
{
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection())
    {
        emit textSelected
        (
            cursor.selectedText(),
            cursor.selectionStart(),
            cursor.selectionEnd()
        );
    }
    else
    {
        emit textDeselected();
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
            // `FocusModeTypewriter` implicitly handeled here as we don't highlight anything but center the current line.
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
    
    // Set the text cursor back to visible and reset the blink timer so that
    // the cursor is always visible whenever it moves to a new position.
    //
    textCursorVisible = true;
    cursorBlinkTimer->stop();
    cursorBlinkTimer->start();
    
    // Update widget to ensure cursor is drawn.
    update();

    emit cursorPositionChanged(this->textCursor().position());
}

void MarkdownEditor::toggleCursorBlink()
{
    textCursorVisible = !textCursorVisible;
    update();
}

void MarkdownEditor::handleCarriageReturn()
{
    QString autoInsertText = "";
    QTextCursor cursor = this->textCursor();
    bool endList = false;

    if (cursor.positionInBlock() < (cursor.block().length() - 1))
    {
        autoInsertText = getPriorIndentation();

        if (cursor.positionInBlock() < autoInsertText.length())
        {
            autoInsertText.truncate(cursor.positionInBlock());
        }
    }
    else
    {
        switch (cursor.block().userState())
        {
            case MarkdownStateNumberedList:
            {
                autoInsertText = getPriorMarkdownBlockItemStart(numberedListRegex);
                QStringList capture = numberedListRegex.capturedTexts();

                if (!autoInsertText.isEmpty() && (capture.size() == 2))
                {
                    // If the line of text is an empty list item, end the list.
                    if (cursor.block().text().length() == autoInsertText.length())
                    {
                        endList = true;
                    }
                    // Else auto-increment the list number.
                    else
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
                }
                else
                {
                    autoInsertText = getPriorIndentation();
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

                    if (autoInsertText.isEmpty())
                    {
                        autoInsertText = getPriorIndentation();
                    }
                    // If the line of text is an empty list item, end the list.
                    else if (cursor.block().text().length() == autoInsertText.length())
                    {
                        endList = true;
                    }
                }
                else // string not empty - GFM task list item
                {
                    // If the line of text is an empty list item, end the list.
                    if (cursor.block().text().length() == autoInsertText.length())
                    {
                        endList = true;
                    }
                    else
                    {
                        // In case the previous line had a completed task with
                        // an X checking it off, make sure a completed task
                        // isn't added as the new task (remove the x and replace
                        // with a space).
                        //
                        autoInsertText = autoInsertText.replace('x', ' ');
                    }
                }
                break;
            case MarkdownStateBlockquote:
                autoInsertText = getPriorMarkdownBlockItemStart(blockquoteRegex);
                break;
            default:
                autoInsertText = getPriorIndentation();
                break;
        }
    }

    if (endList)
    {
        autoInsertText = getPriorIndentation();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.insertText(autoInsertText);
        autoInsertText = "";
    }

    cursor.insertText(QString("\n") + autoInsertText);
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
            // If the first character in an automatched set is being
            // deleted, then delete the second matching one along with it.
            //
            if (autoMatchEnabled && (cursor.positionInBlock() > 0))
            {
                QString blockText = cursor.block().text();

                if (cursor.positionInBlock() < blockText.length())
                {
                    QChar currentChar = blockText[cursor.positionInBlock()];
                    QChar previousChar = blockText[cursor.positionInBlock() - 1];

                    if (markupPairs.value(previousChar) == currentChar)
                    {
                        cursor.movePosition(QTextCursor::Left);
                        cursor.movePosition
                        (
                            QTextCursor::Right,
                            QTextCursor::KeepAnchor,
                            2
                        );
                        cursor.removeSelectedText();
                        return true;
                    }
                }
            }
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
void MarkdownEditor::insertPrefixForBlocks(const QString& prefix)
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
        cursor.insertText(prefix);
        block = block.next();
    }

    cursor.endEditBlock();
}

void MarkdownEditor::createNumberedList(const QChar marker)
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

    int number = 1;

    while (block != end)
    {
        cursor.setPosition(block.position());
        cursor.insertText(QString("%1").arg(number) + marker + " ");
        block = block.next();
        number++;
    }

    cursor.endEditBlock();
}

bool MarkdownEditor::insertPairedCharacters(const QChar firstChar)
{
    if
    (
        autoMatchEnabled
        && markupPairs.contains(firstChar)
        && autoMatchFilter.value(firstChar)
    )
    {
        QChar lastChar = markupPairs.value(firstChar);
        QTextCursor cursor = this->textCursor();
        QTextBlock block;
        QTextBlock end;

        if (cursor.hasSelection())
        {
            block = this->document()->findBlock(cursor.selectionStart());
            end = this->document()->findBlock(cursor.selectionEnd());

            // Only surround selection with matched characters if the
            // selection belongs to the same block.
            //
            if (block == end)
            {
                cursor.beginEditBlock();
                cursor.setPosition(cursor.selectionStart());
                cursor.insertText(firstChar);
                cursor.setPosition(textCursor().selectionEnd());
                cursor.insertText(lastChar);
                cursor = textCursor();

                cursor.setPosition(cursor.selectionStart());
                cursor.setPosition
                (
                    textCursor().selectionEnd() - 1,
                    QTextCursor::KeepAnchor
                );
                setTextCursor(cursor);
                cursor.endEditBlock();
                return true;
            }
        }
        else
        {
            // Get the previous character.  Ensure that it is whitespace.
            int blockPos = cursor.positionInBlock();
            bool doMatch = true;

            // If not at the beginning of the line...
            if (blockPos > 0)
            {
                blockPos--;

                if (!cursor.block().text()[blockPos].isSpace())
                {
                    // If the previous character is not whitespace, allow
                    // character matching only for parentheses and similar
                    // characters that need matching even if preceeded by
                    // non-whitespace (i.e., for mathematical or computer
                    // science expressions).  Otherwise, do not match the
                    // opening character.
                    //
                    switch (firstChar.toLatin1())
                    {
                        case '(':
                        case '[':
                        case '{':
                        case '<':
                            break;
                        default:
                            doMatch = false;
                            break;
                    }
                }
            }

            if (doMatch)
            {
                cursor.insertText(firstChar);
                cursor.insertText(lastChar);
                cursor.movePosition(QTextCursor::PreviousCharacter);
                setTextCursor(cursor);
                return true;
            }
        }
    }

    return false;
}

bool MarkdownEditor::handleEndPairCharacterTyped(const QChar ch)
{
    QTextCursor cursor = this->textCursor();

    bool lookAhead = false;

    if (autoMatchEnabled && !cursor.hasSelection())
    {
        QList<QChar> values = markupPairs.values();

        if (values.contains(ch))
        {
            QChar key = markupPairs.key(ch);

            if (autoMatchFilter.value(key))
            {
                lookAhead = true;
            }
        }
    }

    if (lookAhead)
    {
        QTextCursor cursor = this->textCursor();
        QString text = cursor.block().text();
        int pos = cursor.positionInBlock();

        if (pos < (text.length()))
        {
            // Look ahead to the character after the cursor position. If it
            // matches the character that was entered, then move the cursor
            // one position forward.
            //
            if (text[pos] == ch)
            {
                cursor.movePosition(QTextCursor::NextCharacter);
                setTextCursor(cursor);
                return true;
            }
        }
    }

    return false;
}

bool MarkdownEditor::handleWhitespaceInEmptyMatch(const QChar whitespace)
{
    QTextCursor cursor = this->textCursor();
    QTextBlock block = cursor.block();
    QString text = block.text();
    int pos = cursor.positionInBlock();

    if
    (
        (text.length() > 0) &&
        (pos > 0) &&
        (pos < text.length()) &&
        nonEmptyMarkupPairs.contains(text[pos - 1]) &&
        (text[pos] == nonEmptyMarkupPairs.value(text[pos - 1]))
    )
    {
        cursor.deleteChar();
        cursor.insertText(whitespace);
        return true;
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

QString MarkdownEditor::getPriorIndentation()
{
    QString indent = "";
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

bool MarkdownEditor::atBlockAreaStart(const QTextBlock& block, MarkdownEditor::BlockType& type) const
{
    if (!block.isValid())
    {
        type = BlockTypeNone;
        return false;
    }

    if (atCodeBlockStart(block))
    {
        type = BlockTypeCode;
        return true;
    }

    if (isBlockquote(block))
    {
        type = BlockTypeQuote;
        return true;
    }

    type = BlockTypeNone;
    return false;
}

bool MarkdownEditor::atBlockAreaEnd(const QTextBlock& block, const MarkdownEditor::BlockType type) const
{
    switch (type)
    {
        case BlockTypeCode:
            return atCodeBlockEnd(block);
        case BlockTypeQuote:
            return !isBlockquote(block);
        default:
            return true;
    }
}

bool MarkdownEditor::atCodeBlockStart(const QTextBlock& block) const
{
    return
        (
            (MarkdownStateCodeBlock == block.userState())
            || (MarkdownStateInGithubCodeFence == block.userState())
            || (MarkdownStateInPandocCodeFence == block.userState())
        );
}

bool MarkdownEditor::atCodeBlockEnd(const QTextBlock& block) const
{
    return
        (
            (MarkdownStateCodeBlock != block.userState())
            && (MarkdownStateInPandocCodeFence != block.userState())
            && (MarkdownStateInGithubCodeFence != block.userState())
            && (MarkdownStateCodeFenceEnd != block.userState())
        );
}

bool MarkdownEditor::isBlockquote(const QTextBlock& block) const
{
    return (MarkdownStateBlockquote == block.userState());
}
