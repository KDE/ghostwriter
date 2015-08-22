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

#ifndef TOKEN_H_
#define TOKEN_H_

#include <QString>

class Token
{
    public:
        Token();
        ~Token();

        int getType() const;
        void setType(int type);

        int getPosition() const;
        void setPosition(int position);

        int getLength() const;
        void setLength(int length);

        int getOpeningMarkupLength() const;
        void setOpeningMarkupLength(int length);
        
        int getClosingMarkupLength() const;
        void setClosingMarkupLength(int length);


    private:
        int type;
        int position;
        int length;
        int openingMarkupLength;
        int closingMarkupLength;
};

#endif
