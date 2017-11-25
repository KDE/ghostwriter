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

#ifndef EXPORTERFACTORY_H
#define EXPORTERFACTORY_H

#include <QList>

#include "Exporter.h"

/**
 * Creates Exporters for use with HTML live preview and exporting to disk.
 */
class ExporterFactory
{
    public:
        /**
         * Gets the singleton instance of this class.
         */
        static ExporterFactory* getInstance();

        /**
         * Destructor.
         */
        ~ExporterFactory();

        /**
         * Gets a list of Exporters that support exporting to a file.
         */
        QList<Exporter*> getFileExporters();

        /**
         * Gets a list of Exporters that support exporting to HTML for
         * HTML live preview rendering.
         */
        QList<Exporter*> getHtmlExporters();

        /**
         * Returns the exporter with the given name, or NULL if one
         * with that name does not exist.
         */
        Exporter* getExporterByName(const QString& name);

    private:
        static ExporterFactory* instance;
        QList<Exporter*> fileExporters;
        QList<Exporter*> htmlExporters;

        /*
         * Constructor.
         */
        ExporterFactory();

        /*
         * Executes the given terminal command to see if the executable is
         * installed and available.  An example of a test command would be:
         *
         *      <process_name> --version
         */
        bool isCommandAvailable(const QString& testCommand) const;

        /*
         * Executes the given terminal command to get a version number for the
         * application.  An example of a command would be:
         *
         *      <process_name> --version
         *
         * This method will return a list of integers representing the major,
         * minor, etc., version numbers, in the order printed to stdout.
         */
        QList<int> extractVersionNumber(const QString& command) const;

        /*
         * Convenience method to create a Pandoc exporter with the given name
         * and "from" command line argument.  Since Pandoc supports multiple
         * Markdown flavors, we want to be able to create an exporter for
         * each one (i.e., for Pandoc GitHub Flavored Markdown, Pandoc Strict,
         * Pandoc CommonMark, etc.).  The inputFormat parameter specifies
         * the argument to be passed to the -f option--for example,
         * markdown, markdown_mmd, etc.
         */
        void addPandocExporter
        (
            const QString& name,
            const QString& inputFormat,
            int majorVersion,
            int minorVersion
        );


};

#endif // EXPORTERFACTORY_H
