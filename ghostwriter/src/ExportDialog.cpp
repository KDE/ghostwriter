/***********************************************************************
 *
 * Copyright (C) 2014-2017 wereturtle
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

#include <QFileInfo>
#include <QList>
#include <QString>
#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>

#include "ExportDialog.h"
#include "ExporterFactory.h"
#include "Exporter.h"
#include "MessageBoxHelper.h"

#define GW_LAST_EXPORTER_KEY "Export/lastUsedExporter"
#define GW_SMART_TYPOGRAPHY_KEY "Export/smartTypographyEnabled"

ExportDialog::ExportDialog(TextDocument* document, QWidget* parent)
    : QDialog(parent), document(document)
{
    QList<Exporter*> exporters =
        ExporterFactory::getInstance()->getFileExporters();

    this->setWindowTitle(tr("Export"));
    fileDialogWidget = new QFileDialog(this, Qt::Widget);
    fileDialogWidget->setAcceptMode(QFileDialog::AcceptSave);
    fileDialogWidget->setFileMode(QFileDialog::AnyFile);
    fileDialogWidget->setWindowFlags(Qt::Widget);
    fileDialogWidget->setOption(QFileDialog::DontUseNativeDialog, true);

    QList<QUrl> shortcutFolders;

#if QT_VERSION >= 0x050000
    QStringList standardLocations =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    standardLocations <<
        QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    standardLocations <<
        QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);

    // Adds the "My Computer" shortcut.
    shortcutFolders << QUrl("file:");

    foreach (QString loc, standardLocations)
    {
        shortcutFolders << QUrl::fromLocalFile(loc);
    }

#else
    shortcutFolders
        << QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation))
        << QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation))
        << QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
#endif

    // In Qt 5, the QFileDialog will be scrunched unless setVisible is called.
    // Also, Qt 5 also will not show custom sidebar URLs unless setVisible() is
    // called first.  Note that this is not an issue in Qt 4.
    //
    fileDialogWidget->setVisible(true);

    fileDialogWidget->setSidebarUrls(shortcutFolders);

    connect(fileDialogWidget, SIGNAL(accepted()), this, SLOT(accept()));
    connect(fileDialogWidget, SIGNAL(rejected()), this, SLOT(reject()));

    exporterComboBox = new QComboBox();
    int selectedIndex = 0;

    QSettings settings;
    QString lastExporterName =
        settings.value(GW_LAST_EXPORTER_KEY, QString()).toString();

    for (int i = 0; i < exporters.length(); i++)
    {
        Exporter* exporter = exporters[i];

        exporterComboBox->addItem
        (
            exporter->getName(),
            qVariantFromValue((void *) exporters[i])
        );

        QList<const ExportFormat*> formats =
            exporter->getSupportedFormats();

        // Build file filter list
        QString filter = "";

        for (int j = 0; j < formats.length(); j++)
        {
            filter += formats[j]->getNamedFilter();

            if ((j + 1) < formats.length())
            {
                filter += QString(";;");
            }
        }

        fileFilters.append(filter);

        if (exporter->getName() == lastExporterName)
        {
            selectedIndex = i;
        }
    }

    exporterComboBox->setCurrentIndex(selectedIndex);
    fileDialogWidget->setNameFilter(fileFilters[selectedIndex]);

    QString initialDirPath;
    QString baseName;

    if (!document->getFilePath().isNull() && !document->getFilePath().isEmpty())
    {
        QFileInfo inputFileInfo(document->getFilePath());
        initialDirPath = inputFileInfo.dir().path();
        baseName = inputFileInfo.baseName();
    }
    else
    {
#if QT_VERSION >= 0x050000
        standardLocations =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);

        if (standardLocations.size() > 0)
        {
            initialDirPath = standardLocations[0];
        }
#else
        initialDirPath =
            QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
    }

    fileDialogWidget->setDirectory(initialDirPath);

    if (exporters[selectedIndex]->getSupportedFormats().length() > 0)
    {
        const ExportFormat* currentFormat =
            exporters[selectedIndex]->getSupportedFormats().at(0);
        QString fileSuffix = currentFormat->getDefaultFileExtension();

        if
        (
            !fileSuffix.isNull() &&
            !fileSuffix.isEmpty()
        )
        {
            if (!baseName.isNull())
            {
                fileDialogWidget->selectFile(initialDirPath + "/" +
                    baseName + "." + fileSuffix);
            }
        }

        if (currentFormat->isFileExtensionMandatory())
        {
            fileDialogWidget->setDefaultSuffix(currentFormat->getDefaultFileExtension());
        }
    }

    bool smartTypographyEnabled =
        settings.value(GW_SMART_TYPOGRAPHY_KEY, true).toBool();
    smartTypographyCheckBox = new QCheckBox(tr("Smart Typography"));
    smartTypographyCheckBox->setChecked(smartTypographyEnabled);

    QGroupBox* optionsGroupBox = new QGroupBox(tr("Export Options"));
    QGridLayout* optionsLayout = new QGridLayout();
    optionsLayout->addWidget(new QLabel(tr("Markdown Converter:")), 0, 0, 1, 1);
    optionsLayout->addWidget(exporterComboBox, 0, 1, 1, 1, Qt::AlignLeft);
    optionsLayout->addWidget(smartTypographyCheckBox, 0, 2, 1, 2);
    optionsGroupBox->setLayout(optionsLayout);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(fileDialogWidget);
    layout->addWidget(optionsGroupBox);
    this->setLayout(layout);

    connect(exporterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onExporterChanged(int)));
    connect(fileDialogWidget, SIGNAL(filterSelected(QString)), this, SLOT(onFilterSelected(QString)));
}

ExportDialog::~ExportDialog()
{
    QSettings settings;
    settings.setValue(GW_LAST_EXPORTER_KEY, exporterComboBox->currentText());
    settings.setValue(GW_SMART_TYPOGRAPHY_KEY, smartTypographyCheckBox->isChecked());
}

void ExportDialog::accept()
{
    QDialog::accept();
    this->setResult(fileDialogWidget->result());

    QStringList selectedFiles = fileDialogWidget->selectedFiles();

    if (!selectedFiles.isEmpty())
    {
        QString fileName = selectedFiles.at(0);
        QString selectedFilter = fileDialogWidget->selectedNameFilter();
        int selectedIndex = exporterComboBox->currentIndex();
        QVariant exporterVariant = exporterComboBox->itemData(selectedIndex);
        Exporter* exporter = (Exporter*) exporterVariant.value<void*>();

        foreach (const ExportFormat* format, exporter->getSupportedFormats())
        {
            if (format->getNamedFilter() == selectedFilter)
            {
                QString err;

                QApplication::setOverrideCursor(Qt::WaitCursor);
                emit exportStarted(tr("exporting to %1").arg(fileName));

                exporter->setSmartTypographyEnabled(smartTypographyCheckBox->isChecked());
                exporter->exportToFile
                (
                    format,
                    this->document->getFilePath(),
                    document->toPlainText(),
                    fileName,
                    err
                );

                emit exportComplete();
                QApplication::restoreOverrideCursor();

                if (!err.isNull())
                {
                    MessageBoxHelper::critical(this, tr("Export failed."), err);
                }
                else
                {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
                }

                break;
            }
        }
    }
}

void ExportDialog::reject()
{
    QDialog::reject();
    this->setResult(fileDialogWidget->result());
}

void ExportDialog::onExporterChanged(int index)
{
    QVariant exporterVariant = exporterComboBox->itemData(index);
    Exporter* exporter = (Exporter*) exporterVariant.value<void*>();

    if (exporter->getSupportedFormats().length() > 0)
    {
        const ExportFormat* format = exporter->getSupportedFormats().at(0);

        if (format->isFileExtensionMandatory())
        {
            fileDialogWidget->setDefaultSuffix(format->getDefaultFileExtension());
        }
    }

    fileDialogWidget->setNameFilter(fileFilters[index]);
}

void ExportDialog::onFilterSelected(const QString& filter)
{
    int selectedIndex = exporterComboBox->currentIndex();
    QVariant exporterVariant = exporterComboBox->itemData(selectedIndex);
    Exporter* exporter = (Exporter*) exporterVariant.value<void*>();

    foreach (const ExportFormat* format, exporter->getSupportedFormats())
    {
        if (format->getNamedFilter() == filter)
        {
            if (format->isFileExtensionMandatory())
            {
                fileDialogWidget->setDefaultSuffix(format->getDefaultFileExtension());
            }

            break;
        }
    }
}
