/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

#include "markdowndocument.h"

class QFileDialog;
class QComboBox;
class QCheckBox;

namespace ghostwriter
{
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
    ExportDialog(MarkdownDocument *document, QWidget *parent = 0);
    virtual ~ExportDialog();

signals:
    /**
     * Emitted when an export operation has begun, so that some sort of
     * notification can be provided to the user by the calling program,
     * such as displaying a progress bar.
     */
    void exportStarted(const QString &description);

    /**
     * Emitted when an export operation has completed, so that some sort of
     * notification can be provided to the user by the calling program,
     * such as removing a progress bar previously shown when the export
     * operation began.
     */
    void exportComplete();

private slots:
    /*
    * Called when the user clicks on Export button.
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

private:
    QComboBox *fileFormatComboBox;
    QComboBox *exporterComboBox;
    QCheckBox *smartTypographyCheckBox;
    MarkdownDocument *document;
};
} // namespace ghostwriter

#endif // EXPORTDIALOG_H
