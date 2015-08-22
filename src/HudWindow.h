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

#ifndef HUDWINDOW_H
#define HUDWINDOW_H

#include <QWidget>

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
        HudWindow(QWidget *parent = 0);
        virtual ~HudWindow();
        void setCentralWidget(QWidget* widget);
        void setWindowTitle(const QString& title);
        QColor getForegroundColor();
        void setForegroundColor(const QColor& color);
        QColor getBackgroundColor();

        /**
         * Sets the background color of the window.  Setting an alpha value
         * in the color will change the opacity of the window, but not any
         * of its child widgets.  To set an opacity value that will include
         * all child widgets added to the window, call setWindowOpacity()
         * instead.
         */
        void setBackgroundColor(const QColor& color);

        void setSizeGripEnabled(bool enabled);

    public slots:
        void setDesktopCompositingEnabled(bool enabled);

    protected:
        QGridLayout* layout;
        QLabel* windowTitleLabel;
        QPushButton* closeButton;
        QSizeGrip* sizeGrip;
        QColor foregroundColor;
        QColor backgroundColor;
        QColor titleBarButtonHoverColor;
        QImage dropShadowImg;
        QGraphicsColorizeEffect* closeButtonColorEffect;
        QGraphicsColorizeEffect* sizeGripColorEffect;
        bool isTitleBarBeingDragged;
        QPoint mouseDragStartingPoint;
        bool desktopCompositingEnabled;

        QSize sizeHint() const;
        void paintEvent(QPaintEvent* event);
        void resizeEvent(QResizeEvent* event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        bool eventFilter(QObject* obj, QEvent* event);

    private:
        void resetTitleButtonHoverColor();

};

#endif // HUDWINDOW_H
