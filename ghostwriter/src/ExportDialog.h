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

#include <QDialog>

#include "TextDocument.h"

class QFileDialog;
class QComboBox;
class QCheckBox;

/**
 * A custom file dialog for exporting a document to a number of formats.  Export
 * logic is performed by Exporters, which are provided by ExporterFactory.  The
 * user can select which exporter to use in a combo box.  Also, an option for
 * enabling/disabling smart typography during export is provided in the form of
 * a checkbox.
 */
class ExportDialog : public QDialog
{
    Q_OBJECT

    public:
        /**
         * Constructor that takes text document to export as the parameter,
         * as well as the parent widget which will own this dialog.
         */
        ExportDialog(TextDocument* document, QWidget* parent = 0);
        virtual ~ExportDialog();

    signals:
        /**
         * Emitted when an export operation has begun, so that some sort of
         * notification can be provided to the user by the calling program,
         * such as displaying a progress bar.
         */
        void exportStarted(const QString& description);

        /**
         * Emitted when an exprot operation has completed, so that some sort of
         * notification can be provided to the user by the calling program,
         * such as removing a progress bar previously shown when the export
         * operation began.
         */
        void exportComplete();

    private slots:
        /*
         * Called when the user clicks on Save button.
         */
        void accept();

        /*
         * Called when the user clicks on the Cancel button.
         */
        void reject();

        /*
         * Called when the user changes which exporter to use via selecting
         * a different option in the combo box.
         */
        void onExporterChanged(int index);

        /*
         * Called when the user changes the file filter, i.e., the file format
         * to which to export the document;
         */
        void onFilterSelected(const QString& filter);

    private:
        QFileDialog* fileDialogWidget;
        QComboBox* exporterComboBox;
        QCheckBox* smartTypographyCheckBox;
        TextDocument* document;
        QStringList fileFilters;
};

#endif // EXPORTDIALOG_H
