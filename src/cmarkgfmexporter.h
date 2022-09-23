/*
 * SPDX-FileCopyrightText: 2020-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CMARK_GFM_EXPORTER_H
#define CMARK_GFM_EXPORTER_H

#include "exporter.h"

namespace ghostwriter
{
/**
 * Exports Markdown text to HTML via the built-in cmark-gfm processor.
 */
class CmarkGfmExporter : public Exporter
{
public:
    /**
     * Constructor.
     */
    CmarkGfmExporter();

    /**
     * Destructor.
     */
    ~CmarkGfmExporter();

    /**
     * Exports the given Markdown text to HTML, setting the html parameter
     * to have the HTML output.
     */
    void exportToHtml(const QString &text, QString &html);

    /**
     * Exports the given Markdown text to the given export format and
     * output file path.  Sets err to a non-null string error message
     * if the export fails.  Note that the only supported format for
     * this exporter is HTML.
     */
    void exportToFile
    (
        const ExportFormat *format,
        const QString &inputFilePath,
        const QString &text,
        const QString &outputFilePath,
        QString &err
    );
};
}

#endif // CMARK_GFM_EXPORTER_H
