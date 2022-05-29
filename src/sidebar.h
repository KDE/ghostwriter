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

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QFrame>
#include <QHideEvent>
#include <QPushButton>
#include <QScopedPointer>
#include <QShowEvent>

namespace ghostwriter
{
/**
 * Sidebar similar to VS Code's activity bar/sidebar.  This widget presents
 * a vertical tab bar with checkable buttons for tabs.  At the bottom of the
 * tab bar is an area where action buttons can be added.  For styling with
 * style sheets, use QPushButton and its various pseudo-states.  For
 * convenience, an object of type Sidebar has an object name of "sidebar",
 * allowing the background area to be styled with #sidebar in the style sheet.
 */
class SidebarPrivate;
class Sidebar : public QFrame
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Sidebar)

public:
    /**
     * Constructor.
     */
    Sidebar(QWidget *parent = nullptr);

    /**
     * Destructor
     */
    ~Sidebar();

    /**
     * Adds a new tab at the bottom of the tabs with the given button
     * to represent it, and its corresponding widget to be displayed
     * when the tab is selected.
     */
    void addTab
    (
        QPushButton *button,
        QWidget *widget
    );

    /**
     * Inserts a new tab at the given tab index with the given button
     * to represent it, and its corresponding widget to be displayed
     * when the tab is selected.  Valid tab index values are from zero
     * to tabCount().
     */
    void insertTab
    (
        int index,
        QPushButton *button,
        QWidget *widget
    );

    /**
     * Removes the tab and its corresponding widget at the given index.
     * Valid tab index values are from zero to tabCount() - 1.
     */
    void removeTab(int index);

    /**
     * Sets the currently selected tab to be the tab at the given
     * index, displaying its corresponding widget.  Valid tab index
     * values are from zero to tabCount() - 1.
     */
    void setCurrentTabIndex(int index);

    /**
     * Adds an action button at the bottom of the sidebar.
     */
    void addButton(QPushButton *button);

    /**
     * Inserts an action button at the given button index in the button
     * area of the bottom of the sidebar.  Valid button index values are
     * from zero to buttonCount().
     */
    void insertButton(int index, QPushButton *button);

    /**
     * Removes the action button at the given button index from the button
     * area of the bottom of the sidebar.  Valid button index values are
     * from zero to buttonCount() - 1.
     */
    void removeButton(int index);

    /**
     * Returns the number of tabs in the sidebar.
     */
    int tabCount() const;

    /**
     * Returns the number of action buttons in the button area of the
     * bottom of the sidebar.
     */
    int buttonCount() const;

    /**
     * Sets whether this sidebar should hide itself when it loses focus.
     */
    void setAutoHideEnabled(bool enabled);

    /**
     * Returns whether this sidebar will hide itself when it loses focus.
     */
    bool autoHideEnabled() const;

signals:
    /**
     * Emitted when the sidebar's visibility has changed.
     */
    void visibilityChanged(bool visible);

protected slots:
    void onFocusChanged(QWidget *old, QWidget *now);

protected:
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);

private:
    QScopedPointer<SidebarPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // SIDEBAR_H
