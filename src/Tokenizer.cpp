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

#include "Tokenizer.h"
#include "MarkdownStates.h"


Tokenizer::Tokenizer()
{

}

Tokenizer::~Tokenizer()
{

}

QList<Token> Tokenizer::getTokens() const
{
    return tokens;
}

int Tokenizer::getState() const
{
    return state;
}

bool Tokenizer::backtrackRequested() const
{
    return backtrack;
}

void Tokenizer::clear()
{
    tokens.clear();
    backtrack = false;
    state = MarkdownStateUnknown;
}

void Tokenizer::addToken(const Token& token)
{
    tokens.append(token);
}

void Tokenizer::setState(int state)
{
    this->state = state;
}

void Tokenizer::requestBacktrack()
{
    backtrack = true;
}
