/*
 * SPDX-FileCopyrightText: 2016-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QTimer>

#include "sessionstatistics.h"

namespace ghostwriter
{
SessionStatistics::SessionStatistics(QObject *parent)
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

int SessionStatistics::wordCount() const
{
    return sessionWordCount;
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

    if (deltaWords > 0) {
        totalWordsWritten += deltaWords;
    }

    sessionWordCount += deltaWords;
    lastWordCount = newWordCount;

    if (sessionWordCount < 0) {
        sessionWordCount = 0;
    }

    emit wordCountChanged(sessionWordCount);
    emit pageCountChanged(sessionWordCount / 250);
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
    unsigned long elapsedTime = (unsigned long)(sessionTimer->interval() / 1000);

    totalSeconds += elapsedTime;

    if (idle) {
        idleSeconds += elapsedTime;
    }

    int wpm = calculateWPM();

    emit wordsPerMinuteChanged(wpm);
    emit writingTimeChanged(totalSeconds / 60);
    emit idleTimePercentageChanged((int)(((float)idleSeconds / (float)totalSeconds) * 100.0f));

    int timerTime = 1000;

    if (wpm > 0) {
        timerTime = 5000;
    }

    sessionTimer->stop();
    sessionTimer->start(timerTime);
}

int SessionStatistics::calculateWPM() const
{
    unsigned long delta = totalSeconds - idleSeconds;

    if (delta > 0) {
        return (int)(((float)totalWordsWritten * 60.0f) / (float)delta);
    } else {
        return totalWordsWritten;
    }
}
} // namespace ghostwriter
