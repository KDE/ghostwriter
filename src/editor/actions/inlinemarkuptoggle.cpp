/*
 * SPDX-FileCopyrightText: 2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <tuple>

#include <QChar>
#include <QList>
#include <QPair>
#include <QString>
#include <QTextCharFormat>
#include <QTextLayout>

#include "../textblockdata.h"

#include "inlinemarkuptoggle.h"

namespace ghostwriter
{
typedef struct
{
    int start;
    int end;
} MarkupRange;

class InlineMarkupTogglePrivate
{
public:

    InlineMarkupTogglePrivate(MarkdownNode::NodeType type);
    ~InlineMarkupTogglePrivate();

    std::tuple<int, int> surroundingMarkup(const QTextCursor &cursor) const;
    bool isWordChar(const QChar &ch) const;
    bool insideWord(const QTextCursor &cursor) const;
    void formatSelection(QTextCursor &cursor);
    void formatWord(QTextCursor &cursor);
    void formatPosition(QTextCursor &cursor);
    void removeFormatting(QTextCursor &cursor, int start, int end);

    QString markup;
    MarkdownNode::NodeType type;

};

InlineMarkupToggle::InlineMarkupToggle(MarkdownNode::NodeType type)
    : d(new InlineMarkupTogglePrivate(type))
{
    ;
}

InlineMarkupToggle::~InlineMarkupToggle()
{
    ;
}

void InlineMarkupToggle::operator()(MarkdownEditor *editor)
{
    QTextCursor cursor = editor->textCursor();

    // Look for formatting around the cursor position that has emphasis styling.
    const auto [ markupStart, markupEnd ] = d->surroundingMarkup(cursor);

    // If existing markup surrounding the text cursor was not found,
    // then apply the markup.
    if (markupStart < 0) {

        // If text in the editor is selected, insert the emphasis markup
        // around the selection.
        if (cursor.hasSelection()) {
            d->formatSelection(cursor);
        }
        else {
            // If the cursor is on the boundary of or within a word,
            // insert markup around the word.
            if (d->insideWord(cursor)) {
                d->formatWord(cursor);
            }
            // Otherwise, insert the markup in-place.
            else {
                d->formatPosition(cursor);
            }
        }
    }
    // Otherwise, remove the existing markup surrounding the text cursor.
    else {
        d->removeFormatting(cursor, markupStart, markupEnd);
    }

    editor->setTextCursor(cursor);
}


InlineMarkupTogglePrivate::InlineMarkupTogglePrivate(
    MarkdownNode::NodeType type) : type(type)
{
    switch (type)
    {
    case MarkdownNode::Emph:
        markup = "*";
        break;
    case MarkdownNode::Strong:
        markup = "**";
        break;
    case MarkdownNode::Strikethrough:
        markup = "~~";
        break;
    default:
        markup = "";
        break;
    }
}

InlineMarkupTogglePrivate::~InlineMarkupTogglePrivate()
{
    ;
}

std::tuple<int, int>
InlineMarkupTogglePrivate::surroundingMarkup(const QTextCursor &cursor) const
{
    TextBlockData *data = (TextBlockData*) cursor.block().userData();

    if ((nullptr == data) || data->markup.isEmpty()) {
        return { -1, -1 };
    }

    int pos = cursor.positionInBlock();
    int matchStart = -1;
    int matchEnd = cursor.block().length() + 1;

    for (TextBlockData::MarkupRange &range : data->markup) {
        if ((range.start <= pos)
                && (pos <= (range.end + 1))
                && (type == range.type)
                && (matchStart <= range.start)
                && (matchEnd >= range.end)) {
            matchStart = range.start;
            matchEnd = range.end;
        }
    }

    if (matchStart >= 0) {
        matchStart += cursor.block().position();
        matchEnd += cursor.block().position();
    }

    return { matchStart, matchEnd };
}

bool InlineMarkupTogglePrivate::isWordChar(const QChar & ch) const
{
    return (!ch.isSpace() && !ch.isPunct());
}

bool InlineMarkupTogglePrivate::insideWord(const QTextCursor &cursor) const
{
    auto text = cursor.block().text();

    if (text.length() <= 0) {
        return false;
    }

    int pos = cursor.positionInBlock();

    return ((pos < (text.length() - 1) && isWordChar(text[pos]))
            || ((pos > 0) && isWordChar(text[pos - 1]))
            || ((pos < (text.length() - 2)) && isWordChar(text[pos + 1])));
}

void InlineMarkupTogglePrivate::formatSelection(QTextCursor &cursor)
{
    QTextCursor oldCursor = cursor;

    cursor.beginEditBlock();
    cursor.setPosition(oldCursor.selectionStart());
    cursor.insertText(markup);
    cursor.setPosition(oldCursor.selectionEnd());
    cursor.insertText(markup);
    cursor.endEditBlock();

    int start = oldCursor.anchor();
    int end = oldCursor.position();

    if (start > end) {
        start -= markup.length();
    } else {
        end -= markup.length();
    }

    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
}

void InlineMarkupTogglePrivate::formatWord(QTextCursor &cursor)
{
    QTextCursor oldCursor = cursor;
    int start = 0;
    int end = 0;

    cursor.movePosition(QTextCursor::StartOfWord);
    start = cursor.position();
    
    cursor.movePosition(QTextCursor::EndOfWord);
    end = cursor.position();

    cursor.beginEditBlock();
    cursor.setPosition(end);
    cursor.insertText(markup);
    cursor.setPosition(start);
    cursor.insertText(markup);
    cursor.endEditBlock();
    cursor = oldCursor;
}

void InlineMarkupTogglePrivate::formatPosition(QTextCursor &cursor)
{
    cursor.beginEditBlock();
    cursor.insertText(markup);
    cursor.insertText(markup);
    cursor.endEditBlock();
    cursor.setPosition(cursor.position() - markup.length());
}

void InlineMarkupTogglePrivate::removeFormatting(
    QTextCursor &cursor,
    int start,
    int end)
{
    QTextCursor oldCursor = cursor;

    cursor.beginEditBlock();
    cursor.setPosition(end);

    for (int i = 0; i < markup.length(); i++) {
        cursor.deletePreviousChar();
    }

    cursor.setPosition(start);

    for (int i = 0; i < markup.length(); i++) {
        cursor.deleteChar();
    }

    cursor.endEditBlock();
    cursor = oldCursor;
}

} // namespace ghostwriter
