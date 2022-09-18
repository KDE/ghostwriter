/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
