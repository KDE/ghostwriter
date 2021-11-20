/***********************************************************************
 *
 * Copyright (C) 2021 wereturtle
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

#ifndef STATISTICSINDICATOR_H
#define STATISTICSINDICATOR_H

#include <QComboBox>

#include "documentstatistics.h"
#include "sessionstatistics.h"

namespace ghostwriter
{
/**
 * Combo box widget for use with the status bar as a status indicator.  This
 * widget allows the user to select one document/session statistics to display
 * at a time in the status bar.
 */
class StatisticsIndicator : public QComboBox
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    StatisticsIndicator(DocumentStatistics *documentStats,
        SessionStatistics *sessionStats,
        QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~StatisticsIndicator();

    void showPopup();

};
}

#endif // STATISTICSINDICATOR_H
