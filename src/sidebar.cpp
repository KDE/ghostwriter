/***********************************************************************
 *
 * Copyright (C) 2020-2022 wereturtle
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
    bool autoHideEnabled;

    void changeTab(QPushButton *tab);
};

Sidebar::Sidebar(QWidget *parent)
    : QFrame(parent),
      d_ptr(new SidebarPrivate())
{
    Q_D(Sidebar);

    d->stack = new QStackedWidget(this);
    d->autoHideEnabled = false;

    this->setObjectName("sidebar");
    this->setContentsMargins(0, 0, 0, 0);
    this->connect(qApp,
        &QApplication::focusChanged,
        this,
        &Sidebar::onFocusChanged);

    d->tabs = new QVBoxLayout();
    d->tabs->setObjectName("sidebarTabs");
    d->tabs->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    d->tabs->setSpacing(0);
    d->tabs->setContentsMargins(0, 0, 0, 0);
    d->tabs->setSizeConstraint(QLayout::SetMinimumSize);

    d->tabGroup = new QButtonGroup(this);
    d->tabGroup->setExclusive(true);

    this->connect
    (
        d->tabGroup,
        static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
        [d](QAbstractButton *tab) {
            int index = d->tabs->indexOf(tab);
            d->stack->setCurrentIndex(index);
            d->stack->currentWidget()->setFocus();
        }
    );

    d->buttons = new QVBoxLayout();
    d->buttons->setObjectName("sidebarButtons");
    d->buttons->setAlignment(Qt::AlignBottom | Qt::AlignCenter);
    d->buttons->setSpacing(0);
    d->buttons->setContentsMargins(0, 0, 0, 0);
    d->buttons->setSizeConstraint(QLayout::SetMinimumSize);

    QVBoxLayout *leftBarLayout = new QVBoxLayout();
    leftBarLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    leftBarLayout->setSpacing(0);
    leftBarLayout->setContentsMargins(0, 0, 0, 0);
    leftBarLayout->setSizeConstraint(QLayout::SetMinimumSize);
    leftBarLayout->addLayout(d->tabs);
    leftBarLayout->addStretch(1);
    leftBarLayout->addLayout(d->buttons);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(leftBarLayout);
    layout->addWidget(d->stack);
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
    Q_D(Sidebar);

    this->insertTab(d->tabs->count(), button, widget);
}

void Sidebar::insertTab
(
    int index,
    QPushButton *button,
    QWidget *widget
)
{
    Q_D(Sidebar);

    if (index < 0) {
        index = 0;
    } else if (index > d->tabs->count()) {
        index = d->tabs->count();
    }
    
    button->setParent(this);
    button->setCheckable(true);

    if (d->tabs->count() < 1) {
        button->setChecked(true);
    }

    d->stack->insertWidget(index, widget);
    d->tabs->insertWidget(index, button);
    d->tabGroup->addButton(button);
}

void Sidebar::removeTab(int index)
{
    Q_D(Sidebar);

    if (d->tabs->count() <= 0) {
        return;
    }

    if (index < 0) {
        index = 0;
    } else if (index > d->tabs->count()) {
        index = d->tabs->count() - 1;
    }

    int activeTabIndex = 0;

    if (d->stack->currentIndex() == activeTabIndex) {        
        if (activeTabIndex > 0) {
            activeTabIndex--;
        } else {
            activeTabIndex = 0;
        }
    }

    if (nullptr != d->stack->widget(index)) {
        d->stack->removeWidget(d->stack->widget(index));
    }

    QLayoutItem *item = d->tabs->takeAt(index);

    if ((nullptr != item) && (nullptr != item->widget())) {
        d->tabGroup->removeButton((QAbstractButton *) item->widget());
        item->widget()->deleteLater();
    }

    if (d->stack->count() > 0) {
        d->stack->setCurrentIndex(activeTabIndex);
        QPushButton *tab = (QPushButton *) d->tabs->itemAt(activeTabIndex);

        if (nullptr != tab) {
            tab->setChecked(true);
        }
    }
}

void Sidebar::setCurrentTabIndex(int index)
{
    Q_D(Sidebar);

    if (index < 0) {
        index = 0;
    } else if (index > d->tabs->count()) {
        index = d->tabs->count() - 1;
    }

    QPushButton *tab = (QPushButton *) d->tabs->itemAt(index)->widget();
    tab->setChecked(true);

    d->stack->setCurrentIndex(index);
    d->stack->currentWidget()->setFocus();
}

int Sidebar::tabCount() const
{
    Q_D(const Sidebar);

    return d->tabs->count();
}

int Sidebar::buttonCount() const
{
    Q_D(const Sidebar);

    return d->buttons->count();
}

void Sidebar::setAutoHideEnabled(bool enabled)
{
    Q_D(Sidebar);

    d->autoHideEnabled = enabled;

    if (enabled && !this->hasFocus() && this->isVisible()) {
        QFrame::setVisible(false);
    }
}

bool Sidebar::autoHideEnabled() const
{
    Q_D(const Sidebar);

    return d->autoHideEnabled;
}

void Sidebar::addButton(QPushButton *button)
{
    Q_D(Sidebar);

    this->insertButton(d->buttons->count(), button);
}

void Sidebar::insertButton(int index, QPushButton *button)
{
    Q_D(Sidebar);

    if (index < 0) {
        index = 0;
    } else if (index > d->buttons->count()) {
        index = d->buttons->count();
    }

    d->buttons->insertWidget(index, button);
}

void Sidebar::removeButton(int index)
{
    Q_D(Sidebar);

    if (index < 0) {
        index = 0;
    } else if (index >= d->buttons->count()) {
        index = d->buttons->count() - 1;
    }

    d->buttons->takeAt(index);
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

void Sidebar::onFocusChanged(QWidget *old, QWidget *now) {
    bool focusLost = ((old != nullptr)
        && this->isAncestorOf(old)
        && ((now == nullptr) || !this->isAncestorOf(now)));

    if (this->autoHideEnabled() && focusLost && this->isVisible()) {
        this->setVisible(false);
    }
}

} // namespace ghostwriter
