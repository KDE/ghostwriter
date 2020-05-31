/***********************************************************************
 *
 * Copyright (C) 2020 wereturtle
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

#include <errno.h>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QTextStream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CmarkGfmAPI.h"
#include "CmarkGfmExporter.h"



CmarkGfmExporter::CmarkGfmExporter() : Exporter("cmark-gfm")
{
    supportedFormats.append(ExportFormat::HTML);
}

CmarkGfmExporter::~CmarkGfmExporter()
{

}

void CmarkGfmExporter::exportToHtml(const QString& text, QString& html)
{
    html = CmarkGfmAPI::getInstance()->renderToHtml(text, this->smartTypographyEnabled);
}

void CmarkGfmExporter::exportToFile
(
    const ExportFormat* format,
    const QString& inputFilePath,
    const QString& text,
    const QString& outputFilePath,
    QString& err
)
{
    Q_UNUSED(inputFilePath);

    QString html;

    if (ExportFormat::HTML != format)
    {
        err = QObject::tr("%1 format is unsupported by the cmark-gfm processor.")
            .arg(format->getName());
        return;
    }

    exportToHtml(text, html);

    if (html.isNull())
    {
        err = QObject::tr("Export failed");
        return;
    }

    QFile outputFile(outputFilePath);

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        err = outputFile.errorString();
        return;
    }

    // Write contents to disk.
    QTextStream outStream(&outputFile);

    // Specify the character set (UTF-8) for the HTML document.
    // Browsers typically can't tell if the HTML has unicode characters
    // unless UTF-8 is specified in the <head> section.
    //
    outStream << "<html><head><meta http-equiv=\"Content-Type\" "
        "content=\"text/html; charset=utf-8\" />"
        "<title></title></head><body>";

    outStream << html;
    outStream << "</body></html>";

    if (QFile::NoError != outputFile.error())
    {
        err = outputFile.errorString();
    }

    // Close the file.  All done!
    outputFile.close();
}
