/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QLocale>
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
    Q_D(TimeLabel);
    
    d->timer = new QTimer(this);
    this->connect
    (
        d->timer,
        &QTimer::timeout,
        [d]() {
            d->updateTimeOfDay();
        }
    );

    d->updateTimeOfDay();
}

TimeLabel::~TimeLabel()
{
    ;
}

void TimeLabelPrivate::updateTimeOfDay()
{
    Q_Q(TimeLabel);
    
    QTime currentTime = QTime::currentTime();
    QLocale locale = QLocale::system();
    QString timeText = locale.toString(currentTime, QLocale::ShortFormat);
    q->setText(timeText);

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
