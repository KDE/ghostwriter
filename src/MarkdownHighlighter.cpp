/***********************************************************************
 *
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

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QObject>
#include <QRegExp>
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QStyle>
#include <QApplication>
#include <Qt>

#include "MarkdownHighlighter.h"
#include "MarkdownTokenizer.h"
#include "MarkdownTokenTypes.h"
#include "MarkdownStates.h"
#include "ColorHelper.h"
#include "spelling/dictionary_ref.h"
#include "spelling/dictionary_manager.h"

#define GW_FADE_ALPHA 200

MarkdownHighlighter::MarkdownHighlighter(QTextDocument* document)
    : QSyntaxHighlighter(document), tokenizer(NULL),
        dictionary(DictionaryManager::instance().requestDictionary()),
        cursorPosition(0),
        spellCheckEnabled(false),
        typingPaused(true),
        useUndlerlineForEmphasis(false),
        highlightLineBreaks(false),
        inBlockquote(false),
        defaultTextColor(Qt::black),
        backgroundColor(Qt::white),
        markupColor(Qt::black),
        linkColor(Qt::blue),
        spellingErrorColor(Qt::red)
{
    this->tokenizer = new MarkdownTokenizer();

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

    setupTokenColors();

    for (int i = 0; i < TokenLast; i++)
    {
        applyStyleToMarkup[i] = false;
        emphasizeToken[i] = false;
        strongToken[i] = false;
        strongMarkup[i] = false;
        strikethroughToken[i] = false;
        fontSizeIncrease[i] = 0;
    }

    for (int i = TokenAtxHeading1; i <= TokenAtxHeading6; i++)
    {
        applyStyleToMarkup[i] = true;
    }

    applyStyleToMarkup[TokenEmphasis] = true;
    applyStyleToMarkup[TokenStrong] = true;
    applyStyleToMarkup[TokenAtxHeading1] = true;
    applyStyleToMarkup[TokenAtxHeading2] = true;
    applyStyleToMarkup[TokenAtxHeading3] = true;
    applyStyleToMarkup[TokenAtxHeading4] = true;
    applyStyleToMarkup[TokenAtxHeading5] = true;
    applyStyleToMarkup[TokenAtxHeading6] = true;

    emphasizeToken[TokenEmphasis] = true;
    emphasizeToken[TokenBlockquote] = false;
    strongToken[TokenStrong] = true;
    strongToken[TokenMention] = true;
    strongToken[TokenAtxHeading1] = true;
    strongToken[TokenAtxHeading2] = true;
    strongToken[TokenAtxHeading3] = true;
    strongToken[TokenAtxHeading4] = true;
    strongToken[TokenAtxHeading5] = true;
    strongToken[TokenAtxHeading6] = true;
    strongToken[TokenSetextHeading1Line1] = true;
    strongToken[TokenSetextHeading2Line1] = true;
    strongToken[TokenSetextHeading1Line2] = true;
    strongToken[TokenSetextHeading2Line2] = true;
    strongToken[TokenTableHeader] = true;
    strikethroughToken[TokenStrikethrough] = true;

    setupHeadingFontSize(true);

    strongMarkup[TokenNumberedList] = true;
    strongMarkup[TokenBlockquote] = true;
    strongMarkup[TokenBulletPointList] = true;
}

MarkdownHighlighter::~MarkdownHighlighter()
{
    if (NULL != tokenizer)
    {
        delete tokenizer;
        tokenizer = NULL;
    }
}

// Note:  Never, ever set the QTextBlockFormat for a QTextBlock from within the
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
    int lastState = currentBlockState();

    setFormat(0, text.length(), defaultFormat);

    if (NULL != tokenizer)
    {
        tokenizer->clear();

        QTextBlock block = this->currentBlock();
        int nextState = MarkdownStateUnknown;
        int previousState = this->previousBlockState();

        if (block.next().isValid())
        {
            nextState = block.next().userState();
        }

        tokenizer->tokenize(text, lastState, previousState, nextState);
        setCurrentBlockState(tokenizer->getState());

        if (MarkdownStateBlockquote == tokenizer->getState())
        {
            inBlockquote = true;
        }
        else
        {
            inBlockquote = false;
        }

        QList<Token> tokens = tokenizer->getTokens();

        foreach (Token token, tokens)
        {
            switch (token.getType())
            {
                case TokenAtxHeading1:
                case TokenAtxHeading2:
                case TokenAtxHeading3:
                case TokenAtxHeading4:
                case TokenAtxHeading5:
                case TokenAtxHeading6:
                    applyFormattingForToken(token);
                    emit headingFound
                    (
                        block.position(),
                        token.getType() - TokenAtxHeading1 + 1,
                        text.mid
                            (
                                token.getPosition()
                                    + token.getOpeningMarkupLength(),
                                token.getLength()
                                    - token.getOpeningMarkupLength()
                                    - token.getClosingMarkupLength()
                            ).trimmed()
                    );
                    break;
                case TokenSetextHeading1Line1:
                    applyFormattingForToken(token);
                    emit headingFound(block.position(), 1, text);
                    break;
                case TokenSetextHeading2Line1:
                    applyFormattingForToken(token);
                    emit headingFound(block.position(), 2, text);
                    break;
                case TokenUnknown:
                    qWarning("Highlighter found unknown token type in text block.");
                    break;
                default:
                    applyFormattingForToken(token);
                    break;
            }
        }

        if (tokenizer->backtrackRequested())
        {
            QTextBlock previous = currentBlock().previous();
            emit highlightBlockAtPosition(previous.position());
        }
    }

    if (spellCheckEnabled)
    {
        spellCheck(text);
    }

    // If the block has transitioned from previously being a heading to now
    // being a non-heading, signal that the position in the document no longer
    // contains a heading.
    //
    if
    (
        isHeadingBlockState(lastState)
        && !isHeadingBlockState(currentBlockState())
    )
    {
        emit headingRemoved(currentBlock().position());
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
    const QColor& spellingErrorColor
)
{
    this->defaultTextColor = defaultTextColor;
    this->backgroundColor = backgroundColor;
    this->markupColor = markupColor;
    this->linkColor = linkColor;
    this->spellingErrorColor = spellingErrorColor;
    defaultFormat.setForeground(QBrush(defaultTextColor));
    setupTokenColors();
    rehighlight();
}

void MarkdownHighlighter::setEnableLargeHeadingSizes(const bool enable)
{
    setupHeadingFontSize(enable);
    rehighlight();
}

void MarkdownHighlighter::setUseUnderlineForEmphasis(const bool enable)
{
    useUndlerlineForEmphasis = enable;
    rehighlight();
}

void MarkdownHighlighter::setHighlightLineBreaks(bool enable)
{
    highlightLineBreaks = enable;
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

void MarkdownHighlighter::setBlockquoteStyle(const BlockquoteStyle style)
{
    blockquoteStyle = style;

    switch (style)
    {
        case BlockquoteStyleItalic:
            emphasizeToken[TokenBlockquote] = true;
            break;
        default: // Fancy and Plain
            emphasizeToken[TokenBlockquote] = false;
            break;
    }

    rehighlight();
}

void MarkdownHighlighter::onCursorPositionChanged(int position)
{
    int oldPosition = cursorPosition;
    cursorPosition = position;

    if (spellCheckEnabled)
    {
        QTextBlock oldBlock = document()->findBlock(oldPosition);
        QTextBlock newBlock = document()->findBlock(cursorPosition);

        if (oldBlock != newBlock)
        {
            rehighlightBlock(oldBlock);
        }

        rehighlightBlock(newBlock);
    }
}

void MarkdownHighlighter::onTypingResumed()
{
    typingPaused = false;
}

void MarkdownHighlighter::onTypingPaused()
{
    typingPaused = true;
    QTextBlock block = document()->findBlock(cursorPosition);
    rehighlightBlock(block);
}

void MarkdownHighlighter::onHighlightBlockAtPosition(int position)
{
    QTextBlock block = document()->findBlock(position);
    rehighlightBlock(block);
}

bool MarkdownHighlighter::isHeadingBlockState(int state) const
{
    switch (state)
    {
        case MarkdownStateAtxHeading1:
        case MarkdownStateAtxHeading2:
        case MarkdownStateAtxHeading3:
        case MarkdownStateAtxHeading4:
        case MarkdownStateAtxHeading5:
        case MarkdownStateAtxHeading6:
        case MarkdownStateSetextHeading1Line1:
        case MarkdownStateSetextHeading2Line1:
            return true;
        default:
            return false;
    }
}

void MarkdownHighlighter::spellCheck(const QString& text)
{
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

void MarkdownHighlighter::setupTokenColors()
{
    for (int i = 0; i < TokenLast; i++)
    {
        colorForToken[i] = defaultTextColor;
    }

    QColor fadedColor =
        ColorHelper::applyAlpha
        (
            defaultTextColor,
            backgroundColor,
            GW_FADE_ALPHA
        );

    colorForToken[TokenBlockquote] = fadedColor;
    colorForToken[TokenCodeBlock] = fadedColor;
    colorForToken[TokenVerbatim] = fadedColor;
    colorForToken[TokenHtmlTag] = markupColor;
    colorForToken[TokenHtmlEntity] = markupColor;
    colorForToken[TokenAutomaticLink] = linkColor;
    colorForToken[TokenInlineLink] = linkColor;
    colorForToken[TokenReferenceLink] = linkColor;
    colorForToken[TokenReferenceDefinition] = linkColor;
    colorForToken[TokenImage] = linkColor;
    colorForToken[TokenMention] = linkColor;
    colorForToken[TokenHtmlComment] = markupColor;
    colorForToken[TokenHorizontalRule] = markupColor;
    colorForToken[TokenGithubCodeFence] = markupColor;
    colorForToken[TokenPandocCodeFence] = markupColor;
    colorForToken[TokenCodeFenceEnd] = markupColor;
    colorForToken[TokenSetextHeading1Line2] = markupColor;
    colorForToken[TokenSetextHeading2Line2] = markupColor;
    colorForToken[TokenTableDivider] = markupColor;
    colorForToken[TokenTablePipe] = markupColor;
}

void MarkdownHighlighter::setupHeadingFontSize(bool useLargeHeadings)
{
    if (useLargeHeadings)
    {
        fontSizeIncrease[TokenSetextHeading1Line1] = 6;
        fontSizeIncrease[TokenSetextHeading2Line1] = 5;
        fontSizeIncrease[TokenSetextHeading1Line2] = 6;
        fontSizeIncrease[TokenSetextHeading2Line2] = 5;
        fontSizeIncrease[TokenAtxHeading1] = 6;
        fontSizeIncrease[TokenAtxHeading2] = 5;
        fontSizeIncrease[TokenAtxHeading3] = 4;
        fontSizeIncrease[TokenAtxHeading4] = 3;
        fontSizeIncrease[TokenAtxHeading5] = 2;
        fontSizeIncrease[TokenAtxHeading6] = 1;
    }
    else
    {
        fontSizeIncrease[TokenSetextHeading1Line1] = 0;
        fontSizeIncrease[TokenSetextHeading2Line1] = 0;
        fontSizeIncrease[TokenSetextHeading1Line2] = 0;
        fontSizeIncrease[TokenSetextHeading2Line2] = 0;
        fontSizeIncrease[TokenAtxHeading1] = 0;
        fontSizeIncrease[TokenAtxHeading2] = 0;
        fontSizeIncrease[TokenAtxHeading3] = 0;
        fontSizeIncrease[TokenAtxHeading4] = 0;
        fontSizeIncrease[TokenAtxHeading5] = 0;
        fontSizeIncrease[TokenAtxHeading6] = 0;
    }
}

void MarkdownHighlighter::applyFormattingForToken(const Token& token)
{
    if (TokenUnknown != token.getType())
    {
        int tokenType = token.getType();
        QTextCharFormat format = this->format(token.getPosition());

        QColor tokenColor = colorForToken[tokenType];

        if (inBlockquote && token.getType() != TokenBlockquote)
        {
            tokenColor =
                ColorHelper::applyAlpha
                (
                    tokenColor,
                    backgroundColor,
                    GW_FADE_ALPHA
                );
        }

        if(highlightLineBreaks && token.getType() == TokenLineBreak)
        {
            format.setBackground(QBrush(Qt::gray, Qt::VerPattern));
        }

        format.setForeground(QBrush(tokenColor));

        if (strongToken[tokenType])
        {
            format.setFontWeight(QFont::Bold);
        }

        if (emphasizeToken[tokenType])
        {
            if (useUndlerlineForEmphasis && (tokenType != TokenBlockquote))
            {
                format.setFontUnderline(true);
            }
            else
            {
                format.setFontItalic(true);
            }
        }

        if (strikethroughToken[tokenType])
        {
            format.setFontStrikeOut(true);
        }

        format.setFontPointSize(format.fontPointSize()
            + (qreal) fontSizeIncrease[tokenType]);

        QTextCharFormat markupFormat;

        if
        (
            applyStyleToMarkup[tokenType] &&
            (!emphasizeToken[tokenType] || !useUndlerlineForEmphasis)
        )
        {
            markupFormat = format;
        }
        else
        {
            markupFormat = this->format(token.getPosition());
        }

        QColor adjustedMarkupColor = this->markupColor;

        if (inBlockquote && token.getType() != TokenBlockquote)
        {
            adjustedMarkupColor =
                ColorHelper::applyAlpha
                (
                    adjustedMarkupColor,
                    backgroundColor,
                    GW_FADE_ALPHA
                );
        }

        markupFormat.setForeground(QBrush(adjustedMarkupColor));

        if (strongMarkup[tokenType])
        {
            markupFormat.setFontWeight(QFont::Bold);
        }

        if (token.getOpeningMarkupLength() > 0)
        {
            if
            (
                (TokenBlockquote == token.getType())
                && (BlockquoteStyleFancy == blockquoteStyle)
            )
            {
                markupFormat.setBackground(QBrush(adjustedMarkupColor));
                QString text = currentBlock().text();

                for (int i = token.getPosition(); i < token.getOpeningMarkupLength(); i++)
                {
                    if (!text[i].isSpace())
                    {
                        setFormat
                        (
                            i,
                            1,
                            markupFormat
                        );
                    }
                }
            }
            else
            {
                setFormat
                (
                    token.getPosition(),
                    token.getOpeningMarkupLength(),
                    markupFormat
                );
            }
        }

        setFormat
        (
            token.getPosition() + token.getOpeningMarkupLength(),
            token.getLength()
                - token.getOpeningMarkupLength()
                - token.getClosingMarkupLength(),
            format
        );

        if (token.getClosingMarkupLength() > 0)
        {
            setFormat
            (
                token.getPosition() + token.getLength()
                    - token.getClosingMarkupLength(),
                token.getClosingMarkupLength(),
                markupFormat
            );
        }
    }
    else
    {
        qWarning("Highlighter::applyFormattingForToken() was passed in a "
            "token of unknown type.");
    }
}
