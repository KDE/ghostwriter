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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "SundownExporter.h"

#include "sundown/markdown.h"
#include "sundown/html.h"
#include "sundown/buffer.h"


SundownExporter::SundownExporter() : Exporter("Sundown")
{
    supportedFormats.append(ExportFormat::HTML);
}

SundownExporter::~SundownExporter()
{

}

void SundownExporter::exportToHtml(const QString& text, QString& html)
{
    QByteArray latin1Text = text.toUtf8().data();
    struct buf* htmlOutputBuffer = bufnew(1024);
    struct sd_callbacks callbacks;
    struct html_renderopt options;
    struct sd_markdown* markdown;

    sdhtml_renderer(&callbacks, &options, 0);
    markdown = sd_markdown_new
    (
        MKDEXT_TABLES | MKDEXT_FENCED_CODE | MKDEXT_SPACE_HEADERS
            | MKDEXT_SUPERSCRIPT | MKDEXT_STRIKETHROUGH | MKDEXT_AUTOLINK,
        16,
        &callbacks,
        &options
    );

    sd_markdown_render
    (
        htmlOutputBuffer,
        (const uint8_t*) latin1Text.data(),
        latin1Text.length(),
        markdown
    );

    sd_markdown_free(markdown);

    if (this->getSmartTypographyEnabled())
    {
        // Run HTML through smarty pants to get fancy quotation marks, etc.
        struct buf* smartyPantsBuffer = bufnew(1024);

        sdhtml_smartypants
        (
            smartyPantsBuffer,
            htmlOutputBuffer->data,
            htmlOutputBuffer->size
        );

        // Use QString::fromUtf8 to ensure proper encoding in case there are
        // unicode characters in the output HTML.
        //
        html = QString::fromUtf8
            (
                (char*) smartyPantsBuffer->data,
                smartyPantsBuffer->size
            );

        bufrelease(smartyPantsBuffer);
    }
    else
    {
        // Use QString::fromUtf8 to ensure proper encoding in case there are
        // unicode characters in the output HTML.
        //
        html = QString::fromUtf8
            (
                (char*) htmlOutputBuffer->data,
                htmlOutputBuffer->size
            );
    }

    bufrelease(htmlOutputBuffer);
}

void SundownExporter::exportToFile
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
        err = QObject::tr("%1 format is unsupported by the Sundown processor.")
            .arg(format->getName());
        return;
    }

    exportToHtml(text, html);

    if (html.isNull())
    {
        err = "Export failed";
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
