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

#include <QTime>
#include <QTimer>
#include <QString>

#include "TimeLabel.h"

TimeLabel::TimeLabel(QWidget* parent) :
    QLabel(parent)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeOfDay()));
    this->updateTimeOfDay();
}

TimeLabel::~TimeLabel()
{

}

void TimeLabel::updateTimeOfDay()
{
    QTime currentTime = QTime::currentTime();
    this->setText(currentTime.toString(Qt::DefaultLocaleShortDate));

    QTime nextTime = currentTime.addSecs(60);
    nextTime.setHMS(nextTime.hour(), nextTime.minute(), 0);

    // Set the timer as a single shot rather than a recurring 1000 ms
    // interval, since we don't want the time to slowly drift away
    // from being accurate due to small timer inaccuracies.
    //
    timer->setSingleShot(true);

    int interval = currentTime.msecsTo(nextTime);

    // Ensure interval is never negative.
    if (interval <= 0)
    {
        interval = 1000;
    }

    timer->start(interval);
}


