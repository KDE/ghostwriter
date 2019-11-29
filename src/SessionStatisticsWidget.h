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

#ifndef SESSIONSTATISTICSWIDGET_H
#define SESSIONSTATISTICSWIDGET_H

#include "AbstractStatisticsWidget.h"

class QLabel;

/**
 * Widget to display writing session statistics
 */
class SessionStatisticsWidget : public AbstractStatisticsWidget
{
    Q_OBJECT

    public:
        /**
         * Constructor.
         */
        SessionStatisticsWidget(QWidget* parent = NULL);

        /**
         * Destructor.
         */
        virtual ~SessionStatisticsWidget();

    public slots:
        /**
         * Sets the word count to display.
         */
        void setWordCount(int value);

        /**
         * Sets the page count to display.
         */
        void setPageCount(int value);

        /**
         * Sets the words per minute to display.
         */
        void setWordsPerMinute(int percentage);

        /**
         * Sets the time spent writing.
         */
        void setWritingTime(unsigned long minutes);

        /**
         * Sets the percentage of time spent idle (i.e., NOT writing).
         */
        void setIdleTime(int percentage);

    private:
        QLabel* wordCountLabel;
        QLabel* pageCountLabel;
        QLabel* wpmLabel;
        QLabel* writingTimeLabel;
        QLabel* idleTimePercentageLabel;

};

#endif // SESSIONSTATISTICSWIDGET_H
