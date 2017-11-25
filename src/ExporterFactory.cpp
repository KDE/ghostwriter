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

#include <QProcess>
#include <QRegExp>
#include <QSettings>

#include "ExporterFactory.h"
#include "SundownExporter.h"
#include "CommandLineExporter.h"

ExporterFactory* ExporterFactory::instance = NULL;

ExporterFactory::~ExporterFactory()
{
    // No need to delete Exporter instances, since this is a singleton class,
    // and all will be destroyed upon quitting the application.
    ;
}

ExporterFactory* ExporterFactory::getInstance()
{
    if (NULL == instance)
    {
        instance = new ExporterFactory();
    }

    return instance;
}

QList<Exporter*> ExporterFactory::getFileExporters()
{
    return fileExporters;
}

QList<Exporter*> ExporterFactory::getHtmlExporters()
{
    return htmlExporters;
}

Exporter* ExporterFactory::getExporterByName(const QString& name)
{
    // Search in HTML exporter list first.
    foreach (Exporter* exporter, htmlExporters)
    {
        if (exporter->getName() == name)
        {
            // Found a match!
            return exporter;
        }
    }

    // If HTML exporter list does not contain an exporter
    // with the desired name, search in the file exporter
    // list next.
    //
    foreach (Exporter* exporter, fileExporters)
    {
        if (exporter->getName() == name)
        {
            // Found a match!
            return exporter;
        }
    }

    // No match found.
    return NULL;
}

ExporterFactory::ExporterFactory()
{
    CommandLineExporter* exporter = NULL;
    bool pandocIsAvailable = isCommandAvailable("pandoc --version");
    bool mmdIsAvailable = isCommandAvailable("multimarkdown --version");
    bool discountIsAvailable = isCommandAvailable("markdown -V");
    bool cmarkIsAvailable = isCommandAvailable("cmark --version");
    bool cmarkGfmIsAvailable = isCommandAvailable("cmark-gfm --version");

    SundownExporter* sundownExporter = new SundownExporter();
    fileExporters.append(sundownExporter);
    htmlExporters.append(sundownExporter);

    if (pandocIsAvailable)
    {
        int majorVersion = 0;
        int minorVersion = 0;

        // Check whether version of Pandoc can read CommonMark.
        QList<int> versionNumber = extractVersionNumber("pandoc --version");

        if (versionNumber.length() > 0)
        {
            majorVersion = versionNumber[0];

            if (versionNumber.length() > 1)
            {
                minorVersion = versionNumber[1];
            }
        }

        addPandocExporter("Pandoc", "markdown", majorVersion, minorVersion);

        if
        (
            (majorVersion > 1) ||
            ((1 == majorVersion) && (minorVersion >= 14))
        )
        {
            addPandocExporter("Pandoc CommonMark", "commonmark", majorVersion, minorVersion);
        }

        addPandocExporter("Pandoc GitHub-flavored Markdown", "markdown_github", majorVersion, minorVersion);
        addPandocExporter("Pandoc PHP Markdown Extra", "markdown_phpextra", majorVersion, minorVersion);
        addPandocExporter("Pandoc MultiMarkdown", "markdown_mmd", majorVersion, minorVersion);
        addPandocExporter("Pandoc Strict", "markdown_strict", majorVersion, minorVersion);
    }

    if (mmdIsAvailable)
    {
        int majorVersion = 0;

        QList<int> versionNumber = extractVersionNumber("multimarkdown --version");

        if (versionNumber.length() > 0)
        {
            majorVersion = versionNumber[0];
        }

        exporter = new CommandLineExporter("MultiMarkdown");

        // Smart typography option (--smart) is only availabe in version 5 and below.
        // The option is was removed and enabled by default in version 6 and above.
        //
        if (majorVersion < 6)
        {
            exporter->setSmartTypographyOnArgument("--smart");
        }

        exporter->setSmartTypographyOffArgument("--nosmart");
        exporter->setHtmlRenderCommand(QString("multimarkdown %1 -t html")
            .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG));
        exporter->addFileExportCommand
        (
            ExportFormat::HTML,
            QString("multimarkdown %1 -t html -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
        );
        exporter->addFileExportCommand
        (
            ExportFormat::ODF,
            QString("multimarkdown %1 -t odf -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
        );
        exporter->addFileExportCommand
        (
            ExportFormat::LATEX,
            QString("multimarkdown %1 -t latex -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
        );
        exporter->addFileExportCommand
        (
            ExportFormat::MEMOIR,
            QString("multimarkdown %1 -t memoir -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
        );
        exporter->addFileExportCommand
        (
            ExportFormat::LYX,
            QString("multimarkdown %1 -t lyx -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
        );
        fileExporters.append(exporter);
        htmlExporters.append(exporter);
    }

    if (discountIsAvailable)
    {
        exporter = new CommandLineExporter("Discount");
        exporter->setSmartTypographyOnArgument("-f smarty,strikethrough,footnote");
        exporter->setSmartTypographyOffArgument("-f nosmarty,strikethrough,footnote");
        exporter->setHtmlRenderCommand(QString("markdown %1")
            .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG));
        exporter->addFileExportCommand
        (
            ExportFormat::HTML,
            QString("markdown %1 -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
        );
        fileExporters.append(exporter);
        htmlExporters.append(exporter);
    }

    if (cmarkIsAvailable)
    {
        exporter = new CommandLineExporter("cmark");
        exporter->setSmartTypographyOnArgument("--smart");
        exporter->setHtmlRenderCommand(QString("cmark -t html --smart %1")
            .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG));
        exporter->addFileExportCommand
        (
            ExportFormat::HTML,
            QString("cmark -t html %1")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
        );
        exporter->addFileExportCommand
        (
            ExportFormat::LATEX,
            QString("cmark -t latex %1")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
        );
        exporter->addFileExportCommand
        (
            ExportFormat::MANPAGE,
            QString("cmark -t man %1")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
        );
        fileExporters.append(exporter);
        htmlExporters.append(exporter);
    }

    if (cmarkGfmIsAvailable)
    {
        QString commandStr = "cmark-gfm -t %1 %2"
            " -e table -e strikethrough -e autolink -e tagfilter";
        exporter = new CommandLineExporter("cmark-gfm");
        exporter->setSmartTypographyOnArgument("--smart");
        exporter->setHtmlRenderCommand(commandStr
            .arg("html")
            .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG));
        exporter->addFileExportCommand
        (
            ExportFormat::HTML,
            commandStr
                .arg("html")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
        );
        exporter->addFileExportCommand
        (
            ExportFormat::LATEX,
            commandStr
                .arg("latex")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
        );
        exporter->addFileExportCommand
        (
            ExportFormat::MANPAGE,
            commandStr
                .arg("man")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
        );
        fileExporters.append(exporter);
        htmlExporters.append(exporter);
    }
}

QList<int> ExporterFactory::extractVersionNumber(const QString& command) const
{
    QList<int> versionNumber;
    QProcess process;
    process.start(command);

    if (!process.waitForStarted(500))
    {
        return versionNumber;
    }
    else if (!process.waitForFinished(500))
    {
        process.kill();
        return versionNumber;
    }

    QString versionStr = QString::fromUtf8(process.readAllStandardOutput().data());
    QRegExp versionRegex("\\d+(\\.\\d+)*");
    int pos = versionRegex.indexIn(versionStr);

    if (pos >= 0)
    {
        versionStr = versionStr.mid(pos, versionRegex.matchedLength());
    }

    QStringList numbers = versionStr.split('.', QString::SkipEmptyParts);

    foreach (QString num, numbers)
    {
        versionNumber.append(num.toInt());
    }

    return versionNumber;
}

bool ExporterFactory::isCommandAvailable(const QString& testCommand) const
{
    QProcess process;
    process.start(testCommand);

    if (!process.waitForStarted(500))
    {
        return false;
    }
    else if (!process.waitForFinished(500))
    {
        process.kill();
    }

    return true;
}

void ExporterFactory::addPandocExporter
(
    const QString& name,
    const QString& inputFormat,
    int majorVersion,
    int minorVersion
)
{
    Q_UNUSED(minorVersion)

    CommandLineExporter* exporter = new CommandLineExporter(name);

    if (majorVersion >= 2)
    {
        exporter->setSmartTypographyOnArgument("+smart");
        exporter->setSmartTypographyOffArgument("-smart");
    }
    else
    {
        exporter->setSmartTypographyOnArgument(" --smart");
    }

    exporter->setHtmlRenderCommand
    (
        QString("pandoc -f ") +
            inputFormat +
            CommandLineExporter::SMART_TYPOGRAPHY_ARG +
            " -t html"
    );

    QString standardExportStr =
        QString("pandoc -f ") +
        inputFormat +
        CommandLineExporter::SMART_TYPOGRAPHY_ARG +
        " -t %1 --standalone -o " +
        CommandLineExporter::OUTPUT_FILE_PATH_VAR;

    exporter->addFileExportCommand
    (
        ExportFormat::HTML,
        standardExportStr.arg("html")
    );
    exporter->addFileExportCommand
    (
        ExportFormat::HTML5,
        standardExportStr.arg("html5")
    );
    exporter->addFileExportCommand
    (
        ExportFormat::ODT,
        standardExportStr.arg("odt")
    );
    exporter->addFileExportCommand
    (
        ExportFormat::ODF,
        standardExportStr.arg("opendocument")
    );
    exporter->addFileExportCommand
    (
        ExportFormat::RTF,
        standardExportStr.arg("rtf")
    );
    exporter->addFileExportCommand
    (
        ExportFormat::DOCX,
        standardExportStr.arg("docx")
    );
    exporter->addFileExportCommand
    (
        ExportFormat::PDF_LATEX,
        standardExportStr.arg("latex") +
            " -Vlinkcolor=blue -Vcitecolor=blue -Vurlcolor=blue -Vtoccolor=blue -Vmargin-left=1in -Vmargin-right=1in -Vmargin-top=1in -Vmargin-bottom=1in"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::PDF_CONTEXT,
        standardExportStr.arg("context") +
            " --variable pagenumbering:location=footer --variable layout:header=0mm --variable layout:top=1in --variable layout:bottom=1in --variable layout:leftmargin=1in --variable layout:rightmargin=1in -Vlinkcolor=blue"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::PDF_WKHTML,
        standardExportStr.arg("html5") +
            " -Vmargin-left=1in -Vmargin-right=1in -Vmargin-top=1in -Vmargin-bottom=1in --mathjax"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::EPUBV2,
        standardExportStr.arg("epub") +
            " --toc --toc-depth 6"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::EPUBV3,
        standardExportStr.arg("epub3") +
            " --toc --toc-depth 6"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::FICTIONBOOK2,
        standardExportStr.arg("fb2") +
            " --toc --toc-depth 6"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::LATEX,
        standardExportStr.arg("latex")
    );
    exporter->addFileExportCommand
    (
        ExportFormat::GROFFMAN,
        standardExportStr.arg("man")
    );
    fileExporters.append(exporter);
    htmlExporters.append(exporter);
}
