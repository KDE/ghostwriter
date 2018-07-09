/***********************************************************************
 *
 * Copyright (C) 2014-2017 wereturtle
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

#ifndef HUDWINDOW_H
#define HUDWINDOW_H

#include <QWidget>

#include "HudWindowTypes.h"

class QGridLayout;
class QMouseEvent;
class QPaintEvent;
class QPoint;
class QColor;
class QLabel;
class QSizeGrip;
class QPushButton;
class QGraphicsColorizeEffect;

/**
 * A Heads Up Display dialog window, similar to Mac OS X HUD windows.
 */
class HudWindow : public QWidget
{
    Q_OBJECT

    public:
        /**
         * Constructor.
         */
        HudWindow(QWidget *parent = 0);

        /**
         * Destructor.
         */
        virtual ~HudWindow();

        /**
         * Listens for key press events, and hides the HUD if the Escape key is
         * pressed.
         */
        void keyPressEvent(QKeyEvent* e);

        /**
         * Sets the central widget to display in this HUD.
         */
        void setCentralWidget(QWidget* widget);

        /**
         * Sets the HUD window title.
         */
        void setWindowTitle(const QString& title);

        /**
         * Gets the HUD foreground (i.e., text) color.
         */
        QColor getForegroundColor();

        /**
         * Sets the HUD foreground (i.e., text) color.
         */
        void setForegroundColor(const QColor& color);

        /**
         * Gets the HUD background color.
         */
        QColor getBackgroundColor();

        /**
         * Sets the background color of the window.  Setting an alpha value
         * in the color will change the opacity of the window, but not any
         * of its child widgets.  To set an opacity value that will include
         * all child widgets added to the window, call setWindowOpacity()
         * instead.
         */
        void setBackgroundColor(const QColor& color);

        /**
         * Sets whether the size grip in the lower right corner of the HUD
         * window should be displayed.
         */
        void setSizeGripEnabled(bool enabled);

        /**
         * Sets the window button layout (left or right side).  Defaults to
         * the right-hand side for Windows and Linux, and to the left-hand
         * side for Mac OS X.
         */
        void setButtonLayout(HudWindowButtonLayout layout);

        /**
         * Sets the shape of the window to have either square or rounded
         * corners.  Note that if desktop compositing is disabled, the
         * corners will be square regardless of this setting.
         */
        void setShape(HudWindowShape shape);

        /**
         * Sets whether desktop compositing is enabled for the HUD.  Enabling
         * will allow transparency of the HUD, as well as the drop shadow
         * and rounded corners of the window.  Disabling will cause a plain
         * rectangle to be drawn with no transparency.  This method is provided
         * in case the application runs on a platform that does not support
         * desktop compositing, so that the user can turn off the transparency
         * effect and not have an unpleasant-looking window.
         */
        void setDesktopCompositingEnabled(bool enabled);

    signals:
        void closed();

    protected:
        QGridLayout* layout;
        QLabel* windowTitleLabel;
        QPushButton* closeButton;
        QSizeGrip* sizeGrip;
        QColor foregroundColor;
        QColor backgroundColor;
        QColor titleBarButtonHoverColor;
        QPixmap dropShadowImg;
        QGraphicsColorizeEffect* closeButtonColorEffect;
        QGraphicsColorizeEffect* sizeGripColorEffect;
        bool isTitleBarBeingDragged;
        QPoint mouseDragStartingPoint;
        bool desktopCompositingEnabled;
        QGridLayout* titleBarLayout;
        HudWindowShape hudWindowShape;

        QSize sizeHint() const;
        void paintEvent(QPaintEvent* event);
        void resizeEvent(QResizeEvent* event);
        void moveEvent(QMoveEvent* event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);

    private:
        void predrawDropShadow();

};

#endif // HUDWINDOW_H
