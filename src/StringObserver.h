/***********************************************************************
 *
 * Copyright (C) 2018-2019 wereturtle
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

#ifndef STRINGOBSERVER_H
#define STRINGOBSERVER_H

#include <QObject>
#include <QString>

/**
 * Observer pattern for a QString.  Used in notifying web channel in
 * QtWebEngine (Chromium) that a Qt C++ variable has been modified.
 */
class StringObserver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER mText NOTIFY textChanged FINAL)

    public:
        /**
         * Constructor.
         */
        explicit StringObserver(QObject* parent = nullptr);

        /**
         * Destructor.
         */
        virtual ~StringObserver();

        /**
         * Set the text of the string.
         */
        void setText(const QString& text);

        /**
         * Get the text of the string.
         */
        QString getText() const;

    signals:
        /**
         * Emitted when the string value changes.
         */
        void textChanged(const QString& text);

    private:
        /*
         * Internal string being observed.
         */
        QString mText;
};

#endif // STRINGOBSERVER_H
