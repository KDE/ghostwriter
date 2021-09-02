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
    Q_D(DocumentStatisticsWidget);

    d->wordCountLabel = addStatisticLabel(tr("Words:"), "0");
    d->characterCountLabel = addStatisticLabel(tr("Characters:"), "0");
    d->sentenceCountLabel = addStatisticLabel(tr("Sentences:"), "0");
    d->paragraphCountLabel = addStatisticLabel(tr("Paragraphs:"), "0");
    d->pageCountLabel = addStatisticLabel(tr("Pages:"), LESS_THAN_ONE_STR, PAGE_STATISTIC_INFO_TOOLTIP_STR);
    d->complexWordsLabel = addStatisticLabel(tr("Complex Words:"), "0%");
    d->readingTimeLabel = addStatisticLabel(tr("Reading Time:"), LESS_THAN_ONE_MINUTE_STR);
    d->lixReadingEaseLabel = addStatisticLabel(tr("Reading Ease:"), d->VERY_EASY_READING_EASE_STR, tr("LIX Reading Ease"));
    d->cliLabel = addStatisticLabel(tr("Grade Level:"), "0", tr("Coleman-Liau Readability Index (CLI)"));

}

DocumentStatisticsWidget::~DocumentStatisticsWidget()
{

}

void DocumentStatisticsWidget::setWordCount(int value)
{
    Q_D(DocumentStatisticsWidget);

    setIntegerValueForLabel(d->wordCountLabel, value);
}

void DocumentStatisticsWidget::setCharacterCount(int value)
{
    Q_D(DocumentStatisticsWidget);

    setIntegerValueForLabel(d->characterCountLabel, value);
}

void DocumentStatisticsWidget::setParagraphCount(int value)
{
    Q_D(DocumentStatisticsWidget);

    setIntegerValueForLabel(d->paragraphCountLabel, value);
}

void DocumentStatisticsWidget::setSentenceCount(int value)
{
    Q_D(DocumentStatisticsWidget);

    setIntegerValueForLabel(d->sentenceCountLabel, value);
}

void DocumentStatisticsWidget::setPageCount(int value)
{
    Q_D(DocumentStatisticsWidget);

    setPageValueForLabel(d->pageCountLabel, value);
}

void DocumentStatisticsWidget::setComplexWords(int percentage)
{
    Q_D(DocumentStatisticsWidget);

    setPercentageValueForLabel(d->complexWordsLabel, percentage);
}

void DocumentStatisticsWidget::setReadingTime(int minutes)
{
    Q_D(DocumentStatisticsWidget);

    setTimeValueForLabel(d->readingTimeLabel, minutes);
}

void DocumentStatisticsWidget::setLixReadingEase(int value)
{
    Q_D(DocumentStatisticsWidget);

    QString readingEaseStr = d->VERY_DIFFICULT_READING_EASE_STR;

    if (value <= 25) {
        readingEaseStr = d->VERY_EASY_READING_EASE_STR;
    } else if (value <= 35) {
        readingEaseStr = d->EASY_READING_EASE_STR;
    } else if (value <= 45) {
        readingEaseStr = d->MEDIUM_READING_EASE_STR;
    } else if (value <= 55) {
        readingEaseStr = d->DIFFICULT_READING_EASE_STR;
    }

    setStringValueForLabel(d->lixReadingEaseLabel, readingEaseStr);
}

void DocumentStatisticsWidget::setReadabilityIndex(int value)
{
    Q_D(DocumentStatisticsWidget);

    QString cliStr = tr("Kindergarten");

    if (value > 16) {
        cliStr = tr("Rocket Science");
    } else if (value > 12) {
        cliStr = tr("College");
    } else if (value > 0) {
        cliStr.setNum(value);
    }

    setStringValueForLabel(d->cliLabel, cliStr);
}
} // namespace ghostwriter
