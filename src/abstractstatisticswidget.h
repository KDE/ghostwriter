/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ABSTRACTSTATISTICSWIDGET_H
#define ABSTRACTSTATISTICSWIDGET_H

#include <QListWidget>
#include <QLabel>

namespace ghostwriter
{
/**
 * Abstract class to layout statistics data into a QListWidget.
 * This class is inherited by DocumentStatisticsWidget and
 * SessionStatisticsWidget.
 */
class AbstractStatisticsWidget : public QListWidget
{
    Q_OBJECT

public:
    AbstractStatisticsWidget(QWidget *parent = nullptr);
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
    void setIntegerValueForLabel(QLabel *label, int value);

    /**
     * Sets the text of the given value label to be that of the
     * given string.  Calling this method assures uniform formatting
     * is applied to the label text.
     */
    void setStringValueForLabel(QLabel *label, const QString &value);

    /**
     * Sets the text of the given value label to a percentage.
     * Calling this method assures uniform formatting is applied
     * to the label text.
     */
    void setPercentageValueForLabel(QLabel *label, int percentage);

    /**
     * Set the text of the given value label to represent a time in
     * hours and minutes.
     */
    void setTimeValueForLabel(QLabel *label, unsigned long minutes);

    /**
     * Set the text of the given value label to a number of pages.
     */
    void setPageValueForLabel(QLabel *label, int pages);

    /**
     * Adds a statistic row to the list widget containing a description
     * label and a value label.  The value label is returned.  An optional
     * tool tip string to display when hovering the mouse over the row
     * can be passed in as a parameter.
     */
    QLabel *addStatisticLabel
    (
        const QString &description,
        const QString &initialValue,
        const QString &toolTip = QString()
    );
};
}

#endif // ABSTRACTSTATISTICSWIDGET_H
