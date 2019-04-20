/***********************************************************************
 *
 * Copyright (C) 2018-2019 wereturtle
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

#include "StringObserver.h"

StringObserver::StringObserver(QObject* parent) : QObject(parent)
{
    ;
}

StringObserver::~StringObserver()
{
    ;
}

void StringObserver::setText(const QString &text)
{
    mText = text;
    emit textChanged(mText);
}

QString StringObserver::getText() const
{
    return mText;
}
