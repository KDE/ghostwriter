/***********************************************************************
 *
 * Copyright (C) 2016-2020 wereturtle
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

#ifndef TIMELABEL_H
#define TIMELABEL_H

#include <QLabel>
#include <QScopedPointer>

class QTimer;

namespace ghostwriter
{
/**
 * A simple QLabel that displays the current time of day (HH:MM).
 */
class TimeLabelPrivate;
class TimeLabel : public QLabel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(TimeLabel)

public:
    /**
     * Constructor.
     */
    explicit TimeLabel(QWidget *parent = 0);

    /**
     * Destructor
     */
    virtual ~TimeLabel();

private:
    QScopedPointer<TimeLabelPrivate> d_ptr;

};
} // namespace ghostwriter

#endif // TIMELABEL_H
