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

#ifndef ABSTRACTSTATISTICSWIDGET_H
#define ABSTRACTSTATISTICSWIDGET_H

#include <QListWidget>
#include <QLabel>

/**
 * Abstract class to layout statistics data into a QListWidget.
 * This class is inherited by DocumentStatisticsWidget and
 * SessionStatisticsWidget.
 */
class AbstractStatisticsWidget : public QListWidget
{
    Q_OBJECT

    public:
        AbstractStatisticsWidget(QWidget* parent = NULL);
        virtual ~AbstractStatisticsWidget();

    protected:
        const QString LESS_THAN_ONE_MINUTE_STR;
        const QString LESS_THAN_ONE_STR;
        const QString PAGE_STATISTIC_INFO_TOOLTIP_STR;

        /**
         * Sets the text of the given value label to be an integer.
         * Calling this method assures uniform formatting is applied
         * to the label text.
         */
        void setIntegerValueForLabel(QLabel* label, int value);

        /**
         * Sets the text of the given value label to be that of the
         * given string.  Calling this method assures uniform formatting
         * is applied to the label text.
         */
        void setStringValueForLabel(QLabel* label, const QString& value);

        /**
         * Sets the text of the given value label to a percentage.
         * Calling this method assures uniform formatting is applied
         * to the label text.
         */
        void setPercentageValueForLabel(QLabel* label, int percentage);

        /**
         * Set the text of the given value label to represent a time in
         * hours and minutes.
         */
        void setTimeValueForLabel(QLabel* label, unsigned long minutes);

        /**
         * Set the text of the given value label to a number of pages.
         */
        void setPageValueForLabel(QLabel* label, int pages);

        /**
         * Adds a statistic row to the list widget containing a description
         * label and a value label.  The value label is returned.  An optional
         * tool tip string to display when hovering the mouse over the row
         * can be passed in as a parameter.
         */
        QLabel* addStatisticLabel
        (
            const QString& description,
            const QString& initialValue,
            const QString& toolTip = QString()
        );
};

#endif // ABSTRACTSTATISTICSWIDGET_H
