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

#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include <QColor>
#include <QPushButton>

class ColorButton : public QPushButton
{
	Q_OBJECT

public:
	ColorButton(QWidget* parent = 0);

	QColor color() const;
	QString toString() const;

signals:
	void changed(const QColor& color);

public slots:
	void setColor(const QColor& color);

private slots:
	void onClicked();

private:
	QColor m_color;
};

inline QColor ColorButton::color() const
{
	return m_color;
}

#endif
