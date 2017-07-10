/***********************************************************************
 *
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

#ifndef PREVIEWOPTIONSDIALOG_H
#define PREVIEWOPTIONSDIALOG_H

#include <QObject>
#include <QDialog>

#include "ExporterFactory.h"

class QComboBox;

class PreviewOptionsDialog : public QDialog
{
    Q_OBJECT

    public:
        PreviewOptionsDialog(QWidget* parent = NULL);
        virtual ~PreviewOptionsDialog();

    private slots:
        void onExporterChanged(int index);
        void onStyleSheetChanged(int index);
        void onCustomCssFilesChanged(const QStringList& fileList);


    private:
        ExporterFactory* exporterFactory;
        QStringList defaultStyleSheets;
        QComboBox* previewerComboBox;
        QComboBox* styleSheetComboBox;
        QStringList customCssFiles;

        void buildStyleSheetComboBox();
};

#endif // PREVIEWOPTIONSDIALOG_H
