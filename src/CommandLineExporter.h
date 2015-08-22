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

#ifndef COMMAND_LINE_EXPORTER_H
#define COMMAND_LINE_EXPORTER_H

#include <QObject>
#include <QString>

#include "Exporter.h"

class CommandLineExporter : public Exporter
{
    public:

        CommandLineExporter(const QString& name);
        virtual ~CommandLineExporter();

        void setHtmlExportCommand(const QString& command);
        void setFileExportCommand(const QString& command);
        void setDefaultOutputFileExtension(const QString& fileExtension);

        virtual void exportToHtml(const QString& text, QString& html);
        virtual void exportToFile
        (
            const QString& text,
            const QString& outputFilePath,
            QString& err
        );

    protected:
        QString htmlExportCommand;
        QString fileExportCommand;
        QString defaultOutputFileExtension;

    private:
        bool executeCommand
        (
            const QString& command,
            const QString& textInput,
            const QString& outputFilePath,
            QString& stdoutOutput,
            QString& stderrOutput
        );


};

#endif // COMMAND_LINE_EXPORTER_H
