/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sessionstatisticswidget.h"

namespace ghostwriter
{
class SessionStatisticsWidgetPrivate
{
public:
    SessionStatisticsWidgetPrivate()
    {
        ;
    }

    ~SessionStatisticsWidgetPrivate()
    {
        ;
    }

    QLabel *wordCountLabel;
    QLabel *pageCountLabel;
    QLabel *wpmLabel;
    QLabel *writingTimeLabel;
    QLabel *idleTimePercentageLabel;
};

SessionStatisticsWidget::SessionStatisticsWidget(QWidget *parent) :
    AbstractStatisticsWidget(parent),
    d_ptr(new SessionStatisticsWidgetPrivate())
{
    Q_D(SessionStatisticsWidget);

    d->wordCountLabel = addStatisticLabel(tr("Words Written:"), "0");
    d->pageCountLabel = addStatisticLabel(tr("Pages Written:"), LESS_THAN_ONE_STR, PAGE_STATISTIC_INFO_TOOLTIP_STR);
    d->wpmLabel = addStatisticLabel(tr("Average WPM:"), "0");
    d->writingTimeLabel = addStatisticLabel(tr("Total Time:"), LESS_THAN_ONE_MINUTE_STR);
    d->idleTimePercentageLabel = addStatisticLabel(tr("Idle Time:"), "100%");
}

SessionStatisticsWidget::~SessionStatisticsWidget()
{
    ;
}

void SessionStatisticsWidget::setWordCount(int value)
{
    Q_D(SessionStatisticsWidget);

    setIntegerValueForLabel(d->wordCountLabel, value);
}

void SessionStatisticsWidget::setPageCount(int value)
{
    Q_D(SessionStatisticsWidget);

    setPageValueForLabel(d->pageCountLabel, value);
}

void SessionStatisticsWidget::setWordsPerMinute(int value)
{
    Q_D(SessionStatisticsWidget);

    setIntegerValueForLabel(d->wpmLabel, value);
}

void SessionStatisticsWidget::setWritingTime(int minutes)
{
    Q_D(SessionStatisticsWidget);

    setTimeValueForLabel(d->writingTimeLabel, minutes);
}

void SessionStatisticsWidget::setIdleTime(int percentage)
{
    Q_D(SessionStatisticsWidget);

    setPercentageValueForLabel(d->idleTimePercentageLabel, percentage);
}
} // namespace ghostwriter
