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

#include <QProcess>
#include <QRegularExpression>
#include <QSettings>

#include "exporterfactory.h"
#include "cmarkgfmexporter.h"
#include "commandlineexporter.h"

namespace ghostwriter
{

class ExporterFactoryPrivate
{
public:
    ExporterFactoryPrivate()
    {
        ;
    }

    ~ExporterFactoryPrivate()
    {
        ;
    }

    static ExporterFactory *instance;
    QList<Exporter *> fileExporters;
    QList<Exporter *> htmlExporters;

    /*
    * Executes the given terminal command to see if the executable is
    * installed and available.  An example of a test command would be:
    *
    *      <process_name> --version
    */
    bool isCommandAvailable(const QString &testCommand) const;

    /*
    * Executes the given terminal command to get a version number for the
    * application.  An example of a command would be:
    *
    *      <process_name> --version
    *
    * This method will return a list of integers representing the major,
    * minor, etc., version numbers, in the order printed to stdout.
    */
    QList<int> extractVersionNumber(const QString &command) const;

    /*
    * Convenience method to create a Pandoc exporter with the given name
    * and "from" command line argument.  Since Pandoc supports multiple
    * Markdown flavors, we want to be able to create an exporter for
    * each one (i.e., for Pandoc GitHub Flavored Markdown, Pandoc Strict,
    * Pandoc CommonMark, etc.).  The inputFormat parameter specifies
    * the argument to be passed to the -f option--for example,
    * markdown, markdown_mmd, etc.
    */
    void addPandocExporter
    (
        const QString &name,
        const QString &inputFormat,
        int majorVersion,
        int minorVersion
    );
};

ExporterFactory *ExporterFactoryPrivate::instance = nullptr;

ExporterFactory::~ExporterFactory()
{
    // No need to delete Exporter instances, since this is a singleton class,
    // and all will be destroyed upon quitting the application.
    ;
}

ExporterFactory *ExporterFactory::instance()
{
    if (nullptr == ExporterFactoryPrivate::instance) {
        ExporterFactoryPrivate::instance = new ExporterFactory();
    }

    return ExporterFactoryPrivate::instance;
}

QList<Exporter *> ExporterFactory::fileExporters()
{
    return d_func()->fileExporters;
}

QList<Exporter *> ExporterFactory::htmlExporters()
{
    return d_func()->htmlExporters;
}

Exporter *ExporterFactory::exporterByName(const QString &name)
{
    // Search in HTML exporter list first.
    foreach (Exporter *exporter, d_func()->htmlExporters) {
        if (exporter->name() == name) {
            // Found a match!
            return exporter;
        }
    }

    // If HTML exporter list does not contain an exporter
    // with the desired name, search in the file exporter
    // list next.
    //
    foreach (Exporter *exporter, d_func()->fileExporters) {
        if (exporter->name() == name) {
            // Found a match!
            return exporter;
        }
    }

    // No match found.
    return nullptr;
}

ExporterFactory::ExporterFactory()
    : d_ptr(new ExporterFactoryPrivate())
{
    CommandLineExporter *exporter = nullptr;
    bool pandocIsAvailable = d_func()->isCommandAvailable("pandoc --version");
    bool mmdIsAvailable = d_func()->isCommandAvailable("multimarkdown --version");
    bool cmarkIsAvailable = d_func()->isCommandAvailable("cmark --version");

    CmarkGfmExporter *cmarkGfmExporter = new CmarkGfmExporter();
    d_func()->fileExporters.append(cmarkGfmExporter);
    d_func()->htmlExporters.append(cmarkGfmExporter);

    if (pandocIsAvailable) {
        int majorVersion = 0;
        int minorVersion = 0;

        // Check whether version of Pandoc can read CommonMark.
        QList<int> versionNumber = d_func()->extractVersionNumber("pandoc --version");

        if (versionNumber.length() > 0) {
            majorVersion = versionNumber[0];

            if (versionNumber.length() > 1) {
                minorVersion = versionNumber[1];
            }
        }

        d_func()->addPandocExporter("Pandoc", "markdown", majorVersion, minorVersion);

        if
        (
            (majorVersion > 1) ||
            ((1 == majorVersion) && (minorVersion >= 14))
        ) {
            d_func()->addPandocExporter("Pandoc CommonMark", "commonmark", majorVersion, minorVersion);
        }

        d_func()->addPandocExporter("Pandoc GitHub-flavored Markdown", "markdown_github-hard_line_breaks", majorVersion, minorVersion);
        d_func()->addPandocExporter("Pandoc PHP Markdown Extra", "markdown_phpextra", majorVersion, minorVersion);
        d_func()->addPandocExporter("Pandoc MultiMarkdown", "markdown_mmd", majorVersion, minorVersion);
        d_func()->addPandocExporter("Pandoc Strict", "markdown_strict", majorVersion, minorVersion);
    }

    if (mmdIsAvailable) {
        int majorVersion = 0;

        QList<int> versionNumber = d_func()->extractVersionNumber("multimarkdown --version");

        if (versionNumber.length() > 0) {
            majorVersion = versionNumber[0];
        }

        exporter = new CommandLineExporter("MultiMarkdown");

        // Smart typography option (--smart) is only availabe in version 5 and below.
        // The option is was removed and enabled by default in version 6 and above.
        //
        if (majorVersion < 6) {
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

        // Version 6 removed ODF option and replaced it with ODT and FODT.
        if (majorVersion >= 6) {
            exporter->addFileExportCommand
            (
                ExportFormat::ODT,
                QString("multimarkdown %1 -t odt -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
            );

            exporter->addFileExportCommand
            (
                ExportFormat::ODF,
                QString("multimarkdown %1 -t fodt -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
            );
        } else {
            exporter->addFileExportCommand
            (
                ExportFormat::ODF,
                QString("multimarkdown %1 -t odf -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
            );
        }

        // Version 6 added EPUB 3
        if (majorVersion >= 6) {
            exporter->addFileExportCommand
            (
                ExportFormat::EPUBV3,
                QString("multimarkdown %1 -b -t epub -o %2")
                .arg(CommandLineExporter::SMART_TYPOGRAPHY_ARG)
                .arg(CommandLineExporter::OUTPUT_FILE_PATH_VAR)
            );
        }

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
        d_func()->fileExporters.append(exporter);
        d_func()->htmlExporters.append(exporter);
    }

    if (cmarkIsAvailable) {
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
        d_func()->fileExporters.append(exporter);
        d_func()->htmlExporters.append(exporter);
    }
}

QList<int> ExporterFactoryPrivate::extractVersionNumber(const QString &command) const
{
    QList<int> versionNumber;
    QProcess process;
    process.start(command);

    if (!process.waitForStarted(500)) {
        return versionNumber;
    } else if (!process.waitForFinished(500)) {
        process.kill();
        return versionNumber;
    }

    QString versionStr = QString::fromUtf8(process.readAllStandardOutput().data());
    QRegularExpression versionRegex1("v(\\d+(\\.\\d+)*)");
    QRegularExpression versionRegex2("(\\d+(\\.\\d+)*)");
    QRegularExpressionMatch match;

    // Prefer searching for "v1.6.3" version format over "1.6.3" format
    int pos = versionStr.indexOf(versionRegex1, 0, &match);

    if ((pos >= 0) && match.hasMatch()) {
        versionStr = match.captured(1);
    }
    // Else try the "1.6.3" format without the "v" in front
    else {
        pos = versionStr.indexOf(versionRegex2, 0, &match);

        if ((pos >= 0) && match.hasMatch()) {
            versionStr = match.captured(1);
        }
    }

    QStringList numbers = versionStr.split('.', QString::SkipEmptyParts);

    foreach (QString num, numbers) {
        versionNumber.append(num.toInt());
    }

    return versionNumber;
}

bool ExporterFactoryPrivate::isCommandAvailable(const QString &testCommand) const
{
    QProcess process;
    process.start(testCommand);

    if (!process.waitForStarted(500)) {
        return false;
    } else if (!process.waitForFinished(500)) {
        process.kill();
    }

    return true;
}

void ExporterFactoryPrivate::addPandocExporter
(
    const QString &name,
    const QString &inputFormat,
    int majorVersion,
    int minorVersion
)
{
    Q_UNUSED(minorVersion)

    CommandLineExporter *exporter = new CommandLineExporter(name);

    if (majorVersion >= 2) {
        exporter->setSmartTypographyOnArgument("+smart");
        exporter->setSmartTypographyOffArgument("-smart");
    } else {
        exporter->setSmartTypographyOnArgument(" --smart");
    }

    exporter->setHtmlRenderCommand
    (
        QString("pandoc --mathml -f ") +
        inputFormat +
        CommandLineExporter::SMART_TYPOGRAPHY_ARG +
        " -t html"
    );

    QString standardExportStr;
    if (majorVersion >= 2) {
        standardExportStr =
            QString("pandoc -f ") +
            inputFormat +
            CommandLineExporter::SMART_TYPOGRAPHY_ARG +
            " -t %1 --standalone --mathml --quiet -o " +
            CommandLineExporter::OUTPUT_FILE_PATH_VAR;
    } else {
        // older Pandoc releases don't know the option '--quiet'
        standardExportStr =
            QString("pandoc -f ") +
            inputFormat +
            CommandLineExporter::SMART_TYPOGRAPHY_ARG +
            " -t %1 --standalone --mathml -o " +
            CommandLineExporter::OUTPUT_FILE_PATH_VAR;
    }

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

    // Note: Do not use --mathjax option with WKHTMLTOPDF export, as this will
    // cause pandoc to hang.
    //
    exporter->addFileExportCommand
    (
        ExportFormat::PDF_WKHTML,
        standardExportStr.arg("html5") +
        " -Vmargin-left=1in -Vmargin-right=1in -Vmargin-top=1in -Vmargin-bottom=1in"
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
} // namespace ghostwriter
