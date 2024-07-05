/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QVersionNumber> 

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
    *
    * Returns the version number if the command is available, or else
    * a null version number.
    */
    QVersionNumber isCommandAvailable(const QString &command,
        const QStringList &args) const;

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
    Q_D(ExporterFactory);
    
    return d->fileExporters;
}

QList<Exporter *> ExporterFactory::htmlExporters()
{
    Q_D(ExporterFactory);
    
    return d->htmlExporters;
}

Exporter *ExporterFactory::exporterByName(const QString &name)
{
    Q_D(ExporterFactory);
    
    // Search in HTML exporter list first.
    for (Exporter *exporter : d->htmlExporters) {
        if (exporter->name() == name) {
            // Found a match!
            return exporter;
        }
    }

    // If HTML exporter list does not contain an exporter
    // with the desired name, search in the file exporter
    // list next.
    //
    for (Exporter *exporter : d->fileExporters) {
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
    Q_D(ExporterFactory);
    
    CommandLineExporter *exporter = nullptr;
    QVersionNumber pandocVersion = d->isCommandAvailable("pandoc", QStringList("--version"));
    QVersionNumber mmdVersion = d->isCommandAvailable("multimarkdown", QStringList("--version"));
    QVersionNumber cmarkVersion = d->isCommandAvailable("cmark", QStringList("--version"));

    CmarkGfmExporter *cmarkGfmExporter = new CmarkGfmExporter();
    d->fileExporters.append(cmarkGfmExporter);
    d->htmlExporters.append(cmarkGfmExporter);

    if (!pandocVersion.isNull()) {
        int majorVersion = pandocVersion.majorVersion();
        int minorVersion = pandocVersion.minorVersion();

        // Check version of Pandoc. Drop support for version 1.
        if (majorVersion >= 2) {
            d->addPandocExporter("Pandoc", "markdown",  majorVersion, minorVersion);

            if ((majorVersion > 1) ||
                ((1 == majorVersion) && (minorVersion >= 14))) {
                d->addPandocExporter("Pandoc CommonMark", "commonmark",  majorVersion, minorVersion);
            }

            d->addPandocExporter("Pandoc GitHub-flavored Markdown", "markdown_github-hard_line_breaks",  majorVersion, minorVersion);
            d->addPandocExporter("Pandoc PHP Markdown Extra", "markdown_phpextra",  majorVersion, minorVersion);
            d->addPandocExporter("Pandoc MultiMarkdown", "markdown_mmd", majorVersion, minorVersion);
            d->addPandocExporter("Pandoc Strict", "markdown_strict",  majorVersion, minorVersion);
        }
        else {
            qWarning() << "Version" << pandocVersion << "of pandoc is unsupported.";
        }
    }

    if (!mmdVersion.isNull()) {
        int majorVersion = mmdVersion.majorVersion();

        exporter = new CommandLineExporter("MultiMarkdown");

        // Smart typography option (--smart) is only available in version 5 and below.
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
        d->fileExporters.append(exporter);
        d->htmlExporters.append(exporter);
    }

    if (!cmarkVersion.isNull()) {
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
        d->fileExporters.append(exporter);
        d->htmlExporters.append(exporter);
    }
}

QVersionNumber ExporterFactoryPrivate::isCommandAvailable(const QString &command,
    const QStringList &args) const
{
    QProcess process;
    process.start(command, args);

    if (!process.waitForStarted(5000)) {
        qWarning() << "Command" << command << "is not available.";
        return QVersionNumber();
    } else if (!process.waitForFinished(5000)) {
        qCritical() << command << "process timed out and cannot be used.";
        process.kill();
        return QVersionNumber();
    }

    // Extract the version number
    QString versionStr = QString::fromUtf8(process.readAllStandardOutput().data());
    const QRegularExpression versionRegex1("v(\\d+(\\.\\d+)*)");
    const QRegularExpression versionRegex2("(\\d+(\\.\\d+)*)");
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

    QVersionNumber version = QVersionNumber::fromString(versionStr);
    qInfo().noquote() << "Using" << command << "version" << version.toString();

    return version;
}

void ExporterFactoryPrivate::addPandocExporter
(
    const QString &name,
    const QString &inputFormat,
    int majorVersion,
    int minorVersion
)
{
    Q_UNUSED(majorVersion)
    Q_UNUSED(minorVersion)

    CommandLineExporter *exporter = new CommandLineExporter(name);

    exporter->setSmartTypographyOnArgument("+smart");
    exporter->setSmartTypographyOffArgument("-smart");
    exporter->setMathSupported(true);

    exporter->setHtmlRenderCommand
    (
        QString("pandoc -f ") +
        inputFormat +
        CommandLineExporter::SMART_TYPOGRAPHY_ARG +
        " -t html --mathjax"
    );

    QString standardExportStr =
        QString("pandoc -f ") +
        inputFormat +
        CommandLineExporter::SMART_TYPOGRAPHY_ARG +
        " -t %1 --standalone --quiet -o " +
        CommandLineExporter::OUTPUT_FILE_PATH_VAR;
    

    exporter->addFileExportCommand
    (
        ExportFormat::HTML,
        standardExportStr.arg("html") + " --mathjax"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::HTML5,
        standardExportStr.arg("html5") + " --mathjax"
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
        " -Vlinkcolor=blue -Vcitecolor=blue -Vurlcolor=blue -Vtoccolor=blue -Vmargin-left=1in -Vmargin-right=1in -Vmargin-top=1in -Vmargin-bottom=1in "
    );
    exporter->addFileExportCommand
    (
        ExportFormat::PDF_CONTEXT,
        standardExportStr.arg("context") +
        " --variable pagenumbering:location=footer --variable layout:header=0mm --variable layout:top=1in --variable layout:bottom=1in --variable layout:leftmargin=1in --variable layout:rightmargin=1in -Vlinkcolor=blue"
    );

    // Note: Do not use --mathjax option with WKHTMLTOPDF export, as this will
    // cause pandoc to hang.  KaTex is a good substitute.
    //
    exporter->addFileExportCommand
    (
        ExportFormat::PDF_WKHTML,
        standardExportStr.arg("html5") +
        " --katex -Vmargin-left=1in -Vmargin-right=1in -Vmargin-top=1in -Vmargin-bottom=1in"
    );

    exporter->addFileExportCommand
    (
        ExportFormat::EPUBV2,
        standardExportStr.arg("epub") +
        " --mathml --toc --toc-depth 6"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::EPUBV3,
        standardExportStr.arg("epub3") +
        " --mathml --toc --toc-depth 6"
    );
    exporter->addFileExportCommand
    (
        ExportFormat::FICTIONBOOK2,
        standardExportStr.arg("fb2") +
        " --mathml --toc --toc-depth 6"
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
