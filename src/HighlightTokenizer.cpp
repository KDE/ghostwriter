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
    return tokens;
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
    tokens.append(token);
}

void HighlightTokenizer::setState(int state)
{
    this->state = state;
}

void HighlightTokenizer::requestBacktrack()
{
    backtrack = true;
}
