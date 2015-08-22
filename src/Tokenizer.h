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

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QString>
#include <QList>

#include "Token.h"

class Tokenizer
{
    public:
        Tokenizer();
        virtual ~Tokenizer();

        virtual void tokenize
        (
            const QString& text,
            int currentState,
            int previousState,
            int nextState
        ) = 0;

        QList<Token> getTokens() const;
        int getState() const;
        bool backtrackRequested() const;
        void clear();

    protected:
        void addToken(const Token& token);
        void setState(int state);
        void requestBacktrack();

    private:
        int state;
        QList<Token> tokens;
        bool backtrack;

};

#endif
