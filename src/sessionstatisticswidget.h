/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SESSIONSTATISTICSWIDGET_H
#define SESSIONSTATISTICSWIDGET_H

#include <QScopedPointer>

#include "abstractstatisticswidget.h"

namespace ghostwriter
{
/**
 * Widget to display writing session statistics
 */
class SessionStatisticsWidgetPrivate;
class SessionStatisticsWidget : public AbstractStatisticsWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SessionStatisticsWidget)

public:
    /**
     * Constructor.
     */
    SessionStatisticsWidget(QWidget *parent = nullptr);

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
    void setWritingTime(int minutes);

    /**
     * Sets the percentage of time spent idle (i.e., NOT writing).
     */
    void setIdleTime(int percentage);

private:
    QScopedPointer<SessionStatisticsWidgetPrivate> d_ptr;

};
} // namespace ghostwriter

#endif // SESSIONSTATISTICSWIDGET_H
