/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DOCUMENTSTATISTICSWIDGET_H
#define DOCUMENTSTATISTICSWIDGET_H

#include <QScopedPointer>

#include "abstractstatisticswidget.h"

namespace ghostwriter
{
/**
 * Widget to display document statistics
 */
class DocumentStatisticsWidgetPrivate;
class DocumentStatisticsWidget : public AbstractStatisticsWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DocumentStatisticsWidget)

public:
    /**
     * Constructor.
     */
    DocumentStatisticsWidget(QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~DocumentStatisticsWidget();

public slots:
    /**
     * Sets the word count to display.
     */
    void setWordCount(int value);

    /**
     * Sets the character count to display.
     */
    void setCharacterCount(int value);

    /**
     * Sets the paragraph count to display.
     */
    void setParagraphCount(int value);

    /**
     * Sets the sentence count to display.
     */
    void setSentenceCount(int value);

    /**
     * Sets the page count to display.
     */
    void setPageCount(int value);

    /**
     * Sets the complex word percentage to display.
     */
    void setComplexWords(int percentage);

    /**
     * Sets the reading time to display.
     */
    void setReadingTime(int minutes);

    /**
     * Sets the LIX reading ease to display.
     */
    void setLixReadingEase(int value);

    /**
     * Sets the Coleman-Liau readability index (CLI).
     */
    void setReadabilityIndex(int value);

private:
    QScopedPointer<DocumentStatisticsWidgetPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // DOCUMENTSTATISTICSDISPLAY_H
