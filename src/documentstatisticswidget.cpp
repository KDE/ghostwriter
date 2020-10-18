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

#include <QHBoxLayout>
#include <QLabel>

#include "documentstatisticswidget.h"

namespace ghostwriter
{

class DocumentStatisticsWidgetPrivate
{
public:
    DocumentStatisticsWidgetPrivate()
        : VERY_EASY_READING_EASE_STR(QObject::tr("Very Easy")),
          EASY_READING_EASE_STR(QObject::tr("Easy")),
          MEDIUM_READING_EASE_STR(QObject::tr("Standard")),
          DIFFICULT_READING_EASE_STR(QObject::tr("Difficult")),
          VERY_DIFFICULT_READING_EASE_STR(QObject::tr("Very Difficult"))
    {
        ;
    }

    ~DocumentStatisticsWidgetPrivate()
    {
        ;
    }

    const QString VERY_EASY_READING_EASE_STR;
    const QString EASY_READING_EASE_STR;
    const QString MEDIUM_READING_EASE_STR;
    const QString DIFFICULT_READING_EASE_STR;
    const QString VERY_DIFFICULT_READING_EASE_STR;

    QLabel *wordCountLabel;
    QLabel *characterCountLabel;
    QLabel *paragraphCountLabel;
    QLabel *sentenceCountLabel;
    QLabel *pageCountLabel;
    QLabel *complexWordsLabel;
    QLabel *readingTimeLabel;

    // LIX reading ease
    QLabel *lixReadingEaseLabel;

    // Coleman-Liau readability index (CLI)
    QLabel *cliLabel;
};

DocumentStatisticsWidget::DocumentStatisticsWidget(QWidget *parent)
    : AbstractStatisticsWidget(parent),
      d_ptr(new DocumentStatisticsWidgetPrivate())
{
    d_func()->wordCountLabel = addStatisticLabel(tr("Words:"), "0");
    d_func()->characterCountLabel = addStatisticLabel(tr("Characters:"), "0");
    d_func()->sentenceCountLabel = addStatisticLabel(tr("Sentences:"), "0");
    d_func()->paragraphCountLabel = addStatisticLabel(tr("Paragraphs:"), "0");
    d_func()->pageCountLabel = addStatisticLabel(tr("Pages:"), LESS_THAN_ONE_STR, PAGE_STATISTIC_INFO_TOOLTIP_STR);
    d_func()->complexWordsLabel = addStatisticLabel(tr("Complex Words:"), "0%");
    d_func()->readingTimeLabel = addStatisticLabel(tr("Reading Time:"), LESS_THAN_ONE_MINUTE_STR);
    d_func()->lixReadingEaseLabel = addStatisticLabel(tr("Reading Ease:"), d_func()->VERY_EASY_READING_EASE_STR, tr("LIX Reading Ease"));
    d_func()->cliLabel = addStatisticLabel(tr("Grade Level:"), "0", tr("Coleman-Liau Readability Index (CLI)"));

}

DocumentStatisticsWidget::~DocumentStatisticsWidget()
{

}

void DocumentStatisticsWidget::setWordCount(int value)
{
    setIntegerValueForLabel(d_func()->wordCountLabel, value);
}

void DocumentStatisticsWidget::setCharacterCount(int value)
{
    setIntegerValueForLabel(d_func()->characterCountLabel, value);
}

void DocumentStatisticsWidget::setParagraphCount(int value)
{
    setIntegerValueForLabel(d_func()->paragraphCountLabel, value);
}

void DocumentStatisticsWidget::setSentenceCount(int value)
{
    setIntegerValueForLabel(d_func()->sentenceCountLabel, value);
}

void DocumentStatisticsWidget::setPageCount(int value)
{
    setPageValueForLabel(d_func()->pageCountLabel, value);
}

void DocumentStatisticsWidget::setComplexWords(int percentage)
{
    setPercentageValueForLabel(d_func()->complexWordsLabel, percentage);
}

void DocumentStatisticsWidget::setReadingTime(int minutes)
{
    setTimeValueForLabel(d_func()->readingTimeLabel, minutes);
}

void DocumentStatisticsWidget::setLixReadingEase(int value)
{
    QString readingEaseStr = d_func()->VERY_DIFFICULT_READING_EASE_STR;

    if (value <= 25) {
        readingEaseStr = d_func()->VERY_EASY_READING_EASE_STR;
    } else if (value <= 35) {
        readingEaseStr = d_func()->EASY_READING_EASE_STR;
    } else if (value <= 45) {
        readingEaseStr = d_func()->MEDIUM_READING_EASE_STR;
    } else if (value <= 55) {
        readingEaseStr = d_func()->DIFFICULT_READING_EASE_STR;
    }

    setStringValueForLabel(d_func()->lixReadingEaseLabel, readingEaseStr);
}

void DocumentStatisticsWidget::setReadabilityIndex(int value)
{
    QString cliStr = tr("Kindergarten");

    if (value > 16) {
        cliStr = tr("Rocket Science");
    } else if (value > 12) {
        cliStr = tr("College");
    } else if (value > 0) {
        cliStr.setNum(value);
    }

    setStringValueForLabel(d_func()->cliLabel, cliStr);
}
} // namespace ghostwriter
