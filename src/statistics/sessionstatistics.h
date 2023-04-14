/*
 * SPDX-FileCopyrightText: 2016-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SESSIONSTATISTICS_H
#define SESSIONSTATISTICS_H

#include <QObject>

class QTimer;

namespace ghostwriter
{
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
    SessionStatistics(QObject *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~SessionStatistics();

    int wordCount() const;

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
    void writingTimeChanged(int value);

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
    QTimer *sessionTimer;
    bool idle;
    unsigned long totalSeconds;
    unsigned long idleSeconds;

    int calculateWPM() const;
};
} // namespace ghostwriter

#endif // SESSIONSTATISTICS_H
