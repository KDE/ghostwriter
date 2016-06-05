/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
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

const int Outline::HEADING_LEVEL_ROLE = Qt::UserRole + 1;
const int Outline::DOCUMENT_POS_ROLE = HEADING_LEVEL_ROLE + 1;

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
        QListWidgetItem* lastItem = NULL;

        // Find out in which subsection of the document the cursor presently is
        // located.
        //
        for (int i = 0; i < this->count(); i++)
        {
            QListWidgetItem* currentItem = this->item(i);
            int outlinePos = currentItem->data(DOCUMENT_POS_ROLE).toInt();

            // If current cursor position is less than the first heading's
            // position, then no heading should be styled.
            //
            if ((0 == i) && (position < outlinePos))
            {
                setCurrentItem(currentItem, QItemSelectionModel::Clear);
                this->scrollToItem(currentItem);
                break;
            }

            QListWidgetItem* itemToHighlight = NULL;

            // If cursor position is within the last found heading's text
            // range, then style the last tree item to show that it is the
            // current section heading.
            //
            if ((NULL != lastItem) && (position < outlinePos))
            {
                itemToHighlight = lastItem;
            }
            // Else if the cursor is right at the beginning of the current
            // heading's text range, then style the current tree item.
            //
            else if ((position == outlinePos) || (this->count() - 1 == i))
            {
                itemToHighlight = currentItem;
            }

            if (NULL != itemToHighlight)
            {
                setCurrentItem(itemToHighlight);
                this->scrollToItem
                (
                    itemToHighlight,
                    QAbstractItemView::PositionAtCenter
                );
                break;
            }

            lastItem = currentItem;
        }
    }
}

void Outline::onTextChanged
(
    int position,
    int charsRemoved,
    int charsAdded
)
{
    int startIndex = position;

    if (startIndex < 0)
    {
        startIndex = 0;
    }

    int endAddedIndex = position + charsAdded - 1;

    if (endAddedIndex < startIndex)
    {
        endAddedIndex = startIndex;
    }

    int endRemovedIndex = -1;

    if (charsRemoved > 0)
    {
        endRemovedIndex = position + charsRemoved - 1;
    }

    // Check for headings that were removed, and remove them from the list
    // if they fall in the range of deleted text.
    //
    int index = 0;
    bool headingRemoved = false;

    while (index < this->count())
    {
        int treePos = this->item(index)->data(DOCUMENT_POS_ROLE).toInt();

        if ((treePos >= startIndex) && (treePos <= endRemovedIndex))
        {
            QListWidgetItem* item = this->takeItem(index);
            headingRemoved = true;

            if (NULL != item)
            {
                delete item;
                item = NULL;
            }
        }
        else
        {
            index++;
        }
    }

    // Update document positions in outline.
    updateDocumentPositions(startIndex, charsAdded - charsRemoved);

    // Refresh which heading is highlighted in the outline as the
    // current position, but only if a heading was removed.
    //
    if (headingRemoved)
    {
        this->updateCurrentNavigationHeading(currentPosition);
    }
}

void Outline::insertHeadingIntoOutline
(
    int position,
    int level,
    const QString heading
)
{
    QString headingText("   ");

    for (int i = 1; i < level; i++)
    {
        headingText += "    ";
    }

    headingText += heading;

    if (this->count() <= 0)
    {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(headingText);
        item->setData(DOCUMENT_POS_ROLE, QVariant(position));
        this->addItem(item);
    }
    else
    {
        bool added = false;

        // Find out where it goes.
        for (int i = 0; i < this->count(); i++)
        {
            int outlinePos = this->item(i)->data(DOCUMENT_POS_ROLE).toInt();

            if (position < outlinePos)
            {
                // Insert before
                QListWidgetItem* item = new QListWidgetItem();
                item->setText(headingText);
                item->setData(DOCUMENT_POS_ROLE, QVariant(position));
                this->insertItem(i, item);
                added = true;
                break;
            }
            else if (position == outlinePos)
            {
                // Replace text
                this->item(i)->setText(headingText);
                added = true;
                break;
            }
        }

        if (!added)
        {
            // Insert at the end
            QListWidgetItem* item = new QListWidgetItem();
            item->setText(headingText);
            item->setData(DOCUMENT_POS_ROLE, QVariant(position));
            this->addItem(item);
        }
    }

    // Refresh which heading is highlighted in the outline as the
    // current position.
    //
    this->updateCurrentNavigationHeading(currentPosition);
}

void Outline::removeHeadingFromOutline(int position)
{
    for (int i = 0; i < this->count(); i++)
    {
        int outlinePos = this->item(i)->data(DOCUMENT_POS_ROLE).toInt();

        if (outlinePos == position)
        {
            QListWidgetItem* item = this->takeItem(i);

            if (NULL != item)
            {
                delete item;
                item = NULL;
            }

            // Refresh which heading is highlighted in the outline as the
            // current position.
            //
            this->updateCurrentNavigationHeading(currentPosition);

            return;
        }
    }
}

void Outline::onOutlineHeadingSelected(QListWidgetItem* item)
{
    int documentPosition = item->data(DOCUMENT_POS_ROLE).toInt();
    emit documentPositionNavigated(documentPosition);
    emit headingNumberNavigated(this->row(item) + 1);
}

void Outline::updateDocumentPositions(int startPosition, int offset)
{
    for (int i = 0; i < this->count(); i++)
    {
        int treePos = this->item(i)->data(DOCUMENT_POS_ROLE).toInt();

        if (treePos > startPosition)
        {
            treePos += offset;
            this->item(i)->setData(DOCUMENT_POS_ROLE, QVariant(treePos));
        }
    }
}
