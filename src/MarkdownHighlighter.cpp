/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QStyle>
#include <QApplication>
#include <Qt>
#include <QTextLayout>
#include <QStack>

#include "MarkdownHighlighter.h"
#include "MarkdownStates.h"
#include "TextBlockData.h"
#include "spelling/dictionary_ref.h"
#include "spelling/dictionary_manager.h"

MarkdownHighlighter::MarkdownHighlighter(MarkdownEditor* editor)
    : QSyntaxHighlighter(editor),
      editor(editor),
        dictionary(DictionaryManager::instance().requestDictionary()),
        spellCheckEnabled(false),
        typingPaused(true),
        useUndlerlineForEmphasis(false),
        highlightLineBreaks(false),
        inBlockquote(false),
        defaultTextColor(Qt::black),
        backgroundColor(Qt::white),
        markupColor(Qt::black),
        linkColor(Qt::blue),
        headingColor(Qt::black),
        emphasisColor(Qt::black),
        blockquoteColor(Qt::black),
        codeColor(Qt::black),
        spellingErrorColor(Qt::red)
{
    setDocument(editor->document());
    referenceDefinitionRegex.setPattern("^\\s*\\[(.+?)[^\\\\]\\]:");

    connect(editor, SIGNAL(typingResumed()), this, SLOT(onTypingResumed()));
    connect(editor, SIGNAL(typingPausedScaled()), this, SLOT(onTypingPaused()));
    connect(editor, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));

    connect
    (
        this,
        SIGNAL(highlightBlockAtPosition(int)),
        this,
        SLOT(onHighlightBlockAtPosition(int)),
        Qt::QueuedConnection
    );
    
    QFont font;
    font.setFamily("Monospace");
    font.setWeight(QFont::Normal);
    font.setItalic(false);
    font.setPointSizeF(12.0);
    font.setStyleStrategy(QFont::PreferAntialias);
    defaultFormat.setFont(font);
    defaultFormat.setForeground(QBrush(defaultTextColor));
}

MarkdownHighlighter::~MarkdownHighlighter()
{

}

// Note:  Never set the QTextBlockFormat for a QTextBlock from within the
// highlighter.  Depending on how the block format is modified, a recursive call
// to the highlighter may be triggered, which will cause the application to
// crash.
//
// Likewise, don't try to set the QTextBlockFormat outside the highlighter
// (i.e., from within the text editor).  While the application will not crash,
// the format change will be added to the undo stack.  Attempting to undo from
// that point on will cause the undo stack to be virtually frozen, since undoing
// the format operation causes the text to be considered changed, thus
// triggering the slot that changes the text formatting to be triggered yet
// again.
//
void MarkdownHighlighter::highlightBlock(const QString& text)
{
    int line = currentBlock().blockNumber() + 1;
    int oldState = currentBlock().userState();

    MarkdownAST* ast = ((MarkdownDocument*) this->document())->getMarkdownAST();
    MarkdownNode* node = NULL;

    if (NULL != ast)
    {
        node = ast->findBlockAtLine(line);
    }

    if ((NULL != node) && (MarkdownNode::Invalid != node->getType()))
    {
        applyFormattingForNode(node);
    }
    else
    {
        setFormat(0, currentBlock().length(), defaultFormat);

        if (currentBlock().text().trimmed().isEmpty())
        {
            setCurrentBlockState(MarkdownStateParagraphBreak);
        }
        else if (referenceDefinitionRegex.match(currentBlock().text()).hasMatch())
        {
            QTextCharFormat format = defaultFormat;
            format.setForeground(linkColor);

            setFormat(0, currentBlock().text().indexOf(':'), format);
            setCurrentBlockState(MarkdownStateParagraph);
        }
    }

    if (isSetextHeadingState(oldState) && !isSetextHeadingState(currentBlockState()))
    {
        QTextBlock block = currentBlock();
            
        while
        (
            block.previous().isValid()
            && (isSetextHeadingState(block.previous().userState()))
        )
        {
            block = block.previous();
        }

        if (currentBlock() != block)
        {
            emit highlightBlockAtPosition(block.position());
        }
    }
    else if 
    (
        currentBlock().previous().isValid()
        &&
        (
            (
                (MarkdownStatePipeTableDivider == (oldState & MarkdownStateMask))
                && (MarkdownStatePipeTableDivider != (currentBlockState() & MarkdownStateMask))
            )
            ||
            (
                (MarkdownStatePipeTableDivider != (oldState & MarkdownStateMask))
                && (MarkdownStatePipeTableDivider == (currentBlockState() & MarkdownStateMask))
            )
        )
    )
    {
        emit highlightBlockAtPosition(currentBlock().previous().position());
    }
    
    if (spellCheckEnabled)
    {
        spellCheck(text);
    }
}

void MarkdownHighlighter::setDictionary(const DictionaryRef& dictionary)
{
    this->dictionary = dictionary;

    if (spellCheckEnabled)
    {
        rehighlight();
    }
}

void MarkdownHighlighter::increaseFontSize()
{
    defaultFormat.setFontPointSize(defaultFormat.fontPointSize() + 1.0);
    rehighlight();
}

void MarkdownHighlighter::decreaseFontSize()
{
    defaultFormat.setFontPointSize(defaultFormat.fontPointSize() - 1.0);
    rehighlight();
}

void MarkdownHighlighter::setColorScheme
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
    this->defaultTextColor = defaultTextColor;
    this->backgroundColor = backgroundColor;
    this->markupColor = markupColor;
    this->linkColor = linkColor;
    this->headingColor = headingColor;
    this->emphasisColor = emphasisColor;
    this->blockquoteColor = blockquoteColor;
    this->codeColor = codeColor;
    this->spellingErrorColor = spellingErrorColor;
    defaultFormat.setForeground(QBrush(defaultTextColor));
    rehighlight();
}

void MarkdownHighlighter::setEnableLargeHeadingSizes(const bool enable)
{
    useLargeHeadings = enable;
    rehighlight();
}

void MarkdownHighlighter::setUseUnderlineForEmphasis(const bool enable)
{
    useUndlerlineForEmphasis = enable;
    rehighlight();
}

void MarkdownHighlighter::setFont(const QString& fontFamily, const double fontSize)
{
    QFont font;
    font.setFamily(fontFamily);
    font.setWeight(QFont::Normal);
    font.setItalic(false);
    font.setPointSizeF(fontSize);
    defaultFormat.setFont(font);
    rehighlight();
}

void MarkdownHighlighter::setSpellCheckEnabled(const bool enabled)
{
    spellCheckEnabled = enabled;
    rehighlight();
}

void MarkdownHighlighter::onTypingResumed()
{
    typingPaused = false;
}

void MarkdownHighlighter::onTypingPaused()
{
    typingPaused = true;

    if (spellCheckEnabled)
    {
        QTextBlock block = document()->findBlock(editor->textCursor().position());
        rehighlightBlock(block);
    }
}

void MarkdownHighlighter::onCursorPositionChanged()
{
    if
    (
        spellCheckEnabled &&
        (currentLine != editor->textCursor().block())
    )
    {
        rehighlightBlock(currentLine);
    }

    currentLine = editor->textCursor().block();
}

void MarkdownHighlighter::setBlockquoteStyle(const BlockquoteStyle style)
{
    blockquoteStyle = style;
    rehighlight();
}

void MarkdownHighlighter::setHighlightLineBreaks(bool enable)
{
    highlightLineBreaks = enable;
    rehighlight();
}

void MarkdownHighlighter::onHighlightBlockAtPosition(int position)
{
    QTextBlock block = document()->findBlock(position);
    rehighlightBlock(block);
}

void MarkdownHighlighter::spellCheck(const QString& text)
{
    int cursorPosition = editor->textCursor().position();
    QTextBlock cursorPosBlock = this->document()->findBlock(cursorPosition);
    int cursorPosInBlock = -1;

    if (this->currentBlock() == cursorPosBlock)
    {
        cursorPosInBlock = cursorPosition - cursorPosBlock.position();
    }

    QStringRef misspelledWord = dictionary.check(text, 0);

    while (!misspelledWord.isNull())
    {
        int startIndex = misspelledWord.position();
        int length = misspelledWord.length();

        if (typingPaused || (cursorPosInBlock != (startIndex + length)))
        {
            QTextCharFormat spellingErrorFormat = format(startIndex);
            spellingErrorFormat.setUnderlineColor(spellingErrorColor);
            spellingErrorFormat.setUnderlineStyle
            (
                (QTextCharFormat::UnderlineStyle)
                QApplication::style()->styleHint
                (
                    QStyle::SH_SpellCheckUnderlineStyle
                )
            );

            setFormat(startIndex, length, spellingErrorFormat);
        }

        startIndex += length;
        misspelledWord = dictionary.check(text, startIndex);
    }
}

void MarkdownHighlighter::applyFormattingForNode(const MarkdownNode* const node)
{
    MarkdownNode::NodeType type = node->getType();
    int pos = node->getPosition();
    int length = node->getLength();
    int currentLine = currentBlock().blockNumber() + 1;
    MarkdownState state = MarkdownStateParagraphBreak;

    QTextCharFormat baseFormat = defaultFormat;
    baseFormat.setForeground(defaultTextColor);

    unsigned int indent = 0;
    QString text = currentBlock().text();

    for (int i = 0; i < text.length(); i++)
    {
        if (text[i].isSpace())
        {
            indent++;
        }
        else
        {
            break;
        }
    }

    bool inBlockquote = node->isInsideBlockquote();

    if (inBlockquote)
    {
        baseFormat.setForeground(markupColor);
        baseFormat.setFontItalic(BlockquoteStyleItalic == blockquoteStyle);

        setFormat
        (
            0,
            currentBlock().length(),
            baseFormat
        );

        baseFormat.setForeground(blockquoteColor);
    }
    else
    {
        setFormat
        (
            0,
            currentBlock().length(),
            baseFormat
        );
    }
    
    // Do a pre-order traversal of the nodes.
    QStack<const MarkdownNode*> nodes;
    QStack<QTextCharFormat> nodeFormats;
    nodes.push(node);
    nodeFormats.push(baseFormat);

    while (!nodes.isEmpty())
    {
        const MarkdownNode* current = nodes.pop();
        QTextCharFormat contextFormat = nodeFormats.pop();
        MarkdownNode::NodeType parentType = MarkdownNode::Invalid;
        
        if (NULL != current->getParent())
        {
            parentType = current->getParent()->getType();
        }

        pos = getColumnInLine(current);
        length = current->getLength();
        type = current->getType();
        
        if (lineMatchesNode(currentLine, current))
        {
            
            if
            (
                (MarkdownNode::FootnoteDefinition == parentType)
                || (MarkdownNode::FootnoteReference == parentType)
            )
            {
                type = parentType;
            }
            
            QTextCharFormat format = contextFormat;

            switch (type)
            {
                case MarkdownNode::Heading:
                    length = currentBlock().length();
                    format.setForeground(markupColor);
                    format.setFontWeight(QFont::Bold);
                    contextFormat.setFontWeight(QFont::Bold);

                    if (this->useLargeHeadings)
                    {
                        format.setFontPointSize(format.fontPointSize()
                            + (qreal) (7 - current->getHeadingLevel()));
                        contextFormat.setFontPointSize(format.fontPointSize());
                    }

                    if (inBlockquote)
                    {
                        contextFormat.setForeground(blockquoteColor);
                    }
                    else
                    {
                        contextFormat.setForeground(headingColor);
                    }

                    if (current->isSetextHeading())
                    {
                        switch (current->getHeadingLevel())
                        {
                            case 1:
                                state = MarkdownStateSetextHeading1;
                                break;
                            case 2:
                                state = MarkdownStateSetextHeading2;
                                break;
                            default:
                                state = MarkdownStateUnknown;
                        }

                        // Rehighlight all blocks contained within this heading node.
                        if (currentLine != current->getStartLine())
                        {
                            QTextBlock block = this->document()->findBlockByNumber(current->getStartLine() - 1);

                            if (block.isValid())
                            {
                                emit highlightBlockAtPosition(block.position());
                            }
                        }
                    }
                    else
                    {
                        switch (current->getHeadingLevel())
                        {
                            case 1:
                                state = MarkdownStateAtxHeading1;
                                break;
                            case 2:
                                state = MarkdownStateAtxHeading2;
                                break;
                            case 3:
                                state = MarkdownStateAtxHeading3;
                                break;
                            case 4:
                                state = MarkdownStateAtxHeading4;
                                break;
                            case 5:
                                state = MarkdownStateAtxHeading5;
                                break;
                            case 6:
                                state = MarkdownStateAtxHeading6;
                                break;
                            default:
                                state = MarkdownStateUnknown;
                        }
                    }
                
                    break;
                case MarkdownNode::Text:
                    break;
                case MarkdownNode::Paragraph:
                    if (MarkdownStateUnknown == state)
                    {
                        state = MarkdownStateParagraph;
                    }

                    break;
                case MarkdownNode::BlockQuote:
                    format.setForeground(markupColor);
                    format.setFontItalic(BlockquoteStyleItalic == blockquoteStyle);
                    contextFormat.setForeground(blockquoteColor);
                    contextFormat.setFontItalic(BlockquoteStyleItalic == blockquoteStyle);
                    inBlockquote = true;
                    break;
                case MarkdownNode::CodeBlock:
                    if 
                    (
                        current->isFencedCodeBlock()
                        &&
                        (
                            ((currentBlock().blockNumber() + 1) == current->getStartLine())
                            || ((currentBlock().blockNumber() + 1) == current->getEndLine())
                        )
                    )
                    {
                        format.setForeground(markupColor);
                        state = MarkdownStateCodeBlock;
                    }
                    else if
                    (
                        ((currentBlock().blockNumber() + 1) == current->getEndLine())
                        && (current->getLength() <= 0)
                    )
                    {
                        state = MarkdownStateParagraphBreak;
                    }
                    else
                    {
                        format.setForeground(codeColor);
                        length = currentBlock().length() - pos + 1;
                        state = MarkdownStateCodeBlock;
                    }
                    
                    break;
                case MarkdownNode::ListItem:
                    format.setForeground(markupColor);
                    format.setFontWeight(QFont::Bold);
                    
                    if (current->isNumberedListItem())
                    {
                        state = MarkdownStateNumberedList;
                    }
                    else // Assume bullet list item
                    {
                        state = MarkdownStateBulletPointList;
                    }
                    
                    break;
                case MarkdownNode::TaskListItem:
                {
                    state = MarkdownStateTaskList;
                    format.setForeground(markupColor);
                    format.setFontWeight(QFont::Bold);
                    break;
                }
                case MarkdownNode::Emph:
                    format.setForeground(markupColor);

                    if (this->useUndlerlineForEmphasis)
                    {
                        contextFormat.setFontUnderline(true);
                    }
                    else
                    {
                        contextFormat.setFontItalic(true);
                        format.setFontItalic(true);
                    }

                    contextFormat.setForeground(emphasisColor);
                    break;
                case MarkdownNode::Strong:
                    contextFormat.setForeground(emphasisColor);
                    contextFormat.setFontWeight(QFont::Bold);
                    format.setForeground(markupColor);
                    format.setFontWeight(QFont::Bold);
                    break;
                case MarkdownNode::Code:
                {
                    int backticks = 0;

                    for (int i = pos - 1; i >= 0; i--)
                    {
                        if (text[i] == '`')
                        {
                            backticks++;
                        }
                    }

                    format.setForeground(markupColor);
                    setFormat
                    (
                        pos - backticks,
                        length + (2 * backticks),
                        format
                    );
                    format.setForeground(codeColor);
                    break;
                }
                case MarkdownNode::HtmlInline:
                    format.setForeground(markupColor);
                    contextFormat.setForeground(markupColor);
                    break;
                case MarkdownNode::Link:
                case MarkdownNode::Image:
                    format.setForeground(linkColor);
                    contextFormat.setForeground(linkColor);
                    break;
                case MarkdownNode::ThematicBreak:
                    format.setForeground(markupColor);
                    state = MarkdownStateHorizontalRule;
                    break;
                case MarkdownNode::FootnoteReference:
                    format.setForeground(linkColor);
                    contextFormat.setForeground(linkColor);
                    break;
                case MarkdownNode::FootnoteDefinition:
                    format.setForeground(linkColor);
                    contextFormat.setForeground(linkColor);
                    state = MarkdownStateParagraph;
                    break;
                case MarkdownNode::TableHeading:
                    format.setForeground(markupColor);
                    pos = 0;
                    length = currentBlock().length();
                    contextFormat.setFontWeight(QFont::Bold);
                    state = MarkdownStatePipeTableHeader;
                    break;
                case MarkdownNode::TableRow:
                    format.setForeground(markupColor);
                    pos = 0;
                    length = currentBlock().length();
                    state = MarkdownStatePipeTableRow;
                    break;
                case MarkdownNode::TableCell:
                    format = contextFormat;

                    if 
                    (
                        (NULL != current->getParent())
                        && (MarkdownNode::TableHeading == current->getParent()->getType())
                    )
                    {
                        format.setFontWeight(QFont::Bold);
                    }
                    break;
                case MarkdownNode::Table:
                    format.setForeground(markupColor);
                    pos = 0;
                    length = currentBlock().length();
                    state = MarkdownStatePipeTableDivider;
                    break;
                case MarkdownNode::Strikethrough:
                    format.setForeground(markupColor);
                    contextFormat.setFontStrikeOut(true);
                    break;
                default:
                    if (referenceDefinitionRegex.match(currentBlock().text()).hasMatch())
                    {
                        pos = 0;
                        length = currentBlock().text().indexOf(':') + 1;
                        format.setForeground(linkColor);
                    }
                    else
                    {
                        format.setForeground(markupColor);
                    }

                    break;
            }            

            if (length <= 0)
            {
                length = currentBlock().length();
            }

            setFormat
            (
                pos,
                length,
                format
            );

            if (MarkdownNode::Text == type)
            {
                highlightRefLinks(pos, length);
            }
            else if (MarkdownNode::TaskListItem == type)
            {
                format = contextFormat;
                format.setForeground(linkColor);

                int checkboxStart = text.indexOf('[');
                int checkboxEnd = text.indexOf(']');

                setFormat
                (
                    checkboxStart,
                    checkboxEnd - checkboxStart + 1,
                    format
                );
            }
        }   

        MarkdownNode* child = current->getLastChild();

        while ((NULL != child) && (!child->isInvalid()))
        {
            nodes.push(child);
            nodeFormats.push(contextFormat);
            child = child->getPrevious();
        }
    }

    if (MarkdownStateUnknown != state)
    {            
        state |= indent;

        if (inBlockquote)
        {
            state |= MarkdownStateBlockquote;
        }

        setCurrentBlockState(state);
    }
}

int MarkdownHighlighter::getColumnInLine(const MarkdownNode* const node) const
{
    MarkdownNode::NodeType prevType = MarkdownNode::Invalid;
    
    if (NULL != node->getPrevious())
    {
        prevType = node->getPrevious()->getType();
    }

    static int offset = 0;

    if (node->isBlockType())
    {
        offset = 0;
    }
    else if
    (
        (MarkdownNode::Softbreak == prevType)
        || (MarkdownNode::Linebreak == prevType)
    )
    {
        int pos = 0;
        QChar startChar;
        QString text = currentBlock().text();

        switch (node->getType())
        {
            case MarkdownNode::Text:
                pos = text.indexOf(node->getText()[0]);

                if (node->getText().startsWith('`'))
                {
                    int retValue = pos;
                    pos += node->getText().length() - node->getLength();
                    offset = node->getPosition() - pos;
                    return retValue;
                }

                break;
            case MarkdownNode::Code:
                pos = text.indexOf('`');

                for (int i = pos; i < text.length(); i++)
                {
                    if (text[i] != '`')
                    {
                        pos = i;
                        break;
                    }
                }

                break;
            case MarkdownNode::Emph:
            case MarkdownNode::Strong:
                pos = text.indexOf(QRegularExpression("[*_]"));
                break;
            case MarkdownNode::Link:
                pos = text.indexOf('[');
                break;
            case MarkdownNode::Image:
                pos = text.indexOf('!');
                break;
            case MarkdownNode::HtmlInline:
                pos = text.indexOf('<');
                break;
            case MarkdownNode::Strikethrough:
                pos = text.indexOf('~');
                break;
            default:
                pos = text.indexOf(node->getText()[0]);
                break;
        }

        offset = node->getPosition() - pos;
    }

    return node->getPosition() - offset;
}

bool MarkdownHighlighter::lineMatchesNode(const int line, const MarkdownNode* const node) const
{
    return
        (
            (
                node->isBlockType()
                && (line >= node->getStartLine())
                &&
                (
                    (line <= node->getEndLine())
                    || (0 == node->getEndLine())
                )
            )
            ||
            (
                node->isInlineType()
                &&
                (
                    (line == node->getStartLine())
                    || (line == node->getEndLine())
                    || (0 == node->getEndLine())
                )
            )
        );
}

void MarkdownHighlighter::highlightRefLinks(const int pos, const int length)
{
    QStack<int> bracketPos;
    bool skipNext = false;
    QTextCharFormat format = this->format(pos);
    format.setForeground(linkColor);

    for (int i = pos; i < (pos + length); i++)
    {
        if (skipNext)
        {
            skipNext = false;
            continue;
        }

        switch (currentBlock().text()[i].toLatin1())
        {
            case '\\':
                skipNext = true;
                break;
            case '[':
                bracketPos.push(i);
                break;
            case ']':
                if (!bracketPos.isEmpty())
                {
                    int start = bracketPos.pop();
                    
                    setFormat
                    (
                        start,
                        (i - start + 1),
                        format
                    );
                }

                break;
            default:
                break;
        }
    }
}

bool MarkdownHighlighter::isSetextHeadingState(const int state)
{
    switch(state & MarkdownStateMask)
    {
        case MarkdownStateSetextHeading1:
        case MarkdownStateSetextHeading2:
            return true;
        default:
            return false;
    }
}

void MarkdownHighlighter::backtrackHighlight(const int state) 
{
    QTextBlock block = currentBlock();
        
    while
    (
        block.previous().isValid()
        && (block.previous().userState() == state)
    )
    {
        block = block.previous();
    }

    if (currentBlock() != block)
    {
        emit highlightBlockAtPosition(block.position());
    }
}

