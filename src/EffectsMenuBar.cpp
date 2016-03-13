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

#include "EffectsMenuBar.h"

EffectsMenuBar::EffectsMenuBar(QWidget *parent) :
    QMenuBar(parent),
    dropShadowEffect(NULL),
    opacityEffect(NULL),
    menuIsVisible(true),
    dropShadowEnabled(false),
    autoHideEnabled(false),
    mouseIsHovering(false),
    menuActivated(false)
{

}

EffectsMenuBar::~EffectsMenuBar()
{

}

void EffectsMenuBar::setDropShadow
(
    const QColor& color,
    qreal blurRadius,
    qreal xOffset,
    qreal yOffset
)
{
    dropShadowEnabled = true;
    dropShadowColor = color;
    dropShadowBlurRadius = blurRadius;
    dropShadowXOffset = xOffset;
    dropShadowYOffset = yOffset;

    if (menuIsVisible)
    {
        dropShadowEffect = new QGraphicsDropShadowEffect();
        dropShadowEffect->setColor(color);
        dropShadowEffect->setBlurRadius(blurRadius);
        dropShadowEffect->setXOffset(xOffset);
        dropShadowEffect->setYOffset(yOffset);
        this->setGraphicsEffect(dropShadowEffect);
    }
}

void EffectsMenuBar::removeDropShadow()
{
    if ((NULL != dropShadowEffect) && dropShadowEnabled && menuIsVisible)
    {
        this->setGraphicsEffect(NULL);
    }

    dropShadowEffect = NULL;
    dropShadowEnabled = false;
}

void EffectsMenuBar::setAutoHideEnabled(bool enabled)
{
    autoHideEnabled = enabled;

    if (autoHideEnabled)
    {
        hideBar();
    }
    else
    {
        showBar();
    }
}

void EffectsMenuBar::onAboutToShow()
{
    menuActivated = true;
    showBar();
}

void EffectsMenuBar::onAboutToHide()
{
    menuActivated = false;
    hideBar();
}

void EffectsMenuBar::showBar()
{
    if (!menuIsVisible)
    {
        this->setGraphicsEffect(NULL);
        opacityEffect = NULL;
        this->menuIsVisible = true;

        if (dropShadowEnabled)
        {
            dropShadowEffect = new QGraphicsDropShadowEffect();
            dropShadowEffect->setColor(dropShadowColor);
            dropShadowEffect->setBlurRadius(dropShadowBlurRadius);
            dropShadowEffect->setXOffset(dropShadowXOffset);
            dropShadowEffect->setYOffset(dropShadowYOffset);
            this->setGraphicsEffect(dropShadowEffect);
        }
    }
}

void EffectsMenuBar::hideBar()
{
    if (autoHideEnabled && !mouseIsHovering)
    {
        if (dropShadowEnabled)
        {
            dropShadowEffect = NULL;
        }

        opacityEffect = new QGraphicsOpacityEffect();
        opacityEffect->setOpacity(0.0);
        opacityEffect->setEnabled(true);
        this->setGraphicsEffect(opacityEffect);
        this->menuIsVisible = false;
    }
}

void EffectsMenuBar::enterEvent(QEvent* event)
{
    mouseIsHovering = true;
    showBar();
    QMenuBar::enterEvent(event);
}

void EffectsMenuBar::leaveEvent(QEvent* event)
{
    mouseIsHovering = false;

    if (!menuActivated)
    {
        hideBar();
    }

    QMenuBar::leaveEvent(event);
}
