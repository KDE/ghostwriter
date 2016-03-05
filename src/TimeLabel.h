/***********************************************************************
 *
 * Copyright (C) 2016 wereturtle
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

class QTimer;

/**
 * A simple QLabel that displays the current time of day (HH:MM).
 */
class TimeLabel : public QLabel
{
    Q_OBJECT

    public:
        explicit TimeLabel(QWidget *parent = 0);
        virtual ~TimeLabel();

    private slots:
        void updateTimeOfDay();

    private:
        QTimer* timer;

};

#endif // TIMELABEL_H
