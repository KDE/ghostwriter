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

#include <QTime>
#include <QTimer>
#include <QString>

#include "timelabel.h"

namespace ghostwriter
{
class TimeLabelPrivate
{
    Q_DECLARE_PUBLIC(TimeLabel)

public:
    TimeLabelPrivate(TimeLabel *q_ptr)
        : q_ptr(q_ptr)
    {
        ;
    }

    ~TimeLabelPrivate()
    {
        ;
    }

    void updateTimeOfDay();

    TimeLabel *q_ptr;
    QTimer *timer;
};


TimeLabel::TimeLabel(QWidget *parent) :
    QLabel(parent),
    d_ptr(new TimeLabelPrivate(this))
{
    d_func()->timer = new QTimer(this);
    this->connect
    (
        d_func()->timer,
        &QTimer::timeout,
    [this]() {
        d_func()->updateTimeOfDay();
    }
    );

    d_func()->updateTimeOfDay();
}

TimeLabel::~TimeLabel()
{
    ;
}

void TimeLabelPrivate::updateTimeOfDay()
{
    QTime currentTime = QTime::currentTime();
    q_func()->setText(currentTime.toString(Qt::DefaultLocaleShortDate));

    QTime nextTime = currentTime.addSecs(60);
    nextTime.setHMS(nextTime.hour(), nextTime.minute(), 0);

    // Set the timer as a single shot rather than a recurring 1000 ms
    // interval, since we don't want the time to slowly drift away
    // from being accurate due to small timer inaccuracies.
    //
    timer->setSingleShot(true);

    int interval = currentTime.msecsTo(nextTime);

    // Ensure interval is never negative.
    if (interval <= 0) {
        interval = 1000;
    }

    timer->start(interval);
}
} // namespace ghostwriter
