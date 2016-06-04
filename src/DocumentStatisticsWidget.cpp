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

#include <QHBoxLayout>
#include <QLabel>

#include "DocumentStatisticsWidget.h"

DocumentStatisticsWidget::DocumentStatisticsWidget(QWidget* parent)
    : AbstractStatisticsWidget(parent),
      VERY_EASY_READING_EASE_STR(tr("Very Easy")),
      EASY_READING_EASE_STR(tr("Easy")),
      MEDIUM_READING_EASE_STR(tr("Standard")),
      DIFFICULT_READING_EASE_STR(tr("Difficult")),
      VERY_DIFFICULT_READING_EASE_STR(tr("Very Difficult"))

{
    wordCountLabel = addStatisticLabel(tr("Words:"), "0");
    characterCountLabel = addStatisticLabel(tr("Characters:"), "0");
    sentenceCountLabel = addStatisticLabel(tr("Sentences:"), "0");
    paragraphCountLabel = addStatisticLabel(tr("Paragraphs:"), "0");
    pageCountLabel = addStatisticLabel(tr("Pages:"), LESS_THAN_ONE_STR, PAGE_STATISTIC_INFO_TOOLTIP_STR);
    complexWordsLabel = addStatisticLabel(tr("Complex Words:"), "0%");
    readingTimeLabel = addStatisticLabel(tr("Reading Time:"), LESS_THAN_ONE_MINUTE_STR);
    lixReadingEaseLabel = addStatisticLabel(tr("Reading Ease:"), VERY_EASY_READING_EASE_STR, tr("LIX Reading Ease"));
    cliLabel = addStatisticLabel(tr("Grade Level:"), "0", tr("Coleman-Liau Readability Index (CLI)"));

}

DocumentStatisticsWidget::~DocumentStatisticsWidget()
{

}

void DocumentStatisticsWidget::setWordCount(int value)
{
    setIntegerValueForLabel(wordCountLabel, value);
}

void DocumentStatisticsWidget::setCharacterCount(int value)
{
    setIntegerValueForLabel(characterCountLabel, value);
}

void DocumentStatisticsWidget::setParagraphCount(int value)
{
    setIntegerValueForLabel(paragraphCountLabel, value);
}

void DocumentStatisticsWidget::setSentenceCount(int value)
{
    setIntegerValueForLabel(sentenceCountLabel, value);
}

void DocumentStatisticsWidget::setPageCount(int value)
{
    setPageValueForLabel(pageCountLabel, value);
}

void DocumentStatisticsWidget::setComplexWords(int percentage)
{
    setPercentageValueForLabel(complexWordsLabel, percentage);
}

void DocumentStatisticsWidget::setReadingTime(int minutes)
{
    setTimeValueForLabel(readingTimeLabel, minutes);
}

void DocumentStatisticsWidget::setLixReadingEase(int value)
{
    QString readingEaseStr = VERY_DIFFICULT_READING_EASE_STR;

    if (value <= 25)
    {
        readingEaseStr = VERY_EASY_READING_EASE_STR;
    }
    else if (value <= 35)
    {
        readingEaseStr = EASY_READING_EASE_STR;
    }
    else if (value <= 45)
    {
        readingEaseStr = MEDIUM_READING_EASE_STR;
    }
    else if (value <= 55)
    {
        readingEaseStr = DIFFICULT_READING_EASE_STR;
    }

    setStringValueForLabel(lixReadingEaseLabel, readingEaseStr);
}

void DocumentStatisticsWidget::setReadabilityIndex(int value)
{
    QString cliStr = tr("Kindergarten");

    if (value > 16)
    {
        cliStr = tr("Rocket Science");
    }
    else if (value > 12)
    {
        cliStr = tr("College");
    }
    else if (value > 0)
    {
        cliStr.setNum(value);
    }

    setStringValueForLabel(cliLabel, cliStr);
}

