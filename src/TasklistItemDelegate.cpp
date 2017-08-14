#include <QPainter>
#include <QRect>
#include <QTextDocument>
#include <QTextBlock>
#include <QEvent>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QDebug>

#include "Tasklist.h"

#include "TasklistItemDelegate.h"

Q_DECLARE_METATYPE(QTextBlock)

TasklistItemDelegate::TasklistItemDelegate(QListWidget* parent) :
    QItemDelegate(parent),
    openTask(":/resources/images/task-open-light.svg"),
    closedTask(":/resources/images/task-closed-light.svg")
{
}

void TasklistItemDelegate::drawDisplay(QPainter* painter, const QStyleOptionViewItem& option, const QString& title) const
{
    QRect r = option.rect.adjusted(20, 0, 0, 0);
    const QFontMetrics metrics(option.font);
    const QString elided = metrics.elidedText(title, Qt::ElideRight, r.width());
    painter->drawText(r.left(), r.top(), r.width()-10, r.height(), Qt::AlignLeft | Qt::AlignVCenter, elided, &r);
}

void TasklistItemDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &, const QRect &rect, Qt::CheckState state) const
{
    if(Qt::Checked == state)
    {
        closedTask.paint(painter, rect);
    }
    else if(Qt::Unchecked == state)
    {
        openTask.paint(painter, rect);
    }
}
