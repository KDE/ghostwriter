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

#ifndef SUNDOWNEXPORTER_H
#define SUNDOWNEXPORTER_H

#include "Exporter.h"

/**
 * Exports Markdown text to HTML via the built-in Sundown processor.
 */
class SundownExporter : public Exporter
{
    public:
        /**
         * Constructor.
         */
        SundownExporter();

        /**
         * Destructor.
         */
        ~SundownExporter();

        /**
         * Exports the given Markdown text to HTML, setting the html parameter
         * to have the HTML output.
         */
        void exportToHtml(const QString& text, QString& html);

        /**
         * Exports the given Markdown text to the given export format and
         * output file path.  Sets err to a non-null string error message
         * if the export fails.  Note that the only supported format for
         * this exporter is HTML.
         */
        void exportToFile
        (
            const ExportFormat* format,
            const QString& inputFilePath,
            const QString& text,
            const QString& outputFilePath,
            QString& err
        );
};

#endif // SUNDOWNEXPORTER_H
