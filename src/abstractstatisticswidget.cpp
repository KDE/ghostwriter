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
#include <QApplication>

#include "abstractstatisticswidget.h"

namespace ghostwriter
{
AbstractStatisticsWidget::AbstractStatisticsWidget(QWidget *parent)
    : QListWidget(parent),
      LESS_THAN_ONE_MINUTE_STR(tr("&lt; 1m")),
      LESS_THAN_ONE_STR(tr("&lt; 1")),
      PAGE_STATISTIC_INFO_TOOLTIP_STR(tr("250 words per page"))
{

}

AbstractStatisticsWidget::~AbstractStatisticsWidget()
{

}

void AbstractStatisticsWidget::setIntegerValueForLabel(QLabel *label, int value)
{
    label->setText(QString("<b>%L1</b>").arg(value));
}

void AbstractStatisticsWidget::setStringValueForLabel(QLabel *label, const QString &value)
{
    label->setText(QString("<b>") + value + "</b>");
}

void AbstractStatisticsWidget::setPercentageValueForLabel(QLabel *label, int percentage)
{
    label->setText(QString("<b>%L1%</b>").arg(percentage));
}

void AbstractStatisticsWidget::setTimeValueForLabel(QLabel *label, unsigned long minutes)
{
    QString timeText;

    unsigned long hours = minutes / 60;

    if (minutes < 1) {
        timeText = QString("<b>") + LESS_THAN_ONE_MINUTE_STR + "</b>";
    } else if (hours > 0) {
        timeText = QString("<b>") + tr("%1h %2m").arg(hours).arg(minutes % 60) + "</b>";
    } else {
        timeText = QString("<b>") + tr("%1m").arg(minutes) + "</b>";
    }

    label->setText(timeText);
}

void AbstractStatisticsWidget::setPageValueForLabel(QLabel *label, int pages)
{
    QString pagesText;

    if (pages < 1) {
        pagesText = QString("<b>") + LESS_THAN_ONE_STR + "</b>";
    } else {
        pagesText = QString("<b>%L1</b>").arg(pages);
    }

    label->setText(pagesText);
}

QLabel *AbstractStatisticsWidget::addStatisticLabel
(
    const QString &description,
    const QString &initialValue,
    const QString &toolTip
)
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    QLabel *descriptionLabel = new QLabel(description);

    Qt::Alignment descriptionAlignment = Qt::AlignRight;
    Qt::Alignment valueAlignment = Qt::AlignLeft;

    if (QApplication::isRightToLeft()) {
        descriptionAlignment = Qt::AlignLeft;
        valueAlignment = Qt::AlignRight;
    }

    descriptionLabel->setAlignment(descriptionAlignment | Qt::AlignVCenter);

    QLabel *valueLabel = new QLabel(QString("<b>%1</b>").arg(initialValue));

    valueLabel->setTextFormat(Qt::RichText);
    valueLabel->setAlignment(valueAlignment | Qt::AlignVCenter);

    layout->addWidget(descriptionLabel);
    layout->addWidget(valueLabel);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    QWidget *containerWidget = new QWidget();
    containerWidget->setLayout(layout);

    QListWidgetItem *item = new QListWidgetItem();

    this->addItem(item);
    this->setItemWidget(item, containerWidget);

    if (!toolTip.isNull()) {
        descriptionLabel->setToolTip(toolTip);
        valueLabel->setToolTip(toolTip);
    }

    return valueLabel;
}
}
