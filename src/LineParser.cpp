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

#include "LineParser.h"
#include "LineParserStates.h"

LineParser::LineParser()
{

}

LineParser::~LineParser()
{

}

QList<Token> LineParser::getTokens() const
{
    return tokens;
}

int LineParser::getState() const
{
    return state;
}

bool LineParser::backtrackRequested() const
{
    return backtrack;
}

void LineParser::clear()
{
    tokens.clear();
    backtrack = false;
    state = LINE_PARSER_STATE_UNKNOWN;
}

void LineParser::addToken(const Token& token)
{
    tokens.append(token);
}

void LineParser::setState(int state)
{
    this->state = state;
}

void LineParser::requestBacktrack()
{
    backtrack = true;
}
