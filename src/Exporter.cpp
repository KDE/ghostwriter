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

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QObject>

#include "Exporter.h"

#include <stdio.h>


Exporter::Exporter(const QString& name)
    : smartTypographyEnabled(false), name(name)
{
    ;
}

Exporter::~Exporter()
{
    ;
}

QString Exporter::getName() const
{
    return name;
}

const QList<const ExportFormat*> Exporter::getSupportedFormats() const
{
    return supportedFormats;
}

bool Exporter::getSmartTypographyEnabled() const
{
    return smartTypographyEnabled;
}

void Exporter::setSmartTypographyEnabled(bool enabled)
{
    smartTypographyEnabled = enabled;
}

void Exporter::exportToHtml(const QString& text, QString& html)
{
    Q_UNUSED(text)

    html = QString("<center><b style='color: red'>") +
        QObject::tr("Export to HTML is not supported with this processor.") +
         QString("</b></center>)");
}

