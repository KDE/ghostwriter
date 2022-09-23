/*
 * SPDX-FileCopyrightText: 2021-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
