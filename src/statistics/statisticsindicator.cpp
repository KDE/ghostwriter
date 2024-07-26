/*
 * SPDX-FileCopyrightText: 2021-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QListView>

#include "statisticsindicator.h"

namespace ghostwriter
{
//~ singular %Ln word
//~ plural %Ln words
static QString wordCountText(int value) { return StatisticsIndicator::tr("%Ln word", "%Ln words", value); }

//~ singular %Ln character
//~ plural %Ln characters
static QString characterCountText(int value) { return StatisticsIndicator::tr("%Ln character", "%Ln characters", value); }

//~ singular %Ln sentence
//~ plural %Ln sentences
static QString sentenceCountText(int value) { return StatisticsIndicator::tr("%Ln sentence", "%Ln sentences", value); }

//~ singular %Ln paragraph
//~ plural %Ln paragraphs
static QString paragraphCountText(int value) { return StatisticsIndicator::tr("%Ln paragraph", "%Ln paragraphs", value); }

//~ singular %Ln page
//~ plural %Ln pages
static QString pageCountText(int value) { return StatisticsIndicator::tr("%Ln page", "%Ln pages", value); }

//~ singular %Ln word added
//~ plural %Ln words added
static QString wordsAddedText(int value) { return StatisticsIndicator::tr("%Ln word added", "%Ln words added", value); }

static QString wpmText(int value) { return StatisticsIndicator::tr("%1 wpm").arg(value); }
static QString readTimeText(int minutes) { return StatisticsIndicator::tr("%1:%2 read time")
                                                        .arg((int) (minutes / 60), 2, 10, QChar('0'))
                                                        .arg((int) (minutes % 60), 2, 10, QChar('0')); }
static QString writeTimeText(int minutes) { return StatisticsIndicator::tr("%1:%2 write time")
                                                        .arg((int) (minutes / 60), 2, 10, QChar('0'))
                                                        .arg((int) (minutes % 60), 2, 10, QChar('0')); }


StatisticsIndicator::StatisticsIndicator(DocumentStatistics *documentStats,
        SessionStatistics *sessionStats,
        QWidget *parent)
    : QComboBox(parent)
{
    this->setView(new QListView());
    this->view()->setTextElideMode(Qt::ElideNone);
    this->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    connect(this,
        &QComboBox::currentTextChanged,
        [this](QString text) {
            int max = this->fontMetrics().averageCharWidth() * (text.length() + 2);
            this->setMaximumWidth(max + 20);
            this->setMinimumContentsLength(text.length());
        });


    int index = 0;

    this->addItem(wordCountText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(documentStats,
        &DocumentStatistics::wordCountChanged,
        this,
        [this, index](int value) {
            this->setItemText(index,
                wordCountText(value));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
    index++;

    this->addItem(characterCountText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(documentStats,
        &DocumentStatistics::characterCountChanged,
        this,
        [this, index](int value) {
            this->setItemText(index,
                characterCountText(value));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
    index++;

    this->addItem(sentenceCountText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(documentStats,
        &DocumentStatistics::sentenceCountChanged,
        this,
        [this, index](int value) {
            this->setItemText(index,
                sentenceCountText(value));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
    index++;

    this->addItem(paragraphCountText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(documentStats,
        &DocumentStatistics::paragraphCountChanged,
        this,
        [this, index](int value) {
            this->setItemText(index,
                paragraphCountText(value));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
    index++;

    this->addItem(pageCountText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(documentStats,
        &DocumentStatistics::pageCountChanged,
        this,
        [this, index](int value) {
            this->setItemText(index,
                pageCountText(value));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
    index++;

    this->addItem(readTimeText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(documentStats,
        &DocumentStatistics::readingTimeChanged,
        this,
        [this, index](int minutes) {
            this->setItemText(index,
                readTimeText(minutes));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
    index++;

    this->addItem(wordsAddedText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(sessionStats,
        &SessionStatistics::wordCountChanged,
        this,
        [this, index](int value) {
            this->setItemText(index,
                wordsAddedText(value));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
    index++;

    this->addItem(wpmText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(sessionStats,
        &SessionStatistics::wordsPerMinuteChanged,
        this,
        [this, index](int value) {
            this->setItemText(index,
                wpmText(value));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
    index++;

    this->addItem(writeTimeText(0));
    this->setItemData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    this->connect(sessionStats,
        &SessionStatistics::writingTimeChanged,
        this,
        [this, index](int minutes) {
            this->setItemText(index,
                writeTimeText(minutes));

            if (index == this->currentIndex()) {
                this->setMinimumContentsLength(this->itemText(index).length());
            }
        });
}

StatisticsIndicator::~StatisticsIndicator()
{
    ;
}

void StatisticsIndicator::showPopup()
{
    int max = 0;

    for (int i = 0; i < this->count(); i++) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        int itemWidth = this->fontMetrics().horizontalAdvance(this->itemText(i));
#else
        int itemWidth = this->fontMetrics().boundingRect(this->itemText(i)).width();
#endif

        if (itemWidth > max) {
            max = itemWidth;
        }
    }

    this->view()->setMinimumWidth(max + 20);
    QComboBox::showPopup();
}

}
