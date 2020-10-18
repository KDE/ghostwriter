/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#include <QListWidgetItem>
#include <QString>
#include <QTextBlock>
#include <QVariant>

#include "outlinewidget.h"

namespace ghostwriter
{

class OutlineWidgetPrivate
{
    Q_DECLARE_PUBLIC(OutlineWidget)

public:
    OutlineWidgetPrivate(OutlineWidget *q_ptr)
        : q_ptr(q_ptr)
    {
        ;
    }

    ~OutlineWidgetPrivate()
    {
        ;
    }

    static const int DOCUMENT_POSITION_ROLE;

    OutlineWidget *q_ptr;
    MarkdownEditor *editor;

    /*
    * Invoked when the user selects one of the headings in the outline
    * in order to navigate to a different position in the document.
    */
    void onOutlineHeadingSelected(QListWidgetItem *item);

    void reloadOutline();

    /*
    * Gets the document position stored in the given item.
    */
    int documentPosition(QListWidgetItem *item);

    /*
    * Binary search of the outline tree.  Returns row of the matching
    * QListWidgetItem, or else -1 if the item with the given document
    * position is not found.
    *
    * If exactMatch is false and the item is not found, this method will
    * return the row number where the heading would belong if it were in
    * the tree (i.e., an ideal insertion point for a new heading).
    */
    int findHeading(int position, bool exactMatch = true);
};

const int OutlineWidgetPrivate::DOCUMENT_POSITION_ROLE = Qt::UserRole + 1;

OutlineWidget::OutlineWidget(MarkdownEditor *editor, QWidget *parent)
    : QListWidget(parent),
      d_ptr(new OutlineWidgetPrivate(this))
{
    d_func()->editor = editor;
    this->connect
    (
        this,
        &OutlineWidget::itemActivated,
        [this](QListWidgetItem * item) {
            d_func()->onOutlineHeadingSelected(item);
        }
    );
    this->connect
    (
        this,
        &OutlineWidget::itemClicked,
        [this](QListWidgetItem * item) {
            d_func()->onOutlineHeadingSelected(item);
        }
    );
    this->connect
    (
        editor,
        &MarkdownEditor::cursorPositionChanged,
        this,
        &OutlineWidget::updateCurrentNavigationHeading
    );
    this->connect
    (
        editor->document(),
        &MarkdownDocument::contentsChange,
        [this](int, int, int) {
            d_func()->reloadOutline();
        }
    );
    this->connect
    (
        (MarkdownDocument *)editor->document(),
        static_cast<void (MarkdownDocument::*)(const QTextBlock &)>(&MarkdownDocument::textBlockRemoved),
        [this]() {
            d_func()->reloadOutline();
        }
    );
}

OutlineWidget::~OutlineWidget()
{
    ;
}

void OutlineWidget::updateCurrentNavigationHeading(int position)
{
    if ((this->count() > 0) && (position >= 0)) {
        // Find out in which subsection of the document the cursor presently is
        // located.
        //
        int row = d_func()->findHeading(position, false);

        // If findHeading call recommended an insertion point for a new
        // heading rather than a matching row, then back up one row
        // for the actual heading under which the document position falls.
        //
        if
        (
            (row == this->count())
            ||
            (
                (row >= 0) &&
                (row < this->count()) &&
                (d_func()->documentPosition(this->item(row)) != position)
            )
        ) {
            row--;
        }

        if (row >= 0) {
            QListWidgetItem *itemToHighlight = this->item(row);
            setCurrentItem(itemToHighlight);
            this->scrollToItem
            (
                itemToHighlight,
                QAbstractItemView::PositionAtCenter
            );
        } else {
            // Document position is before the first heading.  Deselect
            // any selected headings, and scroll to the top.
            //
            setCurrentItem(NULL);
            this->scrollToTop();
        }
    }
}

void OutlineWidgetPrivate::onOutlineHeadingSelected(QListWidgetItem *item)
{
    editor->navigateDocument(documentPosition(item));
    emit q_func()->headingNumberNavigated(q_func()->row(item) + 1);
}

void OutlineWidgetPrivate::reloadOutline()
{
    q_func()->clear();

    if ((NULL == editor) || (NULL == editor->document())) {
        return;
    }

    MarkdownAST *ast = ((MarkdownDocument *) editor->document())->markdownAST();

    if (NULL == ast) {
        return;
    }

    QVector<MarkdownNode *> headings = ast->headings();

    foreach (MarkdownNode *heading, headings) {
        QString headingText("   ");

        for (int i = 1; i < heading->headingLevel(); i++) {
            headingText += "    ";
        }

        headingText += heading->text();

        QTextBlock block = editor->document()->findBlockByNumber(heading->startLine() - 1);

        if (block.isValid()) {
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(headingText);
            item->setData(DOCUMENT_POSITION_ROLE, QVariant::fromValue(block.position()));
            q_func()->insertItem(q_func()->count(), item);
        }
    }

    q_func()->updateCurrentNavigationHeading(editor->textCursor().position());
}

int OutlineWidgetPrivate::documentPosition(QListWidgetItem *item)
{
    return item->data(DOCUMENT_POSITION_ROLE).value<int>();
}

int OutlineWidgetPrivate::findHeading(int position, bool exactMatch)
{
    int low = 0;
    int high = q_func()->count() - 1;
    int mid = 0;

    while (low <= high) {
        mid = low + ((high - low) / 2);
        int itemPos = documentPosition(q_func()->item(mid));

        // Check if desired heading at document position is at row "mid".
        if (itemPos == position) {
            return mid;
        }
        // Else if document position is greater than the current item's
        // position, ignore the first half of the list.
        //
        else if (itemPos < position) {
            mid++;
            low = mid;
        }
        // Else if document position is smaller, ignore the last half of the
        // list.
        //
        else {
            high = mid - 1;
        }
    }

    // Heading with desired document position is not present.

    if (exactMatch) {
        return -1;
    } else {
        return mid;
    }
}
} // namespace ghostwriter
