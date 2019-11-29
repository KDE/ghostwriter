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

#ifndef SESSIONSTATISTICS_H
#define SESSIONSTATISTICS_H

#include <QObject>

class QTimer;

/**
 * Class to compute session statistics.
 */
class SessionStatistics : public QObject
{
    Q_OBJECT

    public:
        /**
         * Constructor.
         */
        SessionStatistics(QObject* parent = nullptr);

        /**
         * Destructor.
         */
        virtual ~SessionStatistics();

    signals:
        /**
         * Emitted when word count changes.
         */
        void wordCountChanged(int value);

        /**
         * Emitted when page count changes.
         */
        void pageCountChanged(int value);

        /**
         * Emitted when words per minute change.
         */
        void wordsPerMinuteChanged(int value);

        /**
         * Emitted when writing time in this session changes.
         */
        void writingTimeChanged(unsigned long value);

        /**
         * Emitted when the percentage of time spent idle
         * (i.e., not typing) changes.
         */
        void idleTimePercentageChanged(int percentage);

    public slots:
        /**
         * Resets statistics for a new writing session.
         */
        void startNewSession(int lastWordCount = 0);

        void onDocumentWordCountChanged(int newWordCount);
        void onTypingPaused();
        void onTypingResumed();

    private slots:
        void onSessionTimerExpired();

    private:
        int sessionWordCount;
        int totalWordsWritten;
        int lastWordCount;
        QTimer* sessionTimer;
        bool idle;
        unsigned long totalSeconds;
        unsigned long idleSeconds;

        int calculateWPM() const;
};

#endif // SESSIONSTATISTICS_H
