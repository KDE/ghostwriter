/***********************************************************************
 *
 * Copyright (C) 2016-2019 wereturtle
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

#include "SessionStatisticsWidget.h"

SessionStatisticsWidget::SessionStatisticsWidget(QWidget* parent) :
    AbstractStatisticsWidget(parent)
{
    wordCountLabel = addStatisticLabel(tr("Words Written:"), "0");
    pageCountLabel = addStatisticLabel(tr("Pages Written:"), LESS_THAN_ONE_STR, PAGE_STATISTIC_INFO_TOOLTIP_STR);
    wpmLabel = addStatisticLabel(tr("Average WPM:"), "0");
    writingTimeLabel = addStatisticLabel(tr("Total Time:"), LESS_THAN_ONE_MINUTE_STR);
    idleTimePercentageLabel = addStatisticLabel(tr("Idle Time:"), "100%");
}

SessionStatisticsWidget::~SessionStatisticsWidget()
{

}

void SessionStatisticsWidget::setWordCount(int value)
{
    setIntegerValueForLabel(wordCountLabel, value);
}

void SessionStatisticsWidget::setPageCount(int value)
{
    setPageValueForLabel(pageCountLabel, value);
}

void SessionStatisticsWidget::setWordsPerMinute(int value)
{
    setIntegerValueForLabel(wpmLabel, value);
}

void SessionStatisticsWidget::setWritingTime(unsigned long minutes)
{
    setTimeValueForLabel(writingTimeLabel, minutes);
}

void SessionStatisticsWidget::setIdleTime(int percentage)
{
    setPercentageValueForLabel(idleTimePercentageLabel, percentage);
}
