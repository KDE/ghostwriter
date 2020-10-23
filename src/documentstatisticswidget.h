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
