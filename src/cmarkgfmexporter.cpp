/*
 * SPDX-FileCopyrightText: 2020-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QTextStream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmarkgfmexporter.h"

#include "cmarkgfmapi.h"

namespace ghostwriter
{
CmarkGfmExporter::CmarkGfmExporter() : Exporter("cmark-gfm")
{
    m_supportedFormats.append(ExportFormat::HTML);
}

CmarkGfmExporter::~CmarkGfmExporter()
{

}

void CmarkGfmExporter::exportToHtml(const QString &text, QString &html)
{
    html = CmarkGfmAPI::instance()->renderToHtml(text, this->m_smartTypographyEnabled);
}

void CmarkGfmExporter::exportToFile
(
    const ExportFormat *format,
    const QString &inputFilePath,
    const QString &text,
    const QString &outputFilePath,
    QString &err
)
{
    Q_UNUSED(inputFilePath);

    QString html;

    if (ExportFormat::HTML != format) {
        err = QObject::tr("%1 format is unsupported by the cmark-gfm processor.")
              .arg(format->name());
        return;
    }

    exportToHtml(text, html);

    if (html.isNull()) {
        err = QObject::tr("Export failed");
        return;
    }

    QFile outputFile(outputFilePath);

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
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

    if (QFile::NoError != outputFile.error()) {
        err = outputFile.errorString();
    }

    // Close the file.  All done!
    outputFile.close();
}
}
