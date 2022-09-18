/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TIMELABEL_H
#define TIMELABEL_H

#include <QLabel>
#include <QScopedPointer>

class QTimer;

namespace ghostwriter
{
/**
 * A simple QLabel that displays the current time of day (HH:MM).
 */
class TimeLabelPrivate;
class TimeLabel : public QLabel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(TimeLabel)

public:
    /**
     * Constructor.
     */
    explicit TimeLabel(QWidget *parent = 0);

    /**
     * Destructor
     */
    virtual ~TimeLabel();

private:
    QScopedPointer<TimeLabelPrivate> d_ptr;

};
} // namespace ghostwriter

#endif // TIMELABEL_H
