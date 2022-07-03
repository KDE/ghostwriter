/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVBoxLayout>

#include "exportdialog.h"
#include "exporter.h"
#include "exporterfactory.h"
#include "messageboxhelper.h"

#define GW_LAST_EXPORTER_KEY "Export/lastUsedExporter"
#define GW_SMART_TYPOGRAPHY_KEY "Export/smartTypographyEnabled"

namespace ghostwriter
{
ExportDialog::ExportDialog(MarkdownDocument *document, QWidget *parent)
    : QDialog(parent), document(document)
{
    QList<Exporter *> exporters =
        ExporterFactory::instance()->fileExporters();

    this->setWindowTitle(tr("Export"));
    
    exporterComboBox = new QComboBox();

    QSettings settings;
    QString lastExporterName =
        settings.value(GW_LAST_EXPORTER_KEY, QString()).toString();

    for (int i = 0; i < exporters.length(); i++) {
        Exporter *exporter = exporters[i];

        exporterComboBox->addItem
        (
            exporter->name(),
            QVariant::fromValue((void *) exporters[i])
        );

        if (exporter->name() == lastExporterName) {
            exporterComboBox->setCurrentIndex(i);
        }
    }

    QVariant exporterVariant = exporterComboBox->currentData();
    Exporter *exporter = (Exporter *) exporterVariant.value<void *>();
    fileFormatComboBox = new QComboBox();
    
    foreach (const ExportFormat *format, exporter->supportedFormats()) {
        fileFormatComboBox->addItem(format->name(), QVariant::fromValue((void *) format));
    }
    
    bool smartTypographyEnabled =
        settings.value(GW_SMART_TYPOGRAPHY_KEY, true).toBool();
    smartTypographyCheckBox = new QCheckBox(tr("Smart Typography"));
    smartTypographyCheckBox->setChecked(smartTypographyEnabled);

    QGroupBox *optionsGroupBox = new QGroupBox(tr("Export Options"));
    QFormLayout *optionsLayout = new QFormLayout();
    optionsLayout->addRow(tr("Markdown Converter"), exporterComboBox);
    optionsLayout->addRow(tr("File Format"), fileFormatComboBox);
    optionsLayout->addRow(smartTypographyCheckBox);
    optionsGroupBox->setLayout(optionsLayout);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(optionsGroupBox);
    this->setLayout(layout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton *exportButton = new QPushButton(tr("Export"));
    buttonBox->addButton(exportButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    
    connect(exporterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onExporterChanged(int)));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

ExportDialog::~ExportDialog()
{
    QSettings settings;
    settings.setValue(GW_LAST_EXPORTER_KEY, exporterComboBox->currentText());
    settings.setValue(GW_SMART_TYPOGRAPHY_KEY, smartTypographyCheckBox->isChecked());
}

void ExportDialog::accept()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, false);
    QStringList selectedFiles = fileDialog.selectedFiles();

    QString initialDirPath;
    QString baseName;

    if (!document->filePath().isNull() && !document->filePath().isEmpty()) {
        QFileInfo inputFileInfo(document->filePath());
        initialDirPath = inputFileInfo.dir().path();
        baseName = inputFileInfo.baseName();
    } else {
        QStringList standardLocations =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);

        if (standardLocations.size() > 0) {
            initialDirPath = standardLocations[0];
        }
    }

    QVariant exporterVariant = exporterComboBox->currentData();
    Exporter *exporter = (Exporter *) exporterVariant.value<void *>();

    ExportFormat *format = (ExportFormat *) fileFormatComboBox->currentData().value<void *>();
    QString fileSuffix = format->defaultFileExtension();

    if (!fileSuffix.isNull() && !fileSuffix.isEmpty() && !baseName.isNull()) {
        fileDialog.selectFile(initialDirPath + "/" +
                                        baseName + "." + fileSuffix);
    }

    fileDialog.setNameFilter(format->namedFilter());

    if (format->isFileExtensionMandatory()) {
        fileDialog.setDefaultSuffix(format->defaultFileExtension());
    }

    fileDialog.setDirectory(initialDirPath);

    int status = fileDialog.exec();
    this->setResult(fileDialog.result());

    if (!status) {
        return;
    }

    QString fileName = fileDialog.selectedFiles().at(0);
    QString err;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit exportStarted(tr("exporting to %1").arg(fileName));

    exporter->setSmartTypographyEnabled(smartTypographyCheckBox->isChecked());
    exporter->exportToFile
    (
        format,
        this->document->filePath(),
        document->toPlainText(),
        fileName,
        err
    );    
    
    emit exportComplete();
    QApplication::restoreOverrideCursor();

    if (!err.isNull()) {
        MessageBoxHelper::critical(this, tr("Export failed."), err);
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }

    QDialog::accept();
}

void ExportDialog::reject()
{
    QDialog::reject();
}

void ExportDialog::onExporterChanged(int index)
{
    QVariant exporterVariant = exporterComboBox->itemData(index);
    Exporter *exporter = (Exporter *) exporterVariant.value<void *>();
    fileFormatComboBox->clear();
    
    foreach (const ExportFormat *format, exporter->supportedFormats()) {
        fileFormatComboBox->addItem(format->name(), QVariant::fromValue((void *) format));
    }

    fileFormatComboBox->setCurrentIndex(0);

    QSettings settings;
    settings.setValue(GW_LAST_EXPORTER_KEY, exporter->name());
}

} // namespace ghostwriter
