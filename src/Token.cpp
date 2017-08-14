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

#include <QString>

#include "Token.h"

Token::Token():
    type(-1),
    position(0),
    length(0),
    openingMarkupLength(0),
    closingMarkupLength(0)
{ }

Token::~Token()
{

}

int Token::getType() const
{
    return type;
}

void Token::setType(int type)
{
    this->type = type;
}

int Token::getPosition() const
{
    return position;
}

void Token::setPosition(int position)
{
    this->position = position;
}

int Token::getLength() const
{
    return length;
}

void Token::setLength(int length)
{
    this->length = length;
}

int Token::getOpeningMarkupLength() const
{
    return openingMarkupLength;
}

void Token::setOpeningMarkupLength(int length)
{
    this->openingMarkupLength = length;
}

int Token::getClosingMarkupLength() const
{
    return closingMarkupLength;
}

void Token::setClosingMarkupLength(int length)
{
    this->closingMarkupLength = length;
}
