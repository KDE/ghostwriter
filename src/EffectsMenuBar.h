/***********************************************************************
 *
 * Copyright (C) 2016 wereturtle
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

#ifndef EFFECTSMENUBAR_H
#define EFFECTSMENUBAR_H

#include <QMenuBar>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QKeyEvent>

/**
 * Menu bar that can do drop shadows on its items or be hidden/shown
 * (for use in full screen mode).  Drop shadows are included in this
 * class because the autohide capability uses a QGraphicsEffect.
 * Since drop shadows also use a QGraphicsEffect, and since only one
 * QGraphicsEffect can be used at a time on a widget, this class
 * facilitates easy switching between the two effects as needed.
 */
class EffectsMenuBar : public QMenuBar
{
    Q_OBJECT

    public:
        /**
         * Constructor.
         */
        explicit EffectsMenuBar(QWidget* parent = 0);

        /**
         * Destructor.
         */
        ~EffectsMenuBar();

        /**
         * Adds a QGraphicsDropShadowEffect drop shadow to the menu bar using
         * the specified color, blur radius, and x and y offsets.
         */
        void setDropShadow
        (
            const QColor& color,
            qreal blurRadius,
            qreal xOffset,
            qreal yOffset
        );

        /**
         * Removes any drop shadow previously applied to the menu bar.
         */
        void removeDropShadow();

        /**
         * Sets whether auto hiding the menu bar is enabled.
         */
        void setAutoHideEnabled(bool enabled);

        /**
         * Returns true of the menu bar is currently visible.
         */
        bool isVisible() const;

        /*
         * Shows the menu bar.
         */
        void showBar();

        /*
         * Hides the menu bar.  (Note that this is instead of calling QWidget's hide()
         * method, since we want shortcut keys for the actions to still be
         * available.)
         */
        void hideBar();

    public slots:
        /**
         * Connect this method to QMenu's aboutToShow() signal when adding
         * the menu to this menu bar.
         */
        void onAboutToShow();

        /**
         * Connect this method to QMenu's aboutToHide() signal when adding
         * the menu to this menu bar.
         */
        void onAboutToHide();

    private:
        QColor dropShadowColor;
        qreal dropShadowBlurRadius;
        qreal dropShadowXOffset;
        qreal dropShadowYOffset;
        QGraphicsDropShadowEffect* dropShadowEffect;
        QGraphicsOpacityEffect* opacityEffect;
        bool menuIsVisible;
        bool dropShadowEnabled;
        bool autoHideEnabled;
        bool mouseIsHovering;
        bool menuActivated;

        /*
         * Detects when the mouse hovers over the menu bar, so that we can
         * show it again.
         */
        void enterEvent(QEvent* event);

        /*
         * Detects when the mouse leaves the menu bar, so that we can
         * hide it again.
         */
        void leaveEvent(QEvent* event);
};

#endif // EFFECTSMENUBAR_H
