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

#include <QtAlgorithms>

#include "HighlightTokenizer.h"
#include "HighlighterLineStates.h"

HighlightTokenizer::HighlightTokenizer()
{

}

HighlightTokenizer::~HighlightTokenizer()
{

}

QList<Token> HighlightTokenizer::getTokens() const
{
    return tokens.values();
}

int HighlightTokenizer::getState() const
{
    return state;
}

bool HighlightTokenizer::backtrackRequested() const
{
    return backtrack;
}

void HighlightTokenizer::clear()
{
    tokens.clear();
    backtrack = false;
    state = HIGHLIGHTER_LINE_STATE_UNKNOWN;
}

void HighlightTokenizer::addToken(const Token& token)
{
    // Insert tokens sorted by position to aid with nested formatting.
    tokens.insertMulti(token.getPosition(), token);
}

void HighlightTokenizer::setState(int state)
{
    this->state = state;
}

void HighlightTokenizer::requestBacktrack()
{
    backtrack = true;
}

bool HighlightTokenizer::tokenLessThan(const Token& t1, const Token& t2)
{
    return t1.getPosition() < t2.getPosition();
}
