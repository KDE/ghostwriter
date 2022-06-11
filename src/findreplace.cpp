/***********************************************************************
 *
 * Copyright (C) 2020-2021 wereturtle
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

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QStringList>
#include <QTextEdit>
#include <QTextCursor>
#include <QTimer>

#include "findreplace.h"
#include "3rdparty/QtAwesome/QtAwesome.h"

#define GW_FIND_REPLACE_MATCH_CASE "FindReplace/matchCase"
#define GW_FIND_REPLACE_WHOLE_WORD "FindReplace/wholeWord"
#define GW_FIND_REPLACE_REGEX "FindReplace/regularExpression"
#define GW_FIND_REPLACE_HIGHLIGHT_MATCHES "FindReplace/highlightMatches"

namespace ghostwriter
{
class FindReplacePrivate
{
    Q_DECLARE_PUBLIC(FindReplace)

public:
    FindReplacePrivate(FindReplace *q_ptr)
        : q_ptr(q_ptr)
    {
        this->awesome = new QtAwesome(q_ptr);
        this->awesome->initFontAwesome();
    }

    ~FindReplacePrivate()
    {
        ;
    }

    bool findMatch(QTextCursor& cursor, bool wrap = true, bool backwards = false);
    void highlightMatches(bool enabled);
    void setQueryFromSelection();
    void setReplaceRowVisible(bool visible);
    void startHighlightTimer();
    void closeFindReplace();
    
    FindReplace *q_ptr;

    QtAwesome *awesome;
    QGridLayout *layout;
    QPlainTextEdit *editor;
    QPushButton *matchCaseButton;
    QPushButton *wholeWordButton;
    QPushButton *regularExpressionButton;
    QPushButton *highlightMatchesButton;
    QPushButton *findNextButton;
    QPushButton *findPrevButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;

    QLineEdit *findField;
    QLineEdit *replaceField;
    QLabel *statusLabel;

    bool replaceRowVisible;
    QTimer *highlightTimer;

    QStringList searchHistory;
    int searchHistoryIndex;

    QWidget *prevFocusWidget;

};

FindReplace::FindReplace(QPlainTextEdit *editor, QWidget *parent)
    : QWidget(parent), d_ptr(new FindReplacePrivate(this))
{
    Q_D(FindReplace);
    
    d->editor = editor;
    d->highlightTimer = nullptr;

    QSettings settings;

    d->matchCaseButton = new QPushButton("Aa");
    QFont buttonFont = d->matchCaseButton->font();
    buttonFont.setBold(true);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int width = QFontMetrics(buttonFont).horizontalAdvance("@@@");
#else
    int width = QFontMetrics(buttonFont).boundingRect("@@@").width();
#endif

    d->matchCaseButton->setFixedWidth(width);
    d->matchCaseButton->setFont(buttonFont);
    d->matchCaseButton->setCheckable(true);
    d->matchCaseButton->setChecked(settings.value(GW_FIND_REPLACE_MATCH_CASE, false).toBool());
    d->matchCaseButton->setToolTip(tr("Match case"));

    d->wholeWordButton = new QPushButton(QChar(fa::quoteleft));
    d->wholeWordButton->setFont(d->awesome->font(style::stfas, buttonFont.pointSize()));
    d->wholeWordButton->setFixedWidth(width);
    d->wholeWordButton->setCheckable(true);
    d->wholeWordButton->setChecked(settings.value(GW_FIND_REPLACE_WHOLE_WORD, false).toBool());
    d->wholeWordButton->setToolTip(tr("Whole word"));

    d->regularExpressionButton = new QPushButton(".*");
    d->regularExpressionButton->setFixedWidth(width);
    d->regularExpressionButton->setFont(buttonFont);
    d->regularExpressionButton->setCheckable(true);
    d->regularExpressionButton->setChecked(settings.value(GW_FIND_REPLACE_REGEX, false).toBool());
    d->regularExpressionButton->setToolTip(tr("Regular expression"));

    d->highlightMatchesButton = new QPushButton(QChar(fa::highlighter));
    d->highlightMatchesButton->setFixedWidth(width);
    d->highlightMatchesButton->setFont(d->awesome->font(style::stfas, buttonFont.pointSize()));
    d->highlightMatchesButton->setCheckable(true);
    d->highlightMatchesButton->setChecked(settings.value(GW_FIND_REPLACE_HIGHLIGHT_MATCHES, false).toBool());
    d->highlightMatchesButton->setToolTip(tr("Highlight matches"));
    this->connect(d->highlightMatchesButton,
        &QPushButton::clicked,
        [d](bool checked) {
            d->highlightMatches(checked);
        });

    d->findPrevButton = new QPushButton(QChar(fa::arrowup));
    d->findPrevButton->setFlat(true);
    d->findPrevButton->setFont(d->awesome->font(style::stfas, buttonFont.pointSize()));
    d->findPrevButton->setToolTip(tr("Find previous"));
    connect(d->findPrevButton, SIGNAL(pressed()), this, SLOT(findPrevious()));
    d->findNextButton = new QPushButton(QChar(fa::arrowdown));
    d->findNextButton->setFlat(true);
    d->findNextButton->setFont(d->awesome->font(style::stfas, buttonFont.pointSize()));
    d->findNextButton->setToolTip(tr("Find next"));
    connect(d->findNextButton, SIGNAL(pressed()), this, SLOT(findNext()));

    d->replaceButton = new QPushButton(tr("Replace"));
    connect(d->replaceButton, SIGNAL(pressed()), this, SLOT(replace()));
    d->replaceAllButton = new QPushButton(tr("Replace All"));
    connect(d->replaceAllButton, SIGNAL(pressed()), this, SLOT(replaceAll()));
    d->replaceRowVisible = false;

    d->findField = new QLineEdit();
    d->findField->setProperty("valid", true);
    d->findField->setFocusPolicy(Qt::StrongFocus);
    d->replaceField = new QLineEdit();

    d->statusLabel = new QLabel();

    QPushButton *closeButton = new QPushButton(QChar(fa::timescircle), this);
    closeButton->setFlat(true);
    closeButton->setFont(d->awesome->font(style::stfas, closeButton->font().pointSize()));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    closeButton->setFixedWidth(closeButton->fontMetrics().horizontalAdvance("@@"));
#else
    closeButton->setFixedWidth(closeButton->fontMetrics().boundingRect("@@").width());
#endif

    closeButton->setObjectName("findReplaceCloseButton");
        
    this->connect(closeButton,
        &QPushButton::clicked,
        [d]() {
            d->closeFindReplace();
        }
    );

    d->layout = new QGridLayout();

    d->layout->addWidget(closeButton, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    d->layout->addWidget(d->matchCaseButton, 0, 1, 1, 1, Qt::AlignLeft);
    d->layout->addWidget(d->wholeWordButton, 0, 2, 1, 1, Qt::AlignLeft);
    d->layout->addWidget(d->regularExpressionButton, 0, 3, 1, 1, Qt::AlignLeft);
    d->layout->addWidget(d->highlightMatchesButton, 0, 4, 1, 1, Qt::AlignLeft);
    d->layout->addWidget(new QLabel(tr("Find:")), 0, 5, 1, 1, Qt::AlignRight);
    d->layout->addWidget(d->findField, 0, 6, 1, 1);
    d->layout->addWidget(d->statusLabel, 0, 7, 1, 1);
    d->layout->addWidget(d->findPrevButton, 0, 8, 1, 1, Qt::AlignRight);
    d->layout->addWidget(d->findNextButton, 0, 9, 1, 1, Qt::AlignRight);

    d->layout->addWidget(new QLabel(tr("Replace with:")), 1, 1, 1, 5, Qt::AlignRight);
    d->layout->addWidget(d->replaceField, 1, 6, 1, 1);
    d->layout->addWidget(d->replaceButton, 1, 7, 1, 1);
    d->layout->addWidget(d->replaceAllButton, 1, 8, 1, 2);
    d->layout->setSpacing(5);

    if (d->replaceButton->text().length() > d->replaceAllButton->text().length()) {
        d->layout->setColumnMinimumWidth(8, d->replaceButton->width());
    }
    else {
        d->layout->setColumnMinimumWidth(7, d->replaceAllButton->sizeHint().width());
    }

    this->setLayout(d->layout);

    d->findNextButton->setEnabled(false);
    d->findPrevButton->setEnabled(false);
    d->replaceButton->setEnabled(false);
    d->replaceAllButton->setEnabled(false);

    this->connect(d->findField,
        &QLineEdit::textChanged,
        [d](const QString &text) {
            bool enable = !text.isEmpty();
            d->findNextButton->setEnabled(enable);
            d->findPrevButton->setEnabled(enable);
            d->replaceButton->setEnabled(enable);
            d->replaceAllButton->setEnabled(enable);

            if (d->highlightMatchesButton->isChecked()) {
                d->startHighlightTimer();
            }
        });

    this->connect(d->editor->document(),
        &QTextDocument::contentsChanged,
        [this, d]() {
            if (this->isVisible() && d->highlightMatchesButton->isChecked()) {
                d->highlightMatches(true);
            }
        });

    showFindView();
}

FindReplace::~FindReplace()
{
    Q_D(FindReplace);
    
    QSettings settings;
    settings.setValue(GW_FIND_REPLACE_MATCH_CASE, d->matchCaseButton->isChecked());
    settings.setValue(GW_FIND_REPLACE_WHOLE_WORD, d->wholeWordButton->isChecked());
    settings.setValue(GW_FIND_REPLACE_REGEX, d->regularExpressionButton->isChecked());
    settings.setValue(GW_FIND_REPLACE_HIGHLIGHT_MATCHES, d->highlightMatchesButton->isChecked());
}

bool FindReplace::focusNextPrevChild(bool next)
{
    Q_D(FindReplace);
    
    Q_UNUSED(next)
    return false;
}

void FindReplace::keyPressEvent(QKeyEvent *event) 
{
    Q_D(FindReplace);
    
    int key = event->key();

    switch (key) {
    case Qt::Key_Return:
        if (d->findField->hasFocus() || d->replaceField->hasFocus()) {
            findNext();
        }
        else {
            QWidget::keyPressEvent(event);
        }
        break;
    case Qt::Key_Tab:
        if (d->findField->hasFocus() && d->replaceRowVisible) {
            d->replaceField->setFocus();
        }
        else if (d->replaceField->hasFocus() && d->replaceRowVisible) {
            d->findField->setFocus();
        }
        else {
            QWidget::keyPressEvent(event);
        }
        break;
    case Qt::Key_Escape:
        d->closeFindReplace();
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void FindReplace::showFindView()
{
    Q_D(FindReplace);
    
    if (!this->isVisible()) {
        d->prevFocusWidget = QApplication::focusWidget();
    }

    this->setVisible(true);
    d->findField->setFocus();
    d->setQueryFromSelection();
    d->setReplaceRowVisible(false);

    if (d->highlightMatchesButton->isChecked()) {
        d->highlightMatches(true);
    }
}

void FindReplace::showReplaceView()
{
    Q_D(FindReplace);
    
    if (!this->isVisible()) {
        d->prevFocusWidget = QApplication::focusWidget();
    }

    this->setVisible(true);
    d->findField->setFocus();
    d->setQueryFromSelection();
    d->setReplaceRowVisible(true);

    if (d->highlightMatchesButton->isChecked()) {
        d->highlightMatches(true);
    }
}

void FindReplace::findNext()
{
    Q_D(FindReplace);
    
    if (!this->isVisible()) {
        showFindView();
    }

    QTextCursor cursor = d->editor->textCursor();

    if (d->findMatch(cursor)) {
        d->editor->setTextCursor(cursor);
    }
}

void FindReplace::findPrevious()
{
    Q_D(FindReplace);
    
    if (!this->isVisible()) {
        showFindView();
    }

    QTextCursor cursor = d->editor->textCursor();

    if (d->findMatch(cursor, true, true)) {
        d->editor->setTextCursor(cursor);
    }
}

void FindReplace::replace()
{
    Q_D(FindReplace);
    
    if (!this->isVisible() || !d->replaceRowVisible) {
        showReplaceView();
    }

    if (!d->editor->textCursor().hasSelection()) {
        findNext();
    }

    if (d->editor->textCursor().hasSelection()) {
        d->editor->textCursor().insertText(d->replaceField->text());
    }
}

void FindReplace::replaceAll()
{
    Q_D(FindReplace);
    
    if (!this->isVisible() || !d->replaceRowVisible) {
        showReplaceView();
    }
    
    QTextCursor cursor = d->editor->textCursor();
    cursor.movePosition(QTextCursor::Start);

    int count = 0;

    while (d->findMatch(cursor, false)) {
        cursor.insertText(d->replaceField->text());
        count++;
    }

    d->statusLabel->setText(tr("%Ln replacement(s)", "", count));
    d->editor->setFocus();
}

bool FindReplacePrivate::findMatch(QTextCursor& cursor, bool wrap, bool backwards)
{
    QString searchText = this->findField->text();
    QTextDocument::FindFlags findFlags;
    QRegularExpression expr;

    findFlags.setFlag(QTextDocument::FindCaseSensitively, this->matchCaseButton->isChecked());
    findFlags.setFlag(QTextDocument::FindWholeWords, this->wholeWordButton->isChecked());
    findFlags.setFlag(QTextDocument::FindBackward, backwards);

    if (this->regularExpressionButton->isChecked()) {
        expr.setPattern(searchText);
        QRegularExpression::PatternOptions options = expr.patternOptions();
        options.setFlag(QRegularExpression::CaseInsensitiveOption,
            !this->matchCaseButton->isChecked());
        expr.setPatternOptions(options);
    }

    bool found = false;
    int wrapCount = 0;
    this->statusLabel->setText("");
    this->statusLabel->setProperty("error", false);

    while (!found && (wrapCount < 2)) {
        if (expr.pattern().isEmpty() || expr.pattern().isNull()) {
            cursor = this->editor->document()->find(searchText, cursor, findFlags);
        }
        else {
            cursor = this->editor->document()->find(expr, cursor, findFlags);
        }

        if (!cursor.isNull()) {
            found = true;
        }
        else if (!wrap) {
            break;
        }
        else {
            QTextCursor::MoveOperation location = QTextCursor::Start;
            
            if (backwards) {
                location = QTextCursor::End;
            }

            cursor = this->editor->textCursor();
            cursor.movePosition(location);
            this->statusLabel->setText(QObject::tr("Search wrapped"));

            wrapCount++;
        }
    }

    if (!found) {
        this->statusLabel->setText(QObject::tr("No results"));
        this->statusLabel->setProperty("error", true);
    }

    return found;
}

void FindReplacePrivate::highlightMatches(bool enabled)
{
    // If highlights are enabled, clear any current highlights and return.
    if (!enabled) {
        this->editor->setExtraSelections(QList<QTextEdit::ExtraSelection>());
        return;
    }

    QList<QTextEdit::ExtraSelection> selections;
    QColor highlightedTextColor = this->editor->palette().color(QPalette::HighlightedText);
    QColor highlightColor = this->editor->palette().color(QPalette::Highlight);
    highlightColor.setAlpha(150);

    QTextCursor cursor = this->editor->textCursor();
    cursor.movePosition(QTextCursor::Start);

    QTextEdit::ExtraSelection match;
    match.format.setForeground(highlightedTextColor);
    match.format.setBackground(highlightColor);

    bool movedToMatch = false;
    
    while (findMatch(cursor, false)) {
        match.cursor = cursor;
        selections.append(match);

        if (!movedToMatch && !editor->hasFocus() && (cursor >= this->editor->textCursor())) {
            this->editor->setTextCursor(cursor);
            movedToMatch = true;
        }
    }

    this->editor->setExtraSelections(selections);

    if (!selections.isEmpty()) {
        this->statusLabel->setText(QObject::tr("%1 matches").arg(selections.count()));
    }    
}

void FindReplacePrivate::setQueryFromSelection()
{
    QTextCursor cursor = this->editor->textCursor();

    if (cursor.hasSelection()) {
        this->findField->setText(cursor.selectedText());
    }
}

void FindReplacePrivate::setReplaceRowVisible(bool visible)
{
    this->replaceRowVisible = visible;

    for (int i = 0; i < this->layout->columnCount(); i++) {
        QLayoutItem* item = this->layout->itemAtPosition(1, i);

        if ((nullptr != item) && (nullptr != item->widget())) {
            item->widget()->setVisible(visible);
        }
    }
}

void FindReplacePrivate::startHighlightTimer() 
{
    Q_Q(FindReplace);

    if (nullptr == this->highlightTimer) {
        this->highlightTimer = new QTimer(q);
        this->highlightTimer->setSingleShot(true);
        
        q->connect
        (
            this->highlightTimer,
            &QTimer::timeout,
            [this, q]() {
                if (q->isVisible() && this->highlightMatchesButton->isChecked()) {
                    this->highlightMatches(true);
                }
            }
        );
    }

    if (this->highlightTimer->isActive()) {
        this->highlightTimer->stop();
    }

    this->highlightTimer->start(500);    
}

void FindReplacePrivate::closeFindReplace() 
{
    Q_Q(FindReplace);
    
    if (this->highlightMatchesButton->isChecked()) {
        this->highlightMatches(false);
    }

    q->setVisible(false);
    q->focusWidget()->clearFocus();

    if (nullptr != this->prevFocusWidget) {
        this->prevFocusWidget->setFocus();
    }
}

} // namespace ghostwriter
