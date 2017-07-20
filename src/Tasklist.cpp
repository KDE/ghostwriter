#include "Tasklist.h"
#include "TasklistItemDelegate.h"
#include <QDebug>

Q_DECLARE_METATYPE(QTextBlock)

const int Tasklist::TEXT_BLOCK_ROLE = Qt::UserRole + 1;

Tasklist::Tasklist(QWidget* parent):
    QListWidget(parent),
    currentPosition(0)
{
    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onTasklistItemSelected(QListWidgetItem*)));
    connect(this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(toggleTask(QListWidgetItem*)));
    setSelectionMode(QAbstractItemView::NoSelection);
    setItemDelegate(new TasklistItemDelegate(this));
}

void Tasklist::insertTask(Qt::CheckState state, QString txt, QTextBlock block)
{
    int row = findTasklist(block.position(), false);

    if(row < 0)
    {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(txt);
        item->setData(TEXT_BLOCK_ROLE, QVariant::fromValue(block));
        item->setCheckState(state);
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
                item->setText(txt);
            }
            // Else insert new heading item into list.
            else
            {
                item = new QListWidgetItem();
                item->setText(txt);
                item->setData(TEXT_BLOCK_ROLE, QVariant::fromValue(block));
                item->setCheckState(state);
                this->insertItem(row, item);
            }
        }
        else
        {
            item = new QListWidgetItem();
            item->setText(txt);
            item->setData(TEXT_BLOCK_ROLE, QVariant::fromValue(block));
            item->setCheckState(state);
            this->insertItem(this->count(), item);
        }
    }
}

void Tasklist::removeTask(int position)
{
    int row = findTasklist(position, true);

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
        }
    }
}
void Tasklist::toggleTask(QListWidgetItem* item)
{
    int documentPosition = getDocumentPosition(item);
    emit documentPositionNavigated(documentPosition);
    emit taskToggled();
}

void Tasklist::onTasklistItemSelected(QListWidgetItem* item)
{
    int documentPosition = getDocumentPosition(item);
    emit documentPositionNavigated(documentPosition);
}

int Tasklist::getDocumentPosition(QListWidgetItem* item)
{
    QTextBlock block = item->data(TEXT_BLOCK_ROLE).value<QTextBlock>();
    return block.position();
}

int Tasklist::findTasklist(int position, bool exactMatch)
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
