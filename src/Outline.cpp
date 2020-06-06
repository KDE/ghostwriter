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
#include <QVariant>

#include "Outline.h"

const int Outline::DOCUMENT_POSITION_ROLE = Qt::UserRole + 1;

Outline::Outline(MarkdownEditor* editor, QWidget* parent)
    : QListWidget(parent), editor(editor)
{
    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onOutlineHeadingSelected(QListWidgetItem*)));
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onOutlineHeadingSelected(QListWidgetItem*)));
    connect(editor, SIGNAL(cursorPositionChanged(int)), this, SLOT(updateCurrentNavigationHeading(int)));
    connect(editor->document(), SIGNAL(contentsChange(int, int, int)), this, SLOT(reloadOutline()));
    connect(editor->document(), SIGNAL(textBlockRemoved(const QTextBlock&)), this, SLOT(reloadOutline()));
}

Outline::~Outline()
{

}

void Outline::updateCurrentNavigationHeading(int position)
{
    if ((this->count() > 0) && (position >= 0))
    {
        // Find out in which subsection of the document the cursor presently is
        // located.
        //
        int row = findHeading(position, false);

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
                (getDocumentPosition(this->item(row)) != position)
            )
        )
        {
            row--;
        }

        if (row >= 0)
        {
            QListWidgetItem* itemToHighlight = this->item(row);
            setCurrentItem(itemToHighlight);
            this->scrollToItem
            (
                itemToHighlight,
                QAbstractItemView::PositionAtCenter
            );
        }
        else
        {
            // Document position is before the first heading.  Deselect
            // any selected headings, and scroll to the top.
            //
            setCurrentItem(NULL);
            this->scrollToTop();
        }
    }
}

void Outline::onOutlineHeadingSelected(QListWidgetItem* item)
{
    editor->navigateDocument(getDocumentPosition(item));
    emit headingNumberNavigated(this->row(item) + 1);
}

void Outline::reloadOutline()
{
    this->clear();

    if ((NULL == editor) || (NULL == editor->document()))
    {
        return;
    }

    MarkdownAST* ast = ((MarkdownDocument*) editor->document())->getMarkdownAST();

    if (NULL == ast)
    {
        return;
    }
    
    QVector<MarkdownNode*> headings = ast->getHeadings();

    foreach (MarkdownNode* heading, headings)
    {
        QString headingText("   ");

        for (int i = 1; i < heading->getHeadingLevel(); i++)
        {
            headingText += "    ";
        }

        headingText += heading->getText();

        QTextBlock block = editor->document()->findBlockByNumber(heading->getStartLine() - 1);

        if (block.isValid())
        {
            QListWidgetItem* item = new QListWidgetItem();
            item->setText(headingText);
            item->setData(DOCUMENT_POSITION_ROLE, QVariant::fromValue(block.position()));
            this->insertItem(this->count(), item);
        }
    }

    updateCurrentNavigationHeading(editor->textCursor().position());
}

int Outline::getDocumentPosition(QListWidgetItem* item)
{
    return item->data(DOCUMENT_POSITION_ROLE).value<int>();
}

int Outline::findHeading(int position, bool exactMatch)
{
    int low = 0;
    int high = this->count() - 1;
    int mid = 0;

    while (low <= high)
    {
        mid = low + ((high - low) / 2);
        int itemPos = getDocumentPosition(this->item(mid));

        // Check if desired heading at document position is at row "mid".
        if (itemPos == position)
        {
            return mid;
        }
        // Else if document position is greater than the current item's
        // position, ignore the first half of the list.
        //
        else if (itemPos < position)
        {
            mid++;
            low = mid;
        }
        // Else if document position is smaller, ignore the last half of the
        // list.
        //
        else
        {
            high = mid - 1;
        }
    }

    // Heading with desired document position is not present.

    if (exactMatch)
    {
        return -1;
    }
    else
    {
        return mid;
    }
}
