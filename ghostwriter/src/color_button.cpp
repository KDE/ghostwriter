/***********************************************************************
 *
 * Copyright (C) 2008, 2009, 2010 Graeme Gott <graeme@gottcode.org>
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

#include "color_button.h"

#include <QColorDialog>
#include <QPainter>
#include <QPixmap>

//-----------------------------------------------------------------------------

ColorButton::ColorButton(QWidget* parent)
	: QPushButton(parent)
{
	setAutoDefault(false);
	connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

//-----------------------------------------------------------------------------

QString ColorButton::toString() const
{
	return m_color.name();
}

//-----------------------------------------------------------------------------

void ColorButton::setColor(const QColor& color)
{
	if (m_color == color) {
		return;
	}
	m_color = color;

	QPixmap swatch(75, fontMetrics().height());
	swatch.fill(m_color);
	{
		QPainter painter(&swatch);
		painter.setPen(m_color.darker());
		painter.drawRect(0, 0, swatch.width() - 1, swatch.height() - 1);
		painter.setPen(m_color.lighter());
		painter.drawRect(1, 1, swatch.width() - 3, swatch.height() - 3);
	}
	setIconSize(swatch.size());
	setIcon(swatch);

	emit changed(m_color);
}

//-----------------------------------------------------------------------------

void ColorButton::onClicked()
{
    QColor color = QColorDialog::getColor(m_color, this);
	if (color.isValid()) {
		setColor(color);
	}
}

//-----------------------------------------------------------------------------
