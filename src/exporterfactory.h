/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#include "exporter.h"

namespace ghostwriter
{
/**
 * Creates Exporters for use with HTML live preview and exporting to disk.
 */
class ExporterFactoryPrivate;
class ExporterFactory
{
    Q_DECLARE_PRIVATE(ExporterFactory)

public:
    /**
     * Gets the singleton instance of this class.
     */
    static ExporterFactory *instance();

    /**
     * Destructor.
     */
    ~ExporterFactory();

    /**
     * Gets a list of Exporters that support exporting to a file.
     */
    QList<Exporter *> fileExporters();

    /**
     * Gets a list of Exporters that support exporting to HTML for
     * HTML live preview rendering.
     */
    QList<Exporter *> htmlExporters();

    /**
     * Returns the exporter with the given name, or nullptr if one
     * with that name does not exist.
     */
    Exporter *exporterByName(const QString &name);

private:
    QScopedPointer<ExporterFactoryPrivate> d_ptr;

    /*
    * Constructor.
    */
    ExporterFactory();
};
} // namespace ghostwriter

#endif // EXPORTERFACTORY_H
