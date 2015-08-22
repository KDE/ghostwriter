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

#include <QFileInfo>
#include <QList>
#include <QString>
#include <QApplication>

#include "ExportDialog.h"
#include "ExporterFactory.h"
#include "Exporter.h"
#include "MessageBoxHelper.h"

ExportDialog::ExportDialog(TextDocument* document, QWidget* parent)
    : document(document), QFileDialog(parent)
{
    QString initialDirPath = QString();

    if (!document->getFilePath().isNull() && !document->getFilePath().isEmpty())
    {
        QFileInfo inputFileInfo(document->getFilePath());
        initialDirPath = inputFileInfo.dir().path();
    }

    QList<Exporter*> exporters =
        ExporterFactory::getInstance()->getFileExporters();
    QString filter = "";

    // Build file filter list
    for (int i = 0; i < exporters.length(); i++)
    {
        filter += exporters.at(i)->getFileFormatFilter();

        if ((i + 1) < exporters.length())
        {
            filter += QString(";;");
        }
    }

    this->setDirectory(initialDirPath);
    this->setNameFilter(filter);
    this->setAcceptMode(QFileDialog::AcceptSave);
    this->setFileMode(QFileDialog::AnyFile);
}

ExportDialog::~ExportDialog()
{

}

void ExportDialog::accept()
{
    QFileDialog::accept();

    if (QDialog::Accepted != this->result())
    {
        return;
    }

    QStringList selectedFiles = this->selectedFiles();

    if (!selectedFiles.isEmpty())
    {
        QString fileName = selectedFiles.at(0);
        QString selectedFilter = this->selectedNameFilter();
        QList<Exporter*> exporters =
            ExporterFactory::getInstance()->getFileExporters();

        for (int i = 0; i < exporters.length(); i++)
        {
            Exporter* exporter = exporters.at(i);

            if (exporter->getFileFormatFilter() == selectedFilter)
            {
                QString err;

                QApplication::setOverrideCursor(Qt::WaitCursor);
                emit exportStarted(tr("exporting to %1").arg(fileName));

                exporter->exportToFile
                (
                    document->toPlainText(),
                    fileName, err
                );

                emit exportComplete();
                QApplication::restoreOverrideCursor();

                if (!err.isNull())
                {
                    MessageBoxHelper::critical(this, tr("Export failed."), err);
                }

                break;
            }
        }
    }
}

void ExportDialog::reject()
{
    QDialog::reject();
}
