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

#include "DocumentStatisticsDisplay.h"

const QString DocumentStatisticsDisplay::LESS_THAN_ONE_MINUTE_STR =
    QObject::tr("&lt; 1m");

const QString DocumentStatisticsDisplay::VERY_EASY_READING_EASE_STR =
    QObject::tr("Very Easy");
const QString DocumentStatisticsDisplay::EASY_READING_EASE_STR =
    QObject::tr("Easy");
const QString DocumentStatisticsDisplay::MEDIUM_READING_EASE_STR =
    QObject::tr("Standard");
const QString DocumentStatisticsDisplay::DIFFICULT_READING_EASE_STR =
    QObject::tr("Difficult");
const QString DocumentStatisticsDisplay::VERY_DIFFICULT_READING_EASE_STR =
    QObject::tr("Very Difficult");

DocumentStatisticsDisplay::DocumentStatisticsDisplay(QWidget* parent)
    : QListWidget(parent)
{
    wordCountLabel = addStatisticLabel(tr("Words:"), "0");
    characterCountLabel = addStatisticLabel(tr("Characters:"), "0");
    sentenceCountLabel = addStatisticLabel(tr("Sentences:"), "0");
    paragraphCountLabel = addStatisticLabel(tr("Paragraphs:"), "0");
    pageCountLabel = addStatisticLabel(tr("Pages:"), "0", tr("Assumes 250 words per page"));
    complexWordsLabel = addStatisticLabel(tr("Complex Words:"), "0%");
    readingTimeLabel = addStatisticLabel(tr("Reading Time:"), LESS_THAN_ONE_MINUTE_STR);
    lixReadingEaseLabel = addStatisticLabel(tr("Reading Ease:"), VERY_EASY_READING_EASE_STR, tr("LIX Reading Ease"));
    cliLabel = addStatisticLabel(tr("Grade Level:"), "0", tr("Coleman-Liau Readability Index (CLI)"));
}

DocumentStatisticsDisplay::~DocumentStatisticsDisplay()
{
}

void DocumentStatisticsDisplay::setWordCount(int value)
{
    wordCountLabel->setText(QString("<b>%L1</b>").arg(value));
}


void DocumentStatisticsDisplay::setCharacterCount(int value)
{
    characterCountLabel->setText(QString("<b>%L1</b>").arg(value));
}

void DocumentStatisticsDisplay::setParagraphCount(int value)
{
    paragraphCountLabel->setText(QString("<b>%L1</b>").arg(value));
}

void DocumentStatisticsDisplay::setSentenceCount(int value)
{
    sentenceCountLabel->setText(QString("<b>%L1</b>").arg(value));
}

void DocumentStatisticsDisplay::setPageCount(int value)
{
    pageCountLabel->setText(QString("<b>%L1</b>").arg(value));
}

void DocumentStatisticsDisplay::setComplexWords(int percentage)
{
    complexWordsLabel->setText(tr("<b>%L1%</b>").arg(percentage));
}

void DocumentStatisticsDisplay::setReadingTime(int minutes)
{
    QString readingTimeText;

    int hours = minutes / 60;

    if (minutes < 1)
    {
        readingTimeText = QString("<b>") + LESS_THAN_ONE_MINUTE_STR + "</b>";
    }
    else if (hours > 0)
    {
        readingTimeText = QString("<b>") + tr("%1h %2m").arg(hours).arg(minutes % 60) + "</b>";
    }
    else
    {
        readingTimeText = QString("<b>") + tr("%1m").arg(minutes) + "</b>";
    }

    readingTimeLabel->setText(readingTimeText);
}

void DocumentStatisticsDisplay::setLixReadingEase(int value)
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

    lixReadingEaseLabel->setText(QString("<b>") + readingEaseStr + "</b>");
}

void DocumentStatisticsDisplay::setReadabilityIndex(int value)
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

    cliLabel->setText(QString("<b>") + cliStr + "</b>");
}

QLabel* DocumentStatisticsDisplay::addStatisticLabel
(
    const QString& description,
    const QString& initialValue,
    const QString& toolTip
)
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);

    QLabel* descriptionLabel = new QLabel(description);
    descriptionLabel->setAlignment(Qt::AlignRight);

    QLabel* valueLabel = new QLabel(QString("<b>%1</b>").arg(initialValue));

    valueLabel->setTextFormat(Qt::RichText);
    valueLabel->setAlignment(Qt::AlignLeft);

    layout->addWidget(descriptionLabel);
    layout->addWidget(valueLabel);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    QWidget* containerWidget = new QWidget();
    containerWidget->setLayout(layout);

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(containerWidget->sizeHint());

    this->addItem(item);
    this->setItemWidget(item, containerWidget);

    if (!toolTip.isNull())
    {
        descriptionLabel->setToolTip(toolTip);
        valueLabel->setToolTip(toolTip);
    }

    return valueLabel;
}

