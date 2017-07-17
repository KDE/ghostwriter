/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
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

#include <QString>
#include <QChar>
#include <QRegExp>

#include "MarkdownTokenizer.h"
#include "MarkdownStates.h"

// This character is used to replace escape characters and other characters
// with special meaning in a dummy copy of the current line being parsed,
// for ease of parsing.
//
static const QChar DUMMY_CHAR('$');

static const int MAX_MARKDOWN_HEADING_LEVEL = 6;


MarkdownTokenizer::MarkdownTokenizer()
{
    paragraphBreakRegex.setPattern("^\\s*$");
    heading1SetextRegex.setPattern("^===+\\s*$");
    heading2SetextRegex.setPattern("^---+\\s*$");
    blockquoteRegex.setPattern("^ {0,3}>.*$");
    githubCodeFenceStartRegex.setPattern("^```+.*$");
    githubCodeFenceEndRegex.setPattern("^```+\\s*$");
    pandocCodeFenceStartRegex.setPattern("^~~~+.*$");
    pandocCodeFenceEndRegex.setPattern("^~~~+\\s*$");
    numberedListRegex.setPattern("^ {0,3}[0-9]+[.)]\\s+.*$");
    numberedNestedListRegex.setPattern("^\\s*[0-9]+[.)]\\s+.*$");
    hruleRegex.setPattern("\\s*(\\*\\s*){3,}|(\\s*(_\\s*){3,})|((\\s*(-\\s*){3,}))");
    lineBreakRegex.setPattern(".*\\s{2,}$");
    emphasisRegex.setPattern("(\\*(?![\\s*]).*[^\\s*]\\*)|_(?![\\s_]).*[^\\s_]_");
    emphasisRegex.setMinimal(true);
    strongRegex.setPattern("\\*\\*(?=\\S).*\\S\\*\\*(?!\\*)|__(?=\\S).*\\S__(?!_)");
    strongRegex.setMinimal(true);
    strikethroughRegex.setPattern("~~[^\\s]+.*[^\\s]+~~");
    strikethroughRegex.setMinimal(true);
    verbatimRegex.setPattern("`+");
    htmlTagRegex.setPattern("<[^<>]+>");
    htmlTagRegex.setMinimal(true);
    htmlEntityRegex.setPattern("&[a-zA-Z]+;|&#x?[0-9]+;");
    automaticLinkRegex.setPattern("(<[a-zA-Z]+\\:.+>)|(<.+@.+>)");
    automaticLinkRegex.setMinimal(true);
    inlineLinkRegex.setPattern("\\[.+\\]\\(.+\\)");
    inlineLinkRegex.setMinimal(true);
    referenceLinkRegex.setPattern("\\[(.+)\\]");
    referenceLinkRegex.setMinimal(true);
    referenceDefinitionRegex.setPattern("^\\s*\\[.+\\]:");
    imageRegex.setPattern("!\\[.*\\]\\(.+\\)");
    imageRegex.setMinimal(true);
    htmlInlineCommentRegex.setPattern("<!--.*-->");
    htmlInlineCommentRegex.setMinimal(true);
    mentionRegex.setPattern("\\B@\\w+(\\-\\w+)*(/\\w+(\\-\\w+)*)?");
    pipeTableDividerRegex.setPattern("^ {0,3}(\\|[ :]?)?-{3,}([ :]?\\|[ :]?-{3,}([ :]?\\|)?)+\\s*$");
}

MarkdownTokenizer::~MarkdownTokenizer()
{
    ;
}

void MarkdownTokenizer::tokenize
(
    const QString& text,
    int currentState,
    int previousState,
    const int nextState
)
{
    this->currentState = currentState;
    this->previousState = previousState;
    this->nextState = nextState;

    if
    (
        tokenizeCodeBlock(text)
    )
    {
        ; // No further tokenizing required
    }
    else if
    (
        (MarkdownStateComment != previousState)
        && paragraphBreakRegex.exactMatch(text)
    )
    {
        if
        (
            (MarkdownStateListLineBreak == previousState)
            || (MarkdownStateNumberedList == previousState)
            || (MarkdownStateBulletPointList == previousState)
        )
        {
            setState(MarkdownStateListLineBreak);
        }
        else if
        (
            (MarkdownStateCodeBlock != previousState)
            ||
            (
                !text.startsWith("\t")
                && !text.endsWith("    ")
            )
        )
        {
            setState(MarkdownStateParagraphBreak);
        }
    }
    else if
    (
        tokenizeSetextHeadingLine2(text)
        || tokenizeMultilineComment(text)
        || tokenizeHorizontalRule(text)
        || tokenizeTableDivider(text)
    )
    {
        ; // No further tokenizing required
    }
    else if
    (
        tokenizeAtxHeading(text)
        || tokenizeSetextHeadingLine1(text)
        || tokenizeBlockquote(text)
        || tokenizeNumberedList(text)
        || tokenizeBulletPointList(text)
    )
    {
        tokenizeLineBreak(text);
        tokenizeInline(text);
    }
    else
    {
        if
        (
            (MarkdownStateListLineBreak == previousState)
            || (MarkdownStateNumberedList == previousState)
            || (MarkdownStateBulletPointList == previousState)
        )
        {
            if
            (
                !tokenizeNumberedList(text)
                && !tokenizeBulletPointList(text)
                && (text.startsWith("\t") || text.startsWith("    "))
            )
            {
                setState(previousState);
            }
            else
            {
                setState(MarkdownStateParagraph);
            }
        }
        else
        {
            setState(MarkdownStateParagraph);
        }

        tokenizeLineBreak(text);
        tokenizeInline(text);
    }

    // Make sure that if the second line of a setext heading is removed the
    // first line is reprocessed.  Otherwise, it will still show up in the
    // document as a heading.
    //
    if
    (
        (
            (previousState == MarkdownStateSetextHeading1Line1)
            && (this->getState() != MarkdownStateSetextHeading1Line2)
        )
        ||
        (
            (previousState == MarkdownStateSetextHeading2Line1)
            && (this->getState() != MarkdownStateSetextHeading2Line2)
        )
    )
    {
        this->requestBacktrack();
    }
}

bool MarkdownTokenizer::tokenizeSetextHeadingLine1
(
    const QString& text
)
{
    // Check the next line's state to see if this is a setext-style heading.
    //
    int level = 0;
    Token token;

    if (MarkdownStateSetextHeading1Line2 == nextState)
    {
        level = 1;
        this->setState(MarkdownStateSetextHeading1Line1);
        token.setType(TokenSetextHeading1Line1);
    }
    else if (MarkdownStateSetextHeading2Line2 == nextState)
    {
        level = 2;
        this->setState(MarkdownStateSetextHeading2Line1);
        token.setType(TokenSetextHeading2Line1);
    }

    if (level > 0)
    {
        token.setLength(text.length());
        token.setPosition(0);
        this->addToken(token);
        return true;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeSetextHeadingLine2
(
    const QString& text
)
{
    int level = 0;
    bool setextMatch = false;
    Token token;

    if (MarkdownStateSetextHeading1Line1 == previousState)
    {
        level = 1;
        setextMatch = heading1SetextRegex.exactMatch(text);
        setState(MarkdownStateSetextHeading1Line2);
        token.setType(TokenSetextHeading1Line2);
    }
    else if (MarkdownStateSetextHeading2Line1 == previousState)
    {
        level = 2;
        setextMatch = heading2SetextRegex.exactMatch(text);
        setState(MarkdownStateSetextHeading2Line2);
        token.setType(TokenSetextHeading2Line2);
    }
    else if (MarkdownStateParagraph == previousState)
    {
        bool h1Line2 = heading1SetextRegex.exactMatch(text);
        bool h2Line2 = heading2SetextRegex.exactMatch(text);

        if (h1Line2 || h2Line2)
        {
            // Restart tokenizing on the previous line.
            this->requestBacktrack();
            token.setLength(text.length());
            token.setPosition(0);

            if (h1Line2)
            {
                setState(MarkdownStateSetextHeading1Line2);
                token.setType(TokenSetextHeading1Line2);
            }
            else
            {
                setState(MarkdownStateSetextHeading2Line2);
                token.setType(TokenSetextHeading2Line2);
            }

            this->addToken(token);
            return true;
        }
    }

    if (level > 0)
    {
        if (setextMatch)
        {
            token.setLength(text.length());
            token.setPosition(0);
            this->addToken(token);
            return true;
        }
        else
        {
            // Restart tokenizing on the previous line.
            this->requestBacktrack();
            return false;
        }
    }

    return false;
}

bool MarkdownTokenizer::tokenizeAtxHeading(const QString& text)
{
    QString escapedText = dummyOutEscapeCharacters(text);
    int trailingPoundCount = 0;

    int level = 0;

    // Count the number of pound signs at the front of the string,
    // up to the maximum allowed, to determine the heading level.
    //
    for
    (
        int i = 0;
        ((i < escapedText.length()) && (i < MAX_MARKDOWN_HEADING_LEVEL));
        i++
    )
    {
        if (QChar('#') == escapedText[i])
        {
            level++;
        }
        else
        {
            // We're done counting, as no more pound signs are available.
            break;
        }
    }

    if ((level > 0) && (level < text.length()))
    {
        // Count how many pound signs are at the end of the text.
        for (int i = escapedText.length() - 1; i > level; i--)
        {
            if (QChar('#') == escapedText[i])
            {
                trailingPoundCount++;
            }
            else
            {
                // We're done counting, as no more pound signs are available.
                break;
            }
        }

        Token token;
        token.setPosition(0);
        token.setLength(text.length());
        token.setType((MarkdownTokenType) (TokenAtxHeading1 + level - 1));
        token.setOpeningMarkupLength(level);
        token.setClosingMarkupLength(trailingPoundCount);
        this->addToken(token);
        setState(MarkdownStateAtxHeading1 + level - 1);
        return true;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeNumberedList
(
    const QString& text
)
{
    if
    (
        (
            (
                (MarkdownStateParagraphBreak == previousState)
                || (MarkdownStateUnknown == previousState)
                || (MarkdownStateCodeBlock == previousState)
                || (MarkdownStateCodeFenceEnd == previousState)
            )
            && numberedListRegex.exactMatch(text)
        )
        ||
        (
            (
                (MarkdownStateListLineBreak == previousState)
                || (MarkdownStateNumberedList == previousState)
                || (MarkdownStateBulletPointList == previousState)
            )
            && numberedNestedListRegex.exactMatch(text)
        )
    )
    {
        int periodIndex = text.indexOf(QChar('.'));
        int parenthIndex = text.indexOf(QChar(')'));
        int index = -1;

        if (periodIndex < 0)
        {
            index = parenthIndex;
        }
        else if (parenthIndex < 0)
        {
            index = periodIndex;
        }
        else if (parenthIndex > periodIndex)
        {
            index = periodIndex;
        }
        else
        {
            index = parenthIndex;
        }

        if (index >= 0)
        {
            Token token;
            token.setType(TokenNumberedList);
            token.setPosition(0);
            token.setLength(text.length());
            token.setOpeningMarkupLength(index + 1);
            this->addToken(token);
            setState(MarkdownStateNumberedList);
            return true;
        }

        return false;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeBulletPointList
(
    const QString& text
)
{
    bool foundBulletChar = false;
    int bulletCharIndex = -1;
    int spaceCount = 0;
    bool whitespaceFoundAfterBulletChar = false;

    if
    (
        (MarkdownStateUnknown != previousState)
        && (MarkdownStateParagraphBreak != previousState)
        && (MarkdownStateListLineBreak != previousState)
        && (MarkdownStateNumberedList != previousState)
        && (MarkdownStateBulletPointList != previousState)
        && (MarkdownStateCodeBlock != previousState)
        && (MarkdownStateCodeFenceEnd != previousState)
    )
    {
        return false;
    }

    // Search for the bullet point character, which can
    // be either a '+', '-', or '*'.
    //
    for (int i = 0; i < text.length(); i++)
    {
        if (QChar(' ') == text[i])
        {
            if (foundBulletChar)
            {
                // We've confirmed it's a bullet point by the whitespace that
                // follows the bullet point character, and can now exit the
                // loop.
                //
                whitespaceFoundAfterBulletChar = true;
                break;
            }
            else
            {
                spaceCount++;

                // If this list item is the first in the list, ensure the
                // number of spaces preceeding the bullet point does not
                // exceed three, as that would indicate a code block rather
                // than a bullet point list.
                //
                if
                (
                    (spaceCount > 3)
                    && (MarkdownStateNumberedList != previousState)
                    && (MarkdownStateBulletPointList != previousState)
                    && (MarkdownStateListLineBreak != previousState)
                    &&
                    (
                        (MarkdownStateParagraphBreak == previousState)
                        || (MarkdownStateUnknown == previousState)
                        || (MarkdownStateCodeBlock == previousState)
                        || (MarkdownStateCodeFenceEnd == previousState)
                    )
                )
                {
                    return false;
                }
            }
        }
        else if (QChar('\t') == text[i])
        {
            if (foundBulletChar)
            {
                // We've confirmed it's a bullet point by the whitespace that
                // follows the bullet point character, and can now exit the
                // loop.
                //
                whitespaceFoundAfterBulletChar = true;
                break;
            }
            else if
            (
                (MarkdownStateParagraphBreak == previousState)
                || (MarkdownStateUnknown == previousState)
            )
            {
                // If this list item is the first in the list, ensure that
                // no tab character preceedes the bullet point, as that would
                // indicate a code block rather than a bullet point list.
                //
                return false;
            }
        }
        else if
        (
            (QChar('+') == text[i])
            || (QChar('-') == text[i])
            || (QChar('*') == text[i])
        )
        {
            foundBulletChar = true;
            bulletCharIndex = i;
        }
        else
        {
            return false;
        }
    }

    if ((bulletCharIndex >= 0) && whitespaceFoundAfterBulletChar)
    {
        Token token;
        token.setType(TokenBulletPointList);
        token.setPosition(0);
        token.setLength(text.length());
        token.setOpeningMarkupLength(bulletCharIndex + 1);
        this->addToken(token);
        setState(MarkdownStateBulletPointList);
        return true;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeHorizontalRule(const QString& text)
{
    if (hruleRegex.exactMatch(text))
    {
        Token token;
        token.setType(TokenHorizontalRule);
        token.setPosition(0);
        token.setLength(text.length());
        this->addToken(token);
        setState(MarkdownStateHorizontalRule);
        return true;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeLineBreak(const QString& text)
{
    switch (currentState)
    {
        case MarkdownStateParagraph:
        case MarkdownStateBlockquote:
        case MarkdownStateNumberedList:
        case MarkdownStateBulletPointList:
            switch (previousState)
            {
                case MarkdownStateParagraph:
                case MarkdownStateBlockquote:
                case MarkdownStateNumberedList:
                case MarkdownStateBulletPointList:
                    this->requestBacktrack();
                    break;
            }

            switch (nextState)
            {
                case MarkdownStateParagraph:
                case MarkdownStateBlockquote:
                case MarkdownStateNumberedList:
                case MarkdownStateBulletPointList:
                    if (lineBreakRegex.exactMatch(text))
                    {
                        Token token;
                        token.setType(TokenLineBreak);
                        token.setPosition(text.length()-1);
                        token.setLength(1);
                        this->addToken(token);
                        return true;   
                    }
                    break;
            }
            break;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeBlockquote
(
    const QString& text
)
{
    if
    (
        (MarkdownStateBlockquote == previousState)
        || blockquoteRegex.exactMatch(text)
    )
    {
        // Find any '>' characters at the front of the line.
        int markupLength = 0;

        for (int i = 0; i < text.length(); i++)
        {
            if (QChar('>') == text[i])
            {
                markupLength = i + 1;
            }
            else if (!text[i].isSpace())
            {
                // There are no more '>' characters at the front of the line,
                // so stop processing.
                //
                break;
            }
        }

        Token token;
        token.setType(TokenBlockquote);
        token.setPosition(0);
        token.setLength(text.length());

        if (markupLength > 0)
        {
            token.setOpeningMarkupLength(markupLength);
        }

        addToken(token);
        setState(MarkdownStateBlockquote);
        return true;
    }

    return false;
}

bool MarkdownTokenizer::tokenizeCodeBlock
(
    const QString& text
)
{
    if
    (
        (MarkdownStateInGithubCodeFence == previousState)
        || (MarkdownStateInPandocCodeFence == previousState)
    )
    {
        setState(previousState);

        if
        (
            (
                (MarkdownStateInGithubCodeFence == previousState)
                && githubCodeFenceEndRegex.exactMatch(text)
            )
            ||
            (
                (MarkdownStateInPandocCodeFence == previousState)
                && pandocCodeFenceEndRegex.exactMatch(text)
            )
        )
        {
            Token token;
            token.setType(TokenCodeFenceEnd);
            token.setPosition(0);
            token.setLength(text.length());
            this->addToken(token);
            setState(MarkdownStateCodeFenceEnd);
        }
        else
        {
            Token token;
            token.setType(TokenCodeBlock);
            token.setPosition(0);
            token.setLength(text.length());
            this->addToken(token);
        }

        return true;
    }
    else if
    (
        (
            (MarkdownStateCodeBlock == previousState)
            || (MarkdownStateParagraphBreak == previousState)
            || (MarkdownStateUnknown == previousState)
        )
        && (text.startsWith("\t") || text.startsWith("    "))
    )
    {
        Token token;
        token.setType(TokenCodeBlock);
        token.setPosition(0);
        token.setLength(text.length());
        addToken(token);
        setState(MarkdownStateCodeBlock);
        return true;
    }
    else if
    (
        (MarkdownStateParagraphBreak == previousState)
        || (MarkdownStateParagraph == previousState)
        || (MarkdownStateUnknown == previousState)
        || (MarkdownStateListLineBreak == previousState)
        || (MarkdownStateCodeFenceEnd == previousState)
    )
    {
        bool foundCodeFenceStart = false;
        Token token;

        if (githubCodeFenceStartRegex.exactMatch(text))
        {
            foundCodeFenceStart = true;
            token.setType(TokenGithubCodeFence);
            setState(MarkdownStateInGithubCodeFence);
        }
        else if (pandocCodeFenceStartRegex.exactMatch(text))
        {
            foundCodeFenceStart = true;
            token.setType(TokenPandocCodeFence);
            setState(MarkdownStateInPandocCodeFence);
        }

        if (foundCodeFenceStart)
        {
            token.setPosition(0);
            token.setLength(text.length());
            addToken(token);
            return true;
        }
    }

    return false;
}

bool MarkdownTokenizer::tokenizeMultilineComment
(
    const QString& text
)
{
    if (MarkdownStateComment == previousState)
    {
        // Find the end of the comment, if any.
        int index = text.indexOf("-->");
        Token token;
        token.setType(TokenHtmlComment);
        token.setPosition(0);

        if (index >= 0)
        {
            token.setLength(index + 3);
            addToken(token);

            // Return false so that the rest of the line that isn't within
            // the commented segment can be highlighted as normal paragraph
            // text.
            //
            return false;
        }
        else
        {
            token.setLength(text.length());
            addToken(token);
            setState(MarkdownStateComment);
            return true;
        }
    }

    return false;
}

bool MarkdownTokenizer::tokenizeInline
(
    const QString& text
)
{
    QString escapedText = dummyOutEscapeCharacters(text);

    // Check if the line is a reference definition.
    if (referenceDefinitionRegex.exactMatch(escapedText))
    {
        int colonIndex = escapedText.indexOf(':');
        Token token;
        token.setType(TokenReferenceDefinition);
        token.setPosition(0);
        token.setLength(colonIndex + 1);
        addToken(token);

        // Replace the first bracket so that the '[...]:' reference definition
        // start doesn't get highlighted as a reference link.
        //
        int firstBracketIndex = escapedText.indexOf(QChar('['));

        if (firstBracketIndex >= 0)
        {
            escapedText[firstBracketIndex] = DUMMY_CHAR;
        }
    }

    tokenizeVerbatim(escapedText);
    tokenizeHtmlComments(escapedText);
    tokenizeTableHeaderRow(escapedText);
    tokenizeTableRow(escapedText);
    tokenizeMatches(TokenImage, escapedText, imageRegex, 0, 0, false, true);
    tokenizeMatches(TokenInlineLink, escapedText, inlineLinkRegex, 0, 0, false, true);
    tokenizeMatches(TokenReferenceLink, escapedText, referenceLinkRegex, 0, 0, false, true);
    tokenizeMatches(TokenHtmlEntity, escapedText, htmlEntityRegex);
    tokenizeMatches(TokenAutomaticLink, escapedText, automaticLinkRegex, 0, 0, false, true);
    tokenizeMatches(TokenStrikethrough, escapedText, strikethroughRegex, 2, 2);
    tokenizeMatches(TokenStrong, escapedText, strongRegex, 2, 2, true);
    tokenizeMatches(TokenEmphasis, escapedText, emphasisRegex, 1, 1, true);
    tokenizeMatches(TokenHtmlTag, escapedText, htmlTagRegex);
    tokenizeMatches(TokenMention, escapedText, mentionRegex, 0, 0, false, true);

    return true;
}

void MarkdownTokenizer::tokenizeVerbatim(QString& text)
{
    int index = verbatimRegex.indexIn(text);

    while (index >= 0)
    {
        QString end = "";
        int count = verbatimRegex.matchedLength();

        // Search for the matching end, which should have the same number
        // of back ticks as the start.
        //
        for (int i = 0; i < count; i++)
        {
            end += '`';
        }

        int endIndex = text.indexOf(end, index + count);

        // If the end was found, add the verbatim token.
        if (endIndex >= 0)
        {
            Token token;

            token.setType(TokenVerbatim);
            token.setPosition(index);
            token.setLength(endIndex + count - index);
            token.setOpeningMarkupLength(count);
            token.setClosingMarkupLength(count);
            this->addToken(token);

            // Fill out the token match in the string with the dummy
            // character so that searches for other Markdown elements
            // don't find anything within this token's range in the string.
            //
            for (int i = index; i < (index + token.getLength()); i++)
            {
                text[i] = DUMMY_CHAR;
            }

            index += token.getLength();
        }
        // Else start searching again at the very next character.
        else
        {
            index++;
        }

        index = verbatimRegex.indexIn(text, index);
    }
}

void MarkdownTokenizer::tokenizeHtmlComments(QString& text)
{
    // Check for the end of a multiline comment so that it doesn't get further
    // tokenized. Don't bother formatting the comment itself, however, because
    // it should have already been tokenized in tokenizeMultilineComment().
    //
    if (MarkdownStateComment == previousState)
    {
        int commentEnd = text.indexOf("-->");

        for (int i = 0; i < commentEnd + 3; i++)
        {
            text[i] = DUMMY_CHAR;
        }
    }

    // Now check for inline comments (non-multiline).
    int commentStart = text.indexOf(htmlInlineCommentRegex);

    while (commentStart >= 0)
    {
        int commentLength = htmlInlineCommentRegex.matchedLength();
        Token token;

        token.setType(TokenHtmlComment);
        token.setPosition(commentStart);
        token.setLength(commentLength);
        addToken(token);

        // Replace comment segment with dummy characters so that it doesn't
        // get tokenized again.
        //
        for (int i = commentStart; i < (commentStart + commentLength); i++)
        {
            text[i] = DUMMY_CHAR;
        }

        commentStart = text.indexOf
            (
                htmlInlineCommentRegex,
                commentStart + commentLength
            );
    }

    // Find multiline comment start, if any.
    commentStart = text.indexOf("<!--");

    if (commentStart >= 0)
    {
        Token token;

        token.setType(TokenHtmlComment);
        token.setPosition(commentStart);
        token.setLength(text.length() - commentStart);
        addToken(token);
        setState(MarkdownStateComment);

        // Replace comment segment with dummy characters so that it doesn't
        // get tokenized again.
        //
        for (int i = commentStart; i < text.length(); i++)
        {
            text[i] = DUMMY_CHAR;
        }
    }
}

void MarkdownTokenizer::tokenizeTableHeaderRow(QString& text)
{
    if
    (
        (
            (MarkdownStateParagraphBreak == previousState) ||
            (MarkdownStateListLineBreak == previousState) ||
            (MarkdownStateSetextHeading1Line2 == previousState) ||
            (MarkdownStateSetextHeading2Line2 == previousState) ||
            (MarkdownStateAtxHeading1 == previousState) ||
            (MarkdownStateAtxHeading2 == previousState) ||
            (MarkdownStateAtxHeading3 == previousState) ||
            (MarkdownStateAtxHeading4 == previousState) ||
            (MarkdownStateAtxHeading5 == previousState) ||
            (MarkdownStateAtxHeading6 == previousState) ||
            (MarkdownStateHorizontalRule == previousState) ||
            (MarkdownStateCodeFenceEnd == previousState) ||
            (MarkdownStateUnknown == previousState)
        )
        &&
        (
            (MarkdownStateParagraph == getState()) ||
            (MarkdownStateUnknown == getState())
        )
        && (MarkdownStatePipeTableDivider == nextState)
    )
    {
        setState(MarkdownStatePipeTableHeader);

        int headerStart = 0;

        for (int i = 0; i < text.length(); i++)
        {
            if (QChar('|') == text[i])
            {
                // Replace pipe with space so that it doesn't get formatted
                // again with, for example, strong or emphasis formatting.
                // Note that we use a space rather than DUMMY_CHAR for this,
                // to prevent formatting such as strong and emphasis from
                // picking it up.
                //
                text[i] = ' ';

                Token token;

                if (i > 0)
                {
                    token.setType(TokenTableHeader);
                    token.setPosition(headerStart);
                    token.setLength(i - headerStart);
                    addToken(token);
                }

                token.setType(TokenTablePipe);
                token.setPosition(i);
                token.setLength(1);
                addToken(token);
                headerStart = i + 1;
            }
        }

        if (headerStart < text.length())
        {
            Token token;
            token.setType(TokenTableHeader);
            token.setPosition(headerStart);
            token.setLength(text.length() - headerStart);
            addToken(token);
        }
    }
}

bool MarkdownTokenizer::tokenizeTableDivider(const QString& text)
{
    if (MarkdownStatePipeTableHeader == previousState)
    {
        if (pipeTableDividerRegex.exactMatch(text))
        {
            setState(MarkdownStatePipeTableDivider);

            Token token;
            token.setType(TokenTableDivider);
            token.setLength(text.length());
            token.setPosition(0);
            this->addToken(token);

            return true;
        }
        else
        {
            // Restart tokenizing on the previous line.
            this->requestBacktrack();
        }
    }
    else if (MarkdownStateParagraph == previousState)
    {
        if (pipeTableDividerRegex.exactMatch(text))
        {
            // Restart tokenizing on the previous line.
            this->requestBacktrack();

            setState(MarkdownStatePipeTableDivider);

            Token token;
            token.setLength(text.length());
            token.setPosition(0);
            token.setType(TokenTableDivider);
            this->addToken(token);
            return true;
        }
    }
    return false;
}

void MarkdownTokenizer::tokenizeTableRow(QString& text)
{
    if
    (
        (MarkdownStatePipeTableDivider == previousState) ||
        (MarkdownStatePipeTableRow == previousState)
    )
    {
        setState(MarkdownStatePipeTableRow);

        for (int i = 0; i < text.length(); i++)
        {
            if (QChar('|') == text[i])
            {
                // Replace pipe with space so that it doesn't get formatted
                // again with, for example, strong or emphasis formatting.
                // Note that we use a space rather than DUMMY_CHAR for this,
                // to prevent formatting such as strong and emphasis from
                // picking it up.
                //
                text[i] = ' ';

                Token token;
                token.setType(TokenTablePipe);
                token.setPosition(i);
                token.setLength(1);
                addToken(token);
            }
        }
    }
}

void MarkdownTokenizer::tokenizeMatches
(
    MarkdownTokenType tokenType,
    QString& text,
    QRegExp& regex,
    const int markupStartCount,
    const int markupEndCount,
    const bool replaceMarkupChars,
    const bool replaceAllChars
)
{
    int index = text.indexOf(regex);

    while (index >= 0)
    {
        int length = regex.matchedLength();
        Token token;

        token.setType(tokenType);
        token.setPosition(index);
        token.setLength(length);

        if (markupStartCount > 0)
        {
            token.setOpeningMarkupLength(markupStartCount);
        }

        if (markupEndCount > 0)
        {
            token.setClosingMarkupLength(markupEndCount);
        }

        if (replaceAllChars)
        {
            for (int i = index; i < (index + length); i++)
            {
                text[i] = DUMMY_CHAR;
            }
        }
        else if (replaceMarkupChars)
        {
            for (int i = index; i < (index + markupStartCount); i++)
            {
                text[i] = DUMMY_CHAR;
            }

            for (int i = (index + length - markupEndCount); i < (index + length); i++)
            {
                text[i] = DUMMY_CHAR;
            }
        }

        addToken(token);
        index = text.indexOf(regex, index + length);
    }
}

QString MarkdownTokenizer::dummyOutEscapeCharacters(const QString& text) const
{
    bool escape = false;
    QString escapedText = text;

    for (int i = 0; i < text.length(); i++)
    {
        if (escape)
        {
            escapedText[i] = DUMMY_CHAR; // Use a dummy character.
            escape = false;
        }
        else if (QChar('\\') == text[i])
        {
            escape = true;
        }
    }

    return escapedText;
}
