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


#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QFileDialog>
#include <QString>

#include "TextDocument.h"

class ExportDialog : public QFileDialog
{
    Q_OBJECT

    public:
        ExportDialog(TextDocument* document, QWidget* parent = 0);
        virtual ~ExportDialog();

    signals:
        void exportStarted(const QString& description);
        void exportComplete();

    public slots:
        void accept();
        void reject();

    private:
        TextDocument* document;
};

#endif // EXPORTDIALOG_H
