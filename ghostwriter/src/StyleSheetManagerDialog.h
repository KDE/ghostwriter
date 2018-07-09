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

#ifndef STYLESHEETMANAGERDIALOG_H
#define STYLESHEETMANAGERDIALOG_H

#include <QDialog>

class QListWidget;

/**
 * Manages the addition and removal of the user's Custom Style Sheet (CSS) files
 * for use in the Live HTML Preview.
 */
class StyleSheetManagerDialog : public QDialog
{
    Q_OBJECT

    public:
        /**
         * Constructor.  Takes list of initial style sheets to display as a
         * parameter.
         */
        StyleSheetManagerDialog
        (
            const QStringList& initialStyleSheets,
            QWidget *parent = 0
        );

        /**
         * Destructor.
         */
        ~StyleSheetManagerDialog();

        /**
         * Gets the list of user style sheets (as file paths).  Call this
         * method after exec() returns.
         */
        QStringList getStyleSheets() const;

    private slots:
        void accept();
        void reject();

    private slots:
        void addNewStyleSheet();
        void removeSelectedStyleSheet();

    private:
        QListWidget* styleSheetListWidget;
        QStringList styleSheets;

};

#endif // STYLESHEETMANAGERDIALOG_H
