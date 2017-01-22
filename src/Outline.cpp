/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
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

Q_DECLARE_METATYPE(QTextBlock)

const int Outline::TEXT_BLOCK_ROLE = Qt::UserRole + 1;


Outline::Outline(QWidget* parent)
    : QListWidget(parent)
{
    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onOutlineHeadingSelected(QListWidgetItem*)));
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onOutlineHeadingSelected(QListWidgetItem*)));
    currentPosition = 0;
}

Outline::~Outline()
{

}

void Outline::updateCurrentNavigationHeading(int position)
{
    currentPosition = position;

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

void Outline::insertHeadingIntoOutline
(
    int level,
    const QString& text,
    const QTextBlock& block
)
{
    QString headingText("   ");

    for (int i = 1; i < level; i++)
    {
        headingText += "    ";
    }

    headingText += text;

    int row = findHeading(block.position(), false);

    if (row < 0)
    {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(headingText);
        item->setData(TEXT_BLOCK_ROLE, QVariant::fromValue(block));
        this->insertItem(0, item);
    }
    else
    {
        QListWidgetItem* item = this->item(row);

        if (NULL != item)
        {
            int itemDocPos = getDocumentPosition(item);

            // If the heading is already in the tree, just update
            // the displayed text.
            //
            if (itemDocPos == block.position())
            {
                item->setText(headingText);
            }
            // Else insert new heading item into list.
            else
            {
                item = new QListWidgetItem();
                item->setText(headingText);
                item->setData(TEXT_BLOCK_ROLE, QVariant::fromValue(block));
                this->insertItem(row, item);
            }
        }
        else
        {
            item = new QListWidgetItem();
            item->setText(headingText);
            item->setData(TEXT_BLOCK_ROLE, QVariant::fromValue(block));

            this->insertItem(this->count(), item);
        }

        updateCurrentNavigationHeading(currentPosition);
    }
}

void Outline::removeHeadingFromOutline(int position)
{
    int row = findHeading(position, true);

    if (row >= 0)
    {
        QListWidgetItem* item = this->item(row);

        if (NULL != item)
        {
            item = this->takeItem(row);

            if (NULL != item)
            {
                delete item;
                item = NULL;
            }

            // Refresh which heading is highlighted in the outline as the
            // current position.
            //
            updateCurrentNavigationHeading(currentPosition);
        }
    }
}

void Outline::onOutlineHeadingSelected(QListWidgetItem* item)
{
    int documentPosition = getDocumentPosition(item);
    emit documentPositionNavigated(documentPosition);
    emit headingNumberNavigated(this->row(item) + 1);
}

int Outline::getDocumentPosition(QListWidgetItem* item)
{
    QTextBlock block = item->data(TEXT_BLOCK_ROLE).value<QTextBlock>();

    return block.position();
}

int Outline::findHeading(int position, bool exactMatch)
{
    int low = 0;
    int high = this->count() - 1;
    int mid = 0;

    while (low <= high)
    {
        mid = low + ((high - low) / 2);
        int itemDocPos = getDocumentPosition(this->item(mid));

        // Check if desired heading at document position is at row "mid".
        if (itemDocPos == position)
        {
            return mid;
        }
        // Else if document position is greater than the current item's
        // position, ignore the first half of the list.
        //
        else if (itemDocPos < position)
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
