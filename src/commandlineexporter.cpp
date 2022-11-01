/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QProcess>
#include <QFileInfo>
#include <QObject>
#include <QDir>

#include "commandlineexporter.h"

namespace ghostwriter
{
class CommandLineExporterPrivate
{
public:
    CommandLineExporterPrivate()
    = default;

    ~CommandLineExporterPrivate()
    = default;

    QMap<const ExportFormat *, QString> formatToCommandMap;
    QString smartTypographyOnArgument = "";
    QString smartTypographyOffArgument = "";
    QString htmlRenderCommand = QString();

    bool executeCommand
    (
        const QString &command,
        const QString &inputFilePath,
        const QString &textInput,
        const QString &outputFilePath,
        const QString &options,
        bool smartTypographyEnabled,
        QString &stdoutOutput,
        QString &stderrOutput
    ) const;
};

const QString CommandLineExporter::OUTPUT_FILE_PATH_VAR = QString("${OUTPUT_FILE_PATH}");
const QString CommandLineExporter::SMART_TYPOGRAPHY_ARG = QString("${SMART_TYPOGRAPHY_ARG}");


CommandLineExporter::CommandLineExporter(const QString &name, const QString &options)
    : Exporter(name, options),
      d_ptr(new CommandLineExporterPrivate()){}

CommandLineExporter::~CommandLineExporter() = default;

void CommandLineExporter::setHtmlRenderCommand(const QString &command)
{
    Q_D(CommandLineExporter);
    
    d->htmlRenderCommand = command;
}

void CommandLineExporter::addFileExportCommand
(
    const ExportFormat *format,
    const QString &command
)
{
    Q_D(CommandLineExporter);
    
    d->formatToCommandMap.insert(format, command);
    this->m_supportedFormats.append(format);
}

QString CommandLineExporter::smartTypographyOnArgument() const
{
    Q_D(const CommandLineExporter);
    
    return d->smartTypographyOnArgument;
}

void CommandLineExporter::setSmartTypographyOnArgument(const QString &argument)
{
    Q_D(CommandLineExporter);
    
    d->smartTypographyOnArgument = argument;
}

QString CommandLineExporter::smartTypographyOffArgument() const
{
    Q_D(const CommandLineExporter);
    
    return d->smartTypographyOffArgument;
}

void CommandLineExporter::setSmartTypographyOffArgument(const QString &argument)
{
    Q_D(CommandLineExporter);
    
    d->smartTypographyOffArgument = argument;
}

void CommandLineExporter::setMathSupported(bool supported)
{
    m_mathSupported = supported;
}

void CommandLineExporter::exportToHtml(const QString &text, QString &html)
{
    Q_D(CommandLineExporter);
    
    QString stderrOutput;

    if (d->htmlRenderCommand.isNull() || d->htmlRenderCommand.isEmpty()) {
        html = "<center><b style='color: red'>HTML is not supported for this processor.</b></center>";
        return;
    }

    if
    (
        ! d->executeCommand
        (
            d->htmlRenderCommand,
            QString(),
            text,
            QString(),
            m_options,
            this->m_smartTypographyEnabled,
            html,
            stderrOutput
        )
    ) {
        QString errorMessage = d->htmlRenderCommand;

        if (!stderrOutput.isNull() && !stderrOutput.isEmpty()) {
            errorMessage = stderrOutput;
        }

        html = QString("<center><b style='color: red'>") 
            + QObject::tr("Export failed: ")
            + QString("%1</b></center>").arg(errorMessage);
    }
}

void CommandLineExporter::exportToFile
(
    const ExportFormat *format,
    const QString &inputFilePath,
    const QString &text,
    const QString &outputFilePath,
    QString &err
)
{
    Q_D(CommandLineExporter);
    
    QString stdoutOutput;
    QString stderrOutput;

    if (!d->formatToCommandMap.contains(format)) {
        err = QObject::tr("%1 format is not supported by this processor.")
            .arg(format->name());
        return;
    }

    QString command = d->formatToCommandMap.value(format);

    if
    (
        ! d->executeCommand
        (
            command,
            inputFilePath,
            text,
            outputFilePath,
            m_options,
            this->m_smartTypographyEnabled,
            stdoutOutput,
            stderrOutput
        )
    ) {
        if (!stderrOutput.isNull() && !stderrOutput.isEmpty()) {
            err = stderrOutput;
        } else {
            err = QObject::tr("Failed to execute command: ")
                + QString("%1").arg(command);
        }
    } else {
        err = QString();
    }
}

bool CommandLineExporterPrivate::executeCommand
(
    const QString &command,
    const QString &inputFilePath,
    const QString &textInput,
    const QString &outputFilePath,
    const QString &options,
    const bool smartTypographyEnabled,
    QString &stdoutOutput,
    QString &stderrOutput
) const
{
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);

    QString expandedCommand = command + " ";

    if (!outputFilePath.isNull() && !outputFilePath.isEmpty()) {
        // Redirect stdout to the output file path if the path variable wasn't
        // set in the command string.
        //
        if (!expandedCommand.contains(
                CommandLineExporter::OUTPUT_FILE_PATH_VAR)) {
            process.setStandardOutputFile(outputFilePath);
        } else {
            // Surround file path with quotes in case there are spaces in the
            // path.
            //
            QString outputFilePathWithQuotes = QString('\"') +
                                               outputFilePath + '\"';
            expandedCommand.replace(
                CommandLineExporter::OUTPUT_FILE_PATH_VAR,
                outputFilePathWithQuotes);
        }
    }

    if (smartTypographyEnabled && !smartTypographyOnArgument.isNull()) {
        expandedCommand.replace
        (
            CommandLineExporter::SMART_TYPOGRAPHY_ARG,
            smartTypographyOnArgument
        );
    } else if (!smartTypographyEnabled
            && !smartTypographyOffArgument.isNull()) {
        expandedCommand.replace
        (
            CommandLineExporter::SMART_TYPOGRAPHY_ARG,
            smartTypographyOffArgument
        );
    } else {
        // Replace the smart typography argument with an empty string
        // in case the above two cases are not applicable.
        //
        expandedCommand.replace
        (
            CommandLineExporter::SMART_TYPOGRAPHY_ARG,
            ""
        );
    }

    if (!inputFilePath.isNull() && !inputFilePath.isEmpty()) {
        process.setWorkingDirectory(QFileInfo(inputFilePath).dir().path());
    }
    expandedCommand = expandedCommand + " " + options;


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    process.start(expandedCommand);
#else
    process.startCommand(expandedCommand);
#endif

    if (!process.waitForStarted()) {
        return false;
    } else {
        if (!textInput.isNull() && !textInput.isEmpty()) {
            process.write(textInput.toUtf8());
            process.closeWriteChannel();
        }

        if (!process.waitForFinished()) {
            return false;
        } else {
            stdoutOutput = QString::fromUtf8(
                process.readAllStandardOutput().data());
            stderrOutput = QString::fromUtf8(
                process.readAllStandardError().data());

            if ((QProcess::NormalExit != process.exitStatus())
                    || (0 != process.exitCode())) {
                return false;
            }
        }
    }

    return true;
}
}
