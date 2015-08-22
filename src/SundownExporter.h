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

class SundownExporter : public Exporter
{
    public:
        SundownExporter
        (
            const QString& options,
            const QString& fileFormatFilter
        );

        ~SundownExporter();

        void exportToHtml(const QString& text, QString& html);
        void exportToFile
        (
            const QString& text,
            const QString& outputFilePath,
            QString& err
        );

    private:
        const QString options;
};

#endif // SUNDOWNEXPORTER_H
