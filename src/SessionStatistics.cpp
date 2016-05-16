/***********************************************************************
 *
 * Copyright (C) 2016 wereturtle
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

#include <QTimer>

#include "SessionStatistics.h"

SessionStatistics::SessionStatistics(QObject* parent)
    : QObject(parent)
{
    sessionTimer = new QTimer(this);
    connect(sessionTimer, SIGNAL(timeout()), this, SLOT(onSessionTimerExpired()));
    sessionTimer->setInterval(1000);
    idle = true;

    startNewSession(0);
}

SessionStatistics::~SessionStatistics()
{

}

void SessionStatistics::startNewSession(int initialWordCount)
{
    sessionWordCount = 0;
    totalWordsWritten = 0;
    lastWordCount = initialWordCount;
    totalSeconds = 0;
    idleSeconds = 0;
    idle = true;
    sessionTimer->stop();
    sessionTimer->start();

    emit wordCountChanged(0);
    emit pageCountChanged(0);
    emit wordsPerMinuteChanged(0);
    emit writingTimeChanged(0);
    emit idleTimePercentageChanged(100);
}

void SessionStatistics::onDocumentWordCountChanged(int newWordCount)
{
    int deltaWords = newWordCount - lastWordCount;

    if (deltaWords > 0)
    {
        totalWordsWritten += deltaWords;
    }

    sessionWordCount += deltaWords;
    lastWordCount = newWordCount;

    if (sessionWordCount < 0)
    {
        sessionWordCount = 0;
    }

    emit wordCountChanged(sessionWordCount);
    emit pageCountChanged(sessionWordCount / 250);

    if (totalSeconds < 60)
    {
        emit wordsPerMinuteChanged(calculateWPM());
    }
}

void SessionStatistics::onTypingPaused()
{
    idle = true;
}

void SessionStatistics::onTypingResumed()
{
    idle = false;
}

void SessionStatistics::onSessionTimerExpired()
{
    if (idle)
    {
        idleSeconds++;
    }

    totalSeconds++;

    emit wordsPerMinuteChanged(calculateWPM());
    emit writingTimeChanged(totalSeconds / 60);
    emit idleTimePercentageChanged((int) (((float)idleSeconds / (float)totalSeconds) * 100.0f));
}

int SessionStatistics::calculateWPM() const
{
    unsigned long delta = totalSeconds - idleSeconds;

    if (delta > 0)
    {
        return (int)(((float)totalWordsWritten * 60.0f) / (float)delta);
    }
    else
    {
        return totalWordsWritten;
    }
}
