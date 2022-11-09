/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QObject>

#include "exporter.h"

#include <stdio.h>


namespace ghostwriter
{
Exporter::Exporter(const QString &name)
    : m_smartTypographyEnabled(false), 
      m_mathSupported(false),
      m_name(name){}

Exporter::~Exporter() = default;

QString Exporter::name() const
{
    return m_name;
}

QString Exporter::options() const
{
    return m_options;
}

void Exporter::setOptions(const QString &options)
{
    m_options = options;
}

QList<const ExportFormat *> Exporter::supportedFormats() const
{
    return m_supportedFormats;
}

bool Exporter::smartTypographyEnabled() const
{
    return m_smartTypographyEnabled;
}

void Exporter::setSmartTypographyEnabled(bool enabled)
{
    m_smartTypographyEnabled = enabled;
}

bool Exporter::supportsMath() const
{
    return m_mathSupported;
}

void Exporter::exportToHtml(const QString &text, QString &html)
{
    Q_UNUSED(text)

    html = QString("<center><b style='color: red'>") +
           QObject::tr("Export to HTML is not supported with this processor.") +
           QString("</b></center>)");
}
} // namespace ghostwriter

