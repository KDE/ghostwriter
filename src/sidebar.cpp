/*
 * SPDX-FileCopyrightText: 2020-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

void Sidebar::addTab(const QIcon &icon, QWidget *widget, const QString &tooltip, const QString &objectName)
{
    Q_D(Sidebar);

    this->insertTab(d->tabs->count(), icon, widget, tooltip, objectName);
}

void Sidebar::insertTab(int index, const QIcon &icon, QWidget *widget, const QString &tooltip, const QString &objectName)
{
    Q_D(Sidebar);

    if (index < 0) {
        index = 0;
    } else if (index > d->tabs->count()) {
        index = d->tabs->count();
    }

    QPushButton *button = new QPushButton(this);
    button->setIcon(icon);
    button->setCheckable(true);
    button->setToolTip(tooltip);

    if (d->tabs->count() < 1) {
        button->setChecked(true);
    }

    if (!objectName.isNull() && !objectName.isEmpty()) {
        button->setObjectName(objectName);
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

QPushButton *Sidebar::addButton(const QIcon &icon, const QString &tooltip, const QString &objectName)
{
    Q_D(Sidebar);

    return this->insertButton(d->buttons->count(), icon, tooltip, objectName);
}

QPushButton *Sidebar::insertButton(int index, const QIcon &icon, const QString &tooltip, const QString &objectName)
{
    Q_D(Sidebar);

    if (index < 0) {
        index = 0;
    } else if (index > d->buttons->count()) {
        index = d->buttons->count();
    }

    QPushButton *button = new QPushButton(this);
    button->setIcon(icon);
    button->setFocusPolicy(Qt::NoFocus);
    button->setCheckable(false);
    button->setToolTip(tooltip);

    if (!objectName.isNull() && !objectName.isEmpty()) {
        button->setObjectName(objectName);
    }

    d->buttons->insertWidget(index, button);
    return button;
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
