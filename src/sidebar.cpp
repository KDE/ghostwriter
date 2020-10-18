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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include <QButtonGroup>
#include <QDebug>

#include "sidebar.h"

namespace ghostwriter
{
class SidebarPrivate
{
public:
    SidebarPrivate()
    {
        ;
    }

    ~SidebarPrivate()
    {
        ;
    }

    QStackedWidget *stack;
    QVBoxLayout *tabs;
    QVBoxLayout *buttons;
    QButtonGroup *tabGroup;

    void changeTab(QPushButton *tab);
};

Sidebar::Sidebar(QWidget *parent)
    : QFrame(parent),
      d_ptr(new SidebarPrivate())
{
    d_func()->stack = new QStackedWidget(this);

    this->setObjectName("sidebar");
    this->setContentsMargins(0, 0, 0, 0);

    d_func()->tabs = new QVBoxLayout();
    d_func()->tabs->setObjectName("sidebarTabs");
    d_func()->tabs->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    d_func()->tabs->setSpacing(0);
    d_func()->tabs->setMargin(0);
    d_func()->tabs->setContentsMargins(0, 0, 0, 0);
    d_func()->tabs->setSizeConstraint(QLayout::SetMinimumSize);

    d_func()->tabGroup = new QButtonGroup(this);
    d_func()->tabGroup->setExclusive(true);

    this->connect
    (
        d_func()->tabGroup,
        static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
        [this](QAbstractButton *tab) {
            int index = d_func()->tabs->indexOf(tab);
            d_func()->stack->setCurrentIndex(index);
            d_func()->stack->currentWidget()->setFocus();
        }
    );

    d_func()->buttons = new QVBoxLayout();
    d_func()->buttons->setObjectName("sidebarButtons");
    d_func()->buttons->setAlignment(Qt::AlignBottom | Qt::AlignCenter);
    d_func()->buttons->setSpacing(0);
    d_func()->buttons->setMargin(0);
    d_func()->buttons->setContentsMargins(0, 0, 0, 0);
    d_func()->buttons->setSizeConstraint(QLayout::SetMinimumSize);

    QVBoxLayout *leftBarLayout = new QVBoxLayout();
    leftBarLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    leftBarLayout->setSpacing(0);
    leftBarLayout->setMargin(0);
    leftBarLayout->setContentsMargins(0, 0, 0, 0);
    leftBarLayout->setSizeConstraint(QLayout::SetMinimumSize);
    leftBarLayout->addLayout(d_func()->tabs);
    leftBarLayout->addStretch(1);
    leftBarLayout->addLayout(d_func()->buttons);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(leftBarLayout);
    layout->addWidget(d_func()->stack);
    layout->setSizeConstraint(QLayout::SetNoConstraint);
    this->setLayout(layout);
}

Sidebar::~Sidebar()
{
    ;
}

void Sidebar::addTab
(
    QPushButton *button,
    QWidget *widget
)
{
    this->insertTab(d_func()->tabs->count(), button, widget);
}

void Sidebar::insertTab
(
    int index,
    QPushButton *button,
    QWidget *widget
)
{
    if (index < 0) {
        index = 0;
    } else if (index > d_func()->tabs->count()) {
        index = d_func()->tabs->count();
    }
    
    button->setParent(this);
    button->setCheckable(true);

    if (d_func()->tabs->count() < 1) {
        button->setChecked(true);
    }

    d_func()->stack->insertWidget(index, widget);
    d_func()->tabs->insertWidget(index, button);
    d_func()->tabGroup->addButton(button);
}

void Sidebar::removeTab(int index)
{
    if (d_func()->tabs->count() <= 0) {
        return;
    }

    if (index < 0) {
        index = 0;
    } else if (index > d_func()->tabs->count()) {
        index = d_func()->tabs->count() - 1;
    }

    int activeTabIndex = 0;

    if (d_func()->stack->currentIndex() == activeTabIndex) {        
        if (activeTabIndex > 0) {
            activeTabIndex--;
        } else {
            activeTabIndex = 0;
        }
    }

    if (nullptr != d_func()->stack->widget(index)) {
        d_func()->stack->removeWidget(d_func()->stack->widget(index));
    }

    QLayoutItem *item = d_func()->tabs->takeAt(index);

    if ((nullptr != item) && (nullptr != item->widget())) {
        d_func()->tabGroup->removeButton((QAbstractButton *) item->widget());
        item->widget()->deleteLater();
    }

    if (d_func()->stack->count() > 0) {
        d_func()->stack->setCurrentIndex(activeTabIndex);
        QPushButton *tab = (QPushButton *) d_func()->tabs->itemAt(activeTabIndex);

        if (nullptr != tab) {
            tab->setChecked(true);
        }
    }
}

void Sidebar::setCurrentTabIndex(int index)
{
    if (index < 0) {
        index = 0;
    } else if (index > d_func()->tabs->count()) {
        index = d_func()->tabs->count() - 1;
    }

    QPushButton *tab = (QPushButton *) d_func()->tabs->itemAt(index)->widget();
    tab->setChecked(true);

    d_func()->stack->setCurrentIndex(index);
    d_func()->stack->currentWidget()->setFocus();
}

int Sidebar::tabCount() const
{
    return d_func()->tabs->count();
}

int Sidebar::buttonCount() const
{
    return d_func()->buttons->count();
}

void Sidebar::addButton(QPushButton *button)
{
    this->insertButton(d_func()->buttons->count(), button);
}

void Sidebar::insertButton(int index, QPushButton *button)
{
    if (index < 0) {
        index = 0;
    } else if (index > d_func()->buttons->count()) {
        index = d_func()->buttons->count();
    }

    d_func()->buttons->insertWidget(index, button);
}

void Sidebar::removeButton(int index)
{
    if (index < 0) {
        index = 0;
    } else if (index >= d_func()->buttons->count()) {
        index = d_func()->buttons->count() - 1;
    }

    d_func()->buttons->takeAt(index);
}

void Sidebar::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    emit visibilityChanged(false);
}

void Sidebar::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    emit visibilityChanged(true);
}
} // namespace ghostwriter
