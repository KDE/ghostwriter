/***********************************************************************
 *
 * Copyright (C) 2020 wereturtle
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
    d_func()->editor = editor;
    d_func()->highlightTimer = nullptr;

    QSettings settings;

    d_func()->matchCaseButton = new QPushButton("Aa");
    QFont buttonFont = d_func()->matchCaseButton->font();
    buttonFont.setBold(true);
    int width = QFontMetrics(buttonFont).horizontalAdvance("@@@");

    d_func()->matchCaseButton->setFixedWidth(width);
    d_func()->matchCaseButton->setFont(buttonFont);
    d_func()->matchCaseButton->setCheckable(true);
    d_func()->matchCaseButton->setChecked(settings.value(GW_FIND_REPLACE_MATCH_CASE, false).toBool());
    d_func()->matchCaseButton->setToolTip(tr("Match case"));
    d_func()->wholeWordButton = new QPushButton("“ ”");
    
    d_func()->wholeWordButton->setFont(buttonFont);
    d_func()->wholeWordButton->setFixedWidth(width);
    d_func()->wholeWordButton->setCheckable(true);
    d_func()->wholeWordButton->setChecked(settings.value(GW_FIND_REPLACE_WHOLE_WORD, false).toBool());
    d_func()->wholeWordButton->setToolTip(tr("Whole word"));
    d_func()->regularExpressionButton = new QPushButton(".*");
    d_func()->regularExpressionButton->setFixedWidth(width);
    d_func()->regularExpressionButton->setFont(buttonFont);
    d_func()->regularExpressionButton->setCheckable(true);
    d_func()->regularExpressionButton->setChecked(settings.value(GW_FIND_REPLACE_REGEX, false).toBool());
    d_func()->regularExpressionButton->setToolTip(tr("Regular expression"));
    d_func()->highlightMatchesButton = new QPushButton(QChar(fa::highlighter));
    d_func()->highlightMatchesButton->setFixedWidth(width);
    d_func()->highlightMatchesButton->setFont(d_func()->awesome->font(style::stfas, buttonFont.pointSize()));
    d_func()->highlightMatchesButton->setCheckable(true);
    d_func()->highlightMatchesButton->setChecked(settings.value(GW_FIND_REPLACE_HIGHLIGHT_MATCHES, false).toBool());
    d_func()->highlightMatchesButton->setToolTip(tr("Highlight matches"));
    this->connect(d_func()->highlightMatchesButton,
        &QPushButton::clicked,
        [this](bool checked) {
            d_func()->highlightMatches(checked);
        });

    d_func()->findPrevButton = new QPushButton(QChar(fa::arrowup));
    d_func()->findPrevButton->setFlat(true);
    d_func()->findPrevButton->setFont(d_func()->awesome->font(style::stfas, buttonFont.pointSize()));
    d_func()->findPrevButton->setToolTip(tr("Find previous"));
    connect(d_func()->findPrevButton, SIGNAL(pressed()), this, SLOT(findPrevious()));
    d_func()->findNextButton = new QPushButton(QChar(fa::arrowdown));
    d_func()->findNextButton->setFlat(true);
    d_func()->findNextButton->setFont(d_func()->awesome->font(style::stfas, buttonFont.pointSize()));
    d_func()->findNextButton->setToolTip(tr("Find next"));
    connect(d_func()->findNextButton, SIGNAL(pressed()), this, SLOT(findNext()));
    d_func()->replaceButton = new QPushButton(tr("Replace"));
    connect(d_func()->replaceButton, SIGNAL(pressed()), this, SLOT(replace()));
    d_func()->replaceAllButton = new QPushButton(tr("Replace All"));
    connect(d_func()->replaceAllButton, SIGNAL(pressed()), this, SLOT(replaceAll()));
    d_func()->replaceRowVisible = false;

    d_func()->findField = new QLineEdit();
    d_func()->findField->setProperty("valid", true);
    d_func()->findField->setFocusPolicy(Qt::StrongFocus);
    d_func()->replaceField = new QLineEdit();

    d_func()->statusLabel = new QLabel();

    QPushButton *closeButton = new QPushButton(QChar(fa::timescircle), this);
    closeButton->setFlat(true);
    closeButton->setFont(d_func()->awesome->font(style::stfas, closeButton->font().pointSize()));
    closeButton->setFixedWidth(closeButton->fontMetrics().horizontalAdvance("@@"));
    closeButton->setObjectName("findReplaceCloseButton");
        
    this->connect(closeButton,
        &QPushButton::clicked,
        [this]() {
            d_func()->closeFindReplace();
        }
    );

    d_func()->layout = new QGridLayout();

    d_func()->layout->addWidget(closeButton, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    d_func()->layout->addWidget(d_func()->matchCaseButton, 0, 1, 1, 1, Qt::AlignLeft);
    d_func()->layout->addWidget(d_func()->wholeWordButton, 0, 2, 1, 1, Qt::AlignLeft);
    d_func()->layout->addWidget(d_func()->regularExpressionButton, 0, 3, 1, 1, Qt::AlignLeft);
    d_func()->layout->addWidget(d_func()->highlightMatchesButton, 0, 4, 1, 1, Qt::AlignLeft);
    d_func()->layout->addWidget(new QLabel(tr("Find:")), 0, 5, 1, 1, Qt::AlignRight);
    d_func()->layout->addWidget(d_func()->findField, 0, 6, 1, 1);
    d_func()->layout->addWidget(d_func()->statusLabel, 0, 7, 1, 1);
    d_func()->layout->addWidget(d_func()->findPrevButton, 0, 8, 1, 1, Qt::AlignRight);
    d_func()->layout->addWidget(d_func()->findNextButton, 0, 9, 1, 1, Qt::AlignRight);

    d_func()->layout->addWidget(new QLabel(tr("Replace with:")), 1, 1, 1, 5, Qt::AlignRight);
    d_func()->layout->addWidget(d_func()->replaceField, 1, 6, 1, 1);
    d_func()->layout->addWidget(d_func()->replaceButton, 1, 7, 1, 1);
    d_func()->layout->addWidget(d_func()->replaceAllButton, 1, 8, 1, 2);
    d_func()->layout->setSpacing(5);

    if (d_func()->replaceButton->text().length() > d_func()->replaceAllButton->text().length()) {
        d_func()->layout->setColumnMinimumWidth(8, d_func()->replaceButton->width());
    }
    else {
        d_func()->layout->setColumnMinimumWidth(7, d_func()->replaceAllButton->sizeHint().width());
    }

    this->setLayout(d_func()->layout);

    d_func()->findNextButton->setEnabled(false);
    d_func()->findPrevButton->setEnabled(false);
    d_func()->replaceButton->setEnabled(false);
    d_func()->replaceAllButton->setEnabled(false);

    this->connect(d_func()->findField,
        &QLineEdit::textChanged,
        [this](const QString &text) {
            bool enable = !text.isEmpty();
            d_func()->findNextButton->setEnabled(enable);
            d_func()->findPrevButton->setEnabled(enable);
            d_func()->replaceButton->setEnabled(enable);
            d_func()->replaceAllButton->setEnabled(enable);

            if (d_func()->highlightMatchesButton->isChecked()) {
                d_func()->startHighlightTimer();
            }
        });

    this->connect(d_func()->editor->document(),
        &QTextDocument::contentsChanged,
        [this]() {
            if (this->isVisible() && d_func()->highlightMatchesButton->isChecked()) {
                d_func()->highlightMatches(true);
            }
        });

    showFindView();
}

FindReplace::~FindReplace()
{
    QSettings settings;
    settings.setValue(GW_FIND_REPLACE_MATCH_CASE, d_func()->matchCaseButton->isChecked());
    settings.setValue(GW_FIND_REPLACE_WHOLE_WORD, d_func()->wholeWordButton->isChecked());
    settings.setValue(GW_FIND_REPLACE_REGEX, d_func()->regularExpressionButton->isChecked());
    settings.setValue(GW_FIND_REPLACE_HIGHLIGHT_MATCHES, d_func()->highlightMatchesButton->isChecked());
}

bool FindReplace::focusNextPrevChild(bool next)
{
    Q_UNUSED(next)
    return false;
}

void FindReplace::keyPressEvent(QKeyEvent *event) 
{
    int key = event->key();

    switch (key) {
    case Qt::Key_Return:
        if (d_func()->findField->hasFocus() || d_func()->replaceField->hasFocus()) {
            findNext();
        }
        else {
            QWidget::keyPressEvent(event);
        }
        break;
    case Qt::Key_Tab:
        if (d_func()->findField->hasFocus() && d_func()->replaceRowVisible) {
            d_func()->replaceField->setFocus();
        }
        else if (d_func()->replaceField->hasFocus() && d_func()->replaceRowVisible) {
            d_func()->findField->setFocus();
        }
        else {
            QWidget::keyPressEvent(event);
        }
        break;
    case Qt::Key_Escape:
        d_func()->closeFindReplace();
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void FindReplace::showFindView()
{
    if (!this->isVisible()) {
        d_func()->prevFocusWidget = QApplication::focusWidget();
    }

    this->setVisible(true);
    d_func()->findField->setFocus();
    d_func()->setQueryFromSelection();
    d_func()->setReplaceRowVisible(false);

    if (d_func()->highlightMatchesButton->isChecked()) {
        d_func()->highlightMatches(true);
    }
}

void FindReplace::showReplaceView()
{
    if (!this->isVisible()) {
        d_func()->prevFocusWidget = QApplication::focusWidget();
    }

    this->setVisible(true);
    d_func()->findField->setFocus();
    d_func()->setQueryFromSelection();
    d_func()->setReplaceRowVisible(true);

    if (d_func()->highlightMatchesButton->isChecked()) {
        d_func()->highlightMatches(true);
    }
}

void FindReplace::findNext()
{
    if (!this->isVisible()) {
        showFindView();
    }

    QTextCursor cursor = d_func()->editor->textCursor();

    if (d_func()->findMatch(cursor)) {
        d_func()->editor->setTextCursor(cursor);
    }
}

void FindReplace::findPrevious()
{
    if (!this->isVisible()) {
        showFindView();
    }

    QTextCursor cursor = d_func()->editor->textCursor();

    if (d_func()->findMatch(cursor, true, true)) {
        d_func()->editor->setTextCursor(cursor);
    }
}

void FindReplace::replace()
{
    if (!this->isVisible() || !d_func()->replaceRowVisible) {
        showReplaceView();
    }

    if (!d_func()->editor->textCursor().hasSelection()) {
        findNext();
    }

    if (d_func()->editor->textCursor().hasSelection()) {
        d_func()->editor->textCursor().insertText(d_func()->replaceField->text());
    }
}

void FindReplace::replaceAll()
{
    if (!this->isVisible() || !d_func()->replaceRowVisible) {
        showReplaceView();
    }
    
    QTextCursor cursor = d_func()->editor->textCursor();
    cursor.movePosition(QTextCursor::Start);

    int count = 0;

    while (d_func()->findMatch(cursor, false)) {
        cursor.insertText(d_func()->replaceField->text());
        count++;
    }

    d_func()->statusLabel->setText(tr("%1 replacements").arg(count));
    d_func()->editor->setFocus();
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

        if ((NULL != item) && (NULL != item->widget())) {
            item->widget()->setVisible(visible);
        }
    }
}

void FindReplacePrivate::startHighlightTimer() 
{
    if (nullptr == this->highlightTimer) {
        this->highlightTimer = new QTimer(q_func());
        this->highlightTimer->setSingleShot(true);
        
        q_func()->connect
        (
            this->highlightTimer,
            &QTimer::timeout,
            [this]() {
                if (q_func()->isVisible() && this->highlightMatchesButton->isChecked()) {
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
    if (this->highlightMatchesButton->isChecked()) {
        this->highlightMatches(false);
    }

    q_func()->setVisible(false);
    q_func()->focusWidget()->clearFocus();

    if (nullptr != this->prevFocusWidget) {
        this->prevFocusWidget->setFocus();
    }
}

} // namepsace ghostwriter
