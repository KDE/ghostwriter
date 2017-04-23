/***********************************************************************
 *
 * Copyright (C) 2008, 2009, 2010 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) 2017 wereturtle
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

#ifndef IMAGE_BUTTON_H
#define IMAGE_BUTTON_H

#include <QPushButton>

class ImageButton : public QPushButton
{
	Q_OBJECT

public:
    ImageButton(qreal devicePixelRatio, QWidget* parent = 0);

	QString image() const;
	QString toString() const;

signals:
	void changed(const QString& path);

public slots:
	void setImage(const QString& image, const QString& path);
	void unsetImage();

private slots:
	void onClicked();

private:
    qreal devicePixelRatio;
	QString m_image;
    QString m_path;
};

inline QString ImageButton::image() const {
	return m_image;
}

inline QString ImageButton::toString() const {
	return m_path;
}

#endif
