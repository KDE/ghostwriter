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
#include <QFileInfo>
#include <QObject>

#include "CommandLineExporter.h"
#include <stdio.h>


CommandLineExporter::CommandLineExporter(const QString& name)
    : Exporter(name),
      htmlExportCommand(QString()),
      fileExportCommand(QString()),
      defaultOutputFileExtension(QString())
{
    ;
}

CommandLineExporter::~CommandLineExporter()
{
    ;
}

void CommandLineExporter::setHtmlExportCommand(const QString& command)
{
    htmlExportCommand = command;
}

void CommandLineExporter::setFileExportCommand(const QString& command)
{
    fileExportCommand = command;
}

void CommandLineExporter::setDefaultOutputFileExtension(const QString& fileExtension)
{
    defaultOutputFileExtension = fileExtension;
}

void CommandLineExporter::exportToHtml(const QString& text, QString& html)
{
    QString stderrOuptut;

    if (!executeCommand(htmlExportCommand, text, QString(), html, stderrOuptut))
    {
        html = QString("<center><b style='color: red'>") + QObject::tr("Export failed: ") + QString("%1</b></center>)").arg(htmlExportCommand);
    }
    else if (!stderrOuptut.isNull() && !stderrOuptut.isEmpty())
    {
        html = QString("<center><b style='color: red'>") + QObject::tr("Export failed: ") + QString("%1</b></center>").arg(stderrOuptut);
    }
}

void CommandLineExporter::exportToFile
(
    const QString& text,
    const QString& outputFilePath,
    QString& err
)
{
    QString stdoutOutput;
    QString stderrOuptut;

    QString filePath = outputFilePath;

    if (!defaultOutputFileExtension.isNull() && !defaultOutputFileExtension.isEmpty())
    {
        QFileInfo fileInfo(outputFilePath);

        if (0 != fileInfo.suffix().compare(defaultOutputFileExtension, Qt::CaseInsensitive))
        {
            filePath += QString(".") + defaultOutputFileExtension;
        }
    }

    if (!executeCommand(fileExportCommand, text, filePath, stdoutOutput, stderrOuptut))
    {
        err = QObject::tr("Failed to execute command: ") + QString("%1").arg(fileExportCommand);
    }
    else if (!stderrOuptut.isNull() && !stderrOuptut.isEmpty())
    {
        err = stderrOuptut;
    }
    else
    {
        err = QString();
    }
}

bool CommandLineExporter::executeCommand
(
    const QString& command,
    const QString& textInput,
    const QString& outputFilePath,
    QString& stdoutOutput,
    QString& stderrOutput
)
{
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);

    QString expandedCommand = command + QString(" ");

    if (!outputFilePath.isNull() && !outputFilePath.isEmpty())
    {
        expandedCommand.replace(QString("${OUTPUT_FILE_PATH}"), outputFilePath);
    }

    process.start(expandedCommand);

    if (!process.waitForStarted())
    {
        return false;
    }
    else
    {
        if (!textInput.isNull() && !textInput.isEmpty())
        {
            process.write(textInput.toUtf8());
            process.closeWriteChannel();
        }

        if (!process.waitForFinished())
        {
            return false;
        }
        else
        {
            stdoutOutput = QString::fromUtf8(process.readAllStandardOutput().data());
            stderrOutput = QString::fromUtf8(process.readAllStandardError().data());
        }
    }

    return true;
}
