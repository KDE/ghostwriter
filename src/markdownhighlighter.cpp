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
#include <QDebug>
#include <QFont>
#include <QObject>
#include <QPainter>
#include <QRegularExpression>
#include <QStaticText>
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

#include "markdownhighlighter.h"
#include "markdownstates.h"
#include "spelling/dictionary_ref.h"
#include "spelling/dictionary_manager.h"

namespace ghostwriter
{
class MarkdownHighlighterPrivate
{
    Q_DISABLE_COPY(MarkdownHighlighterPrivate)
    Q_DECLARE_PUBLIC(MarkdownHighlighter)

public:
    MarkdownHighlighterPrivate(MarkdownHighlighter *highlighter) :
        q_ptr(highlighter),
        dictionary(DictionaryManager::instance().requestDictionary()),
        inBlockquote(false),
        spellCheckEnabled(false),
        typingPaused(true),
        useUndlerlineForEmphasis(false)
    {
        ;
    }

    ~MarkdownHighlighterPrivate()
    {
        ;
    }

    MarkdownHighlighter *const q_ptr;

    ColorScheme colors;
    QTextBlock currentLine;
    QTextCharFormat defaultFormat;
    DictionaryRef dictionary;
    MarkdownEditor *editor;
    QRegularExpression heading1SetextRegex;
    QRegularExpression heading2SetextRegex;
    bool inBlockquote;
    QRegularExpression referenceDefinitionRegex;
    QRegularExpression inlineHtmlCommentRegex;
    bool spellCheckEnabled;
    bool typingPaused;
    bool useLargeHeadings;
    bool useUndlerlineForEmphasis;
    bool italicizeBlockquotes;

    bool isSetextHeadingState(const int state);
    bool lineMatchesNode(const int line, const MarkdownNode *const node) const;
    int columnInLine(const MarkdownNode *const node, const QString &lineText) const;
    void applyFormattingForNode(const MarkdownNode *const node);
    void highlightRefLinks(const int pos, const int length);
    void setupHeadingFontSize(bool useLargeHeadings);
    void spellCheck(const QString &text);
};

MarkdownHighlighter::MarkdownHighlighter
(
    MarkdownEditor *editor,
    const ColorScheme &colors
) : QSyntaxHighlighter(editor),
    d_ptr(new MarkdownHighlighterPrivate(this))
{
    Q_D(MarkdownHighlighter);

    d->colors = colors;
    d->editor = editor;
    d->spellCheckEnabled = false;
    d->typingPaused = true;
    d->useUndlerlineForEmphasis = false;
    d->italicizeBlockquotes = false;
    d->inBlockquote = false;

    setDocument(editor->document());
    d->referenceDefinitionRegex.setPattern("^\\s*\\[(.+?)[^\\\\]\\]:");
    d->inlineHtmlCommentRegex.setPattern("^\\s*<\\!--.*-->\\s*$");

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
    d->defaultFormat.setFont(font);
    d->defaultFormat.setForeground(QBrush(d->colors.foreground));
}

MarkdownHighlighter::~MarkdownHighlighter()
{
    ;
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
void MarkdownHighlighter::highlightBlock(const QString &text)
{
    Q_D(MarkdownHighlighter);

    int line = currentBlock().blockNumber() + 1;
    int oldState = currentBlock().userState();

    MarkdownAST *ast = ((MarkdownDocument *) this->document())->markdownAST();
    MarkdownNode *node = nullptr;

    if (nullptr != ast) {
        node = ast->findBlockAtLine(line);
    }

    if ((nullptr != node) && (MarkdownNode::Invalid != node->type())) {
        d->applyFormattingForNode(node);
    } else {
        setFormat(0, currentBlock().length(), d->colors.foreground);

        if (currentBlock().text().trimmed().isEmpty()) {
            setCurrentBlockState(MarkdownStateParagraphBreak);
        } else if (d->referenceDefinitionRegex.match(currentBlock().text()).hasMatch()) {
            QTextCharFormat format = d->defaultFormat;
            format.setForeground(d->colors.link);

            setFormat(0, currentBlock().text().indexOf(':'), format);
            setCurrentBlockState(MarkdownStateParagraph);
        } else if (d->inlineHtmlCommentRegex.match(currentBlock().text()).hasMatch()) {
            QTextCharFormat format = d->defaultFormat;
            format.setForeground(d->colors.inlineHtml);
            setFormat(0, currentBlock().text().length(), format);

            if (previousBlockState() != MarkdownStateUnknown) {
                setCurrentBlockState(previousBlockState());
            } else {
                setCurrentBlockState(MarkdownStateParagraph);
            }
        }
    }

    if (d->isSetextHeadingState(oldState) && !d->isSetextHeadingState(currentBlockState())) {
        QTextBlock block = currentBlock();

        while
        (
            block.previous().isValid()
            && (d->isSetextHeadingState(block.previous().userState()))
        ) {
            block = block.previous();
        }

        if (currentBlock() != block) {
            emit highlightBlockAtPosition(block.position());
        }
    } else if
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
    ) {
        emit highlightBlockAtPosition(currentBlock().previous().position());
    }

    // Highlight last two spaces of the line to indicate line breaks.
    //
    QRegularExpression whitespaceRegex("(\\s+)");
    QRegularExpressionMatchIterator matchIter = whitespaceRegex.globalMatch(currentBlock().text());

    while (matchIter.hasNext()) {
        QRegularExpressionMatch match = matchIter.next();
        QTextCharFormat format = this->format(match.capturedStart());

        format.setForeground(Qt::transparent);
        this->setFormat(match.capturedStart(), match.capturedLength(), format);
    }

    if (currentBlock().text().endsWith("  ")) {
        QTextCharFormat format = this->format(currentBlock().text().length() - 2);
        format.setForeground(d->colors.listMarkup);
        this->setFormat(currentBlock().text().length() - 2, 2, format);
    }

    if (d->spellCheckEnabled) {
        d->spellCheck(text);
    }
}

void MarkdownHighlighter::setDictionary(const DictionaryRef &dictionary)
{
    Q_D(MarkdownHighlighter);

    d->dictionary = dictionary;

    if (d->spellCheckEnabled) {
        rehighlight();
    }
}

void MarkdownHighlighter::increaseFontSize()
{
    Q_D(MarkdownHighlighter);

    d->defaultFormat.setFontPointSize(d->defaultFormat.fontPointSize() + 1.0);
    rehighlight();
}

void MarkdownHighlighter::decreaseFontSize()
{
    Q_D(MarkdownHighlighter);

    d->defaultFormat.setFontPointSize(d->defaultFormat.fontPointSize() - 1.0);
    rehighlight();
}

void MarkdownHighlighter::setColorScheme(const ColorScheme &colors)
{
    Q_D(MarkdownHighlighter);

    d->colors = colors;
    d->defaultFormat.setForeground(QBrush(colors.foreground));
    rehighlight();
}

void MarkdownHighlighter::setEnableLargeHeadingSizes(const bool enable)
{
    Q_D(MarkdownHighlighter);

    d->useLargeHeadings = enable;
    rehighlight();
}

void MarkdownHighlighter::setUseUnderlineForEmphasis(const bool enable)
{
    Q_D(MarkdownHighlighter);

    d->useUndlerlineForEmphasis = enable;
    rehighlight();
}

void MarkdownHighlighter::setItalicizeBlockquotes(const bool enable)
{
    Q_D(MarkdownHighlighter);

    d->italicizeBlockquotes = enable;
    rehighlight();
}

void MarkdownHighlighter::setFont(const QString &fontFamily, const double fontSize)
{
    Q_D(MarkdownHighlighter);

    QFont font;
    font.setFamily(fontFamily);
    font.setWeight(QFont::Normal);
    font.setItalic(false);
    font.setPointSizeF(fontSize);
    d->defaultFormat.setFont(font);

    rehighlight();
}

void MarkdownHighlighter::setSpellCheckEnabled(const bool enabled)
{
    Q_D(MarkdownHighlighter);

    d->spellCheckEnabled = enabled;
    rehighlight();
}

void MarkdownHighlighter::onTypingResumed()
{
    Q_D(MarkdownHighlighter);

    d->typingPaused = false;
}

void MarkdownHighlighter::onTypingPaused()
{
    Q_D(MarkdownHighlighter);

    d->typingPaused = true;

    if (d->spellCheckEnabled) {
        QTextBlock block = document()->findBlock(d->editor->textCursor().position());
        rehighlightBlock(block);
    }
}

void MarkdownHighlighter::onCursorPositionChanged()
{
    Q_D(MarkdownHighlighter);

    if
    (
        d->spellCheckEnabled &&
        (d->currentLine != d->editor->textCursor().block())
    ) {
        rehighlightBlock(d->currentLine);
    }

    d->currentLine = d->editor->textCursor().block();
}

void MarkdownHighlighter::onHighlightBlockAtPosition(int position)
{
    QTextBlock block = document()->findBlock(position);
    rehighlightBlock(block);
}

void MarkdownHighlighterPrivate::spellCheck(const QString &text)
{
    Q_Q(MarkdownHighlighter);

    int cursorPosition = editor->textCursor().position();
    QTextBlock cursorPosBlock = q->document()->findBlock(cursorPosition);
    int cursorPosInBlock = -1;

    if (q->currentBlock() == cursorPosBlock) {
        cursorPosInBlock = cursorPosition - cursorPosBlock.position();
    }

    QStringRef misspelledWord = dictionary.check(text, 0);

    while (!misspelledWord.isNull()) {
        int startIndex = misspelledWord.position();
        int length = misspelledWord.length();

        if (typingPaused || (cursorPosInBlock != (startIndex + length))) {
            QTextCharFormat spellingErrorFormat = q->format(startIndex);
            spellingErrorFormat.setUnderlineColor(colors.error);
            spellingErrorFormat.setUnderlineStyle
            (
                (QTextCharFormat::UnderlineStyle)
                QApplication::style()->styleHint
                (
                    QStyle::SH_SpellCheckUnderlineStyle
                )
            );

            q->setFormat(startIndex, length, spellingErrorFormat);
        }

        startIndex += length;
        misspelledWord = dictionary.check(text, startIndex);
    }
}

void MarkdownHighlighterPrivate::applyFormattingForNode(const MarkdownNode *const node)
{
    Q_Q(MarkdownHighlighter);

    MarkdownNode::NodeType type = node->type();
    int pos = node->position();
    int length = node->length();
    int currentLine = q->currentBlock().blockNumber() + 1;
    MarkdownState state = MarkdownStateParagraphBreak;

    QTextCharFormat baseFormat = defaultFormat;
    baseFormat.setForeground(colors.foreground);

    unsigned int indent = 0;
    QString text = q->currentBlock().text();

    for (int i = 0; i < text.length(); i++) {
        if (text[i].isSpace()) {
            indent++;
        } else {
            break;
        }
    }

    bool inBlockquote = node->isInsideBlockquote();

    if (inBlockquote) {
        baseFormat.setForeground(colors.blockquoteMarkup);
        baseFormat.setFontItalic(italicizeBlockquotes);

        q->setFormat
        (
            0,
            q->currentBlock().length(),
            baseFormat
        );

        baseFormat.setForeground(colors.blockquoteText);
    } else {
        q->setFormat
        (
            0,
            q->currentBlock().length(),
            baseFormat
        );
    }

    // Do a pre-order traversal of the nodes.
    QStack<const MarkdownNode *> nodes;
    QStack<QTextCharFormat> nodeFormats;
    nodes.push(node);
    nodeFormats.push(baseFormat);

    while (!nodes.isEmpty()) {
        const MarkdownNode *current = nodes.pop();
        QTextCharFormat contextFormat = nodeFormats.pop();
        MarkdownNode::NodeType parentType = MarkdownNode::Invalid;

        if (nullptr != current->parent()) {
            parentType = current->parent()->type();
        }

        pos = columnInLine(current, q->currentBlock().text());
        length = current->length();
        type = current->type();

        if (lineMatchesNode(currentLine, current)) {

            if
            (
                (MarkdownNode::FootnoteDefinition == parentType)
                || (MarkdownNode::FootnoteReference == parentType)
            ) {
                type = parentType;
            }

            QTextCharFormat format = contextFormat;

            switch (type) {
            case MarkdownNode::Heading:
                length = q->currentBlock().length();
                format.setFontWeight(QFont::Bold);
                contextFormat.setFontWeight(QFont::Bold);

                if (useLargeHeadings) {
                    format.setFontPointSize(format.fontPointSize()
                                            + (qreal)(7 - current->headingLevel()));
                    contextFormat.setFontPointSize(format.fontPointSize());
                }

                if (inBlockquote) {
                    format.setForeground(colors.blockquoteMarkup);
                    contextFormat.setForeground(colors.blockquoteText);
                } else {
                    format.setForeground(colors.headingMarkup);
                    contextFormat.setForeground(colors.headingText);
                }

                if (current->isSetextHeading()) {
                    switch (current->headingLevel()) {
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
                    if (currentLine != current->startLine()) {
                        QTextBlock block = q->document()->findBlockByNumber(current->startLine() - 1);

                        if (block.isValid()) {
                            emit q->highlightBlockAtPosition(block.position());
                        }
                    }
                } else {
                    switch (current->headingLevel()) {
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
                if (MarkdownStateUnknown == state) {
                    state = MarkdownStateParagraph;
                }

                break;
            case MarkdownNode::BlockQuote:
                format.setForeground(colors.blockquoteMarkup);
                format.setFontItalic(italicizeBlockquotes);
                contextFormat.setForeground(colors.blockquoteText);
                contextFormat.setFontItalic(italicizeBlockquotes);
                inBlockquote = true;
                break;
            case MarkdownNode::CodeBlock:
                if
                (
                    current->isFencedCodeBlock()
                    &&
                    (
                        ((q->currentBlock().blockNumber() + 1) == current->startLine())
                        || ((q->currentBlock().blockNumber() + 1) == current->endLine())
                    )
                ) {
                    format.setForeground(colors.codeMarkup);
                    state = MarkdownStateCodeBlock;
                } else if
                (
                    ((q->currentBlock().blockNumber() + 1) == current->endLine())
                    && (current->length() <= 0)
                ) {
                    state = MarkdownStateParagraphBreak;
                } else {
                    format.setForeground(colors.codeText);
                    length = q->currentBlock().length() - pos + 1;
                    state = MarkdownStateCodeBlock;
                }

                break;
            case MarkdownNode::ListItem:
                format.setForeground(colors.listMarkup);
                format.setFontWeight(QFont::Bold);

                if (current->isNumberedListItem()) {
                    state = MarkdownStateNumberedList;
                } else { // Assume bullet list item
                    state = MarkdownStateBulletPointList;
                }

                break;
            case MarkdownNode::TaskListItem:
                state = MarkdownStateTaskList;
                format.setForeground(colors.listMarkup);
                format.setFontWeight(QFont::Bold);
                break;
            case MarkdownNode::Emph:
                format.setForeground(colors.emphasisMarkup);

                if (useUndlerlineForEmphasis) {
                    contextFormat.setFontUnderline(true);
                } else {
                    contextFormat.setFontItalic(true);
                    format.setFontItalic(true);
                }

                contextFormat.setForeground(colors.emphasisText);
                break;
            case MarkdownNode::Strong:
                contextFormat.setForeground(colors.emphasisText);
                contextFormat.setFontWeight(QFont::Bold);
                format.setForeground(colors.emphasisMarkup);
                format.setFontWeight(QFont::Bold);
                break;
            case MarkdownNode::Code: {
                int backticks = 0;

                for (int i = pos - 1; i >= 0; i--) {
                    if (text[i] == QChar('`')) {
                        backticks++;
                    } else {
                        break;
                    }
                }

                format.setForeground(colors.codeMarkup);
                q->setFormat
                (
                    pos - backticks,
                    length + (2 * backticks),
                    format
                );
                format.setForeground(colors.codeText);
                break;
            }
            case MarkdownNode::HtmlInline:
                format.setForeground(colors.inlineHtml);
                contextFormat.setForeground(colors.inlineHtml);
                break;
            case MarkdownNode::Link:
                format.setForeground(colors.link);
                contextFormat.setForeground(colors.link);
                break;
            case MarkdownNode::Image:
                format.setForeground(colors.image);
                contextFormat.setForeground(colors.image);
                break;
            case MarkdownNode::ThematicBreak:
                format.setForeground(colors.divider);
                state = MarkdownStateHorizontalRule;
                break;
            case MarkdownNode::FootnoteReference:
                format.setForeground(colors.link);
                contextFormat.setForeground(colors.link);
                break;
            case MarkdownNode::FootnoteDefinition:
                format.setForeground(colors.link);
                contextFormat.setForeground(colors.link);
                state = MarkdownStateParagraph;
                break;
            case MarkdownNode::TableHeading:
                format.setForeground(colors.emphasisMarkup);
                pos = 0;
                length = q->currentBlock().length();
                contextFormat.setFontWeight(QFont::Bold);
                state = MarkdownStatePipeTableHeader;
                break;
            case MarkdownNode::TableRow:
                format.setForeground(colors.emphasisMarkup);
                pos = 0;
                length = q->currentBlock().length();
                state = MarkdownStatePipeTableRow;
                break;
            case MarkdownNode::TableCell:
                format = contextFormat;

                if
                (
                    (nullptr != current->parent())
                    && (MarkdownNode::TableHeading == current->parent()->type())
                ) {
                    format.setFontWeight(QFont::Bold);
                }
                break;
            case MarkdownNode::Table:
                format.setForeground(colors.emphasisMarkup);
                pos = 0;
                length = q->currentBlock().length();
                state = MarkdownStatePipeTableDivider;
                break;
            case MarkdownNode::Strikethrough:
                format.setForeground(colors.emphasisMarkup);
                contextFormat.setFontStrikeOut(true);
                break;
            default:
                if (referenceDefinitionRegex.match(q->currentBlock().text()).hasMatch()) {
                    pos = 0;
                    length = q->currentBlock().text().indexOf(':') + 1;
                    format.setForeground(colors.link);
                } else {
                    format.setForeground(colors.blockquoteMarkup);
                }

                break;
            }

            if ((length <= 0) || (length > q->currentBlock().length())) {
                length = q->currentBlock().length();
            }

            q->setFormat
            (
                pos,
                length,
                format
            );

            if (MarkdownNode::Text == type) {
                highlightRefLinks(pos, length);
            } else if (MarkdownNode::TaskListItem == type) {
                format = contextFormat;
                format.setForeground(colors.link);

                int checkboxStart = text.indexOf('[');
                int checkboxEnd = text.indexOf(']');

                q->setFormat
                (
                    checkboxStart,
                    checkboxEnd - checkboxStart + 1,
                    format
                );
            }
        }

        MarkdownNode *child = current->lastChild();

        while ((nullptr != child) && (!child->isInvalid())) {
            nodes.push(child);
            nodeFormats.push(contextFormat);
            child = child->previous();
        }
    }

    if (MarkdownStateUnknown != state) {
        state |= indent;

        if (inBlockquote) {
            state |= MarkdownStateBlockquote;
        }

        q->setCurrentBlockState(state);
    }
}

int MarkdownHighlighterPrivate::columnInLine(const MarkdownNode *const node, const QString &lineText) const
{
    MarkdownNode::NodeType prevType = MarkdownNode::Invalid;

    if (nullptr != node->previous()) {
        prevType = node->previous()->type();
    }

    static int offset = 0;

    if (node->isBlockType()) {
        offset = 0;
    } else if
    (
        (MarkdownNode::Softbreak == prevType)
        || (MarkdownNode::Linebreak == prevType)
    ) {
        int pos = 0;
        QString text = lineText;

        switch (node->type()) {
        case MarkdownNode::Text:
            pos = text.indexOf(node->text()[0]);

            if (node->text().startsWith('`')) {
                int retValue = pos;
                pos += node->text().length() - node->length();
                offset = node->position() - pos;
                return retValue;
            }

            break;
        case MarkdownNode::Code:
            pos = text.indexOf('`');

            for (int i = pos; i < text.length(); i++) {
                if (text[i] != '`') {
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
            pos = text.indexOf(node->text()[0]);
            break;
        }

        offset = node->position() - pos;
    }

    return node->position() - offset;
}

void MarkdownHighlighterPrivate::highlightRefLinks(const int pos, const int length)
{
    Q_Q(MarkdownHighlighter);

    QStack<int> bracketPos;
    bool skipNext = false;
    QTextCharFormat format = q->format(pos);
    format.setForeground(colors.link);

    for (int i = pos; i < (pos + length) && (i < q->currentBlock().text()); i++) {
        if (skipNext) {
            skipNext = false;
            continue;
        }

        switch (q->currentBlock().text()[i].toLatin1()) {
        case '\\':
            skipNext = true;
            break;
        case '[':
            bracketPos.push(i);
            break;
        case ']':
            if (!bracketPos.isEmpty()) {
                int start = bracketPos.pop();

                q->setFormat(start, (i - start + 1), format);
            }

            break;
        default:
            break;
        }
    }
}

bool MarkdownHighlighterPrivate::lineMatchesNode(const int line, const MarkdownNode *const node) const
{
    return
        (
            (
                node->isBlockType()
                && (line >= node->startLine())
                &&
                (
                    (line <= node->endLine())
                    || (0 == node->endLine())
                )
            )
            ||
            (
                node->isInlineType()
                &&
                (
                    (line == node->startLine())
                    || (line == node->endLine())
                    || (0 == node->endLine())
                )
            )
        );
}

bool MarkdownHighlighterPrivate::isSetextHeadingState(const int state)
{
    switch (state & MarkdownStateMask) {
    case MarkdownStateSetextHeading1:
    case MarkdownStateSetextHeading2:
        return true;
    default:
        return false;
    }
}
} // namespace ghostwriter
