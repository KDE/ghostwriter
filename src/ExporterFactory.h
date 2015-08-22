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

#ifndef EXPORTERFACTORY_H
#define EXPORTERFACTORY_H

#include <QList>

#include "Exporter.h"

class ExporterFactory
{
    public:
        static ExporterFactory* getInstance();

        ~ExporterFactory();

        QList<Exporter*> getFileExporters();
        QList<Exporter*> getHtmlExporters();

    private:
        static ExporterFactory* instance;
        QList<Exporter*> fileExporters;
        QList<Exporter*> htmlExporters;

        ExporterFactory();

        // testCommand is a simplified command to see if executable is
        // installed and available. An example of a test command would be:
        //    <process_name> --version
        //
        bool isCommandAvailable(const QString& testCommand) const;
};

#endif // EXPORTERFACTORY_H
