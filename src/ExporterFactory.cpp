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
#include <QObject>

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

ExporterFactory::ExporterFactory()
{
    CommandLineExporter* exporter = NULL;
    bool pandocIsAvailable = isCommandAvailable("pandoc --version");
    bool mmdIsAvailable = isCommandAvailable("multimarkdown --version");
    bool discountIsAvailable = isCommandAvailable("markdown -V");

    SundownExporter* sundownExporter = new SundownExporter(QString(), QObject::tr("Sundown to HTML") + QString(" (*.html *.htm *.xhtml)"));
    fileExporters.append(sundownExporter);
    htmlExporters.append(sundownExporter);

    if (pandocIsAvailable)
    {
        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to HTML") + QString(" (*.html *.htm *.xhtml)"));
        exporter->setHtmlExportCommand("pandoc --smart -f markdown -t html");
        exporter->setFileExportCommand("pandoc --smart -f markdown -t html --standalone -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
        htmlExporters.append(exporter);

        // Don't set HTML 5 as previewable, since the QTextEdit class can't display HTML 5.
        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to HTML 5") + QString(" (*.html *.htm *.xhtml)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t html5 -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc Github");
        exporter->setFileFormatFilter(QObject::tr("Github-flavored Markdown (Pandoc Extension) to HTML") + QString(" (*.html *.htm *.xhtml)"));
        exporter->setHtmlExportCommand("pandoc --smart -f markdown_github -t html");
        fileExporters.append(exporter);
        htmlExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc Strict");
        exporter->setFileFormatFilter(QObject::tr("Strict Markdown.pl (Pandoc Strict Extension) to HTML") + QString(" (*.html *.htm *.xhtml)"));
        exporter->setHtmlExportCommand("pandoc --smart -f markdown_strict -t html");
        exporter->setFileExportCommand("pandoc --smart -f markdown_strict -t html -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
        htmlExporters.append(exporter);
    }

    if (mmdIsAvailable)
    {
        exporter = new CommandLineExporter("MultiMarkdown");
        exporter->setFileFormatFilter(QObject::tr("MultiMarkdown to HTML") + QString(" (*.html *.htm)"));
        exporter->setHtmlExportCommand("multimarkdown -t html");
        exporter->setFileExportCommand("multimarkdown -t html -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
        htmlExporters.append(exporter);
    }

    if (discountIsAvailable)
    {
        exporter = new CommandLineExporter("Discount");
        exporter->setFileFormatFilter(QObject::tr("Discount to HTML") + QString(" (*.html *.htm)"));
        exporter->setHtmlExportCommand("markdown");
        exporter->setFileExportCommand("markdown -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
        htmlExporters.append(exporter);
    }

    if (pandocIsAvailable)
    {
        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to OpenDocument Text (ODT)") + QString(" (*.odt)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t odt -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to OpenDocument Format (ODF)") + QString(" (*.odt *.fodt)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t opendocument -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
    }

    if (mmdIsAvailable)
    {
        exporter = new CommandLineExporter("MultiMarkdown");
        exporter->setFileFormatFilter(QObject::tr("MultiMarkdown to Open Document Text (ODT)") + QString(" (*.odt *.fodt)"));
        exporter->setFileExportCommand("multimarkdown -t odf -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
    }

    if (pandocIsAvailable)
    {
        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to Word Document") + QString(" (*.docx)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t docx -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to Rich Text Format") + QString(" (*.rtf)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t rtf -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("rtf");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to PDF") + QString(" (*.pdf)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t latex -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("pdf");
        fileExporters.append(exporter);
    }

    if (pandocIsAvailable)
    {
        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to LaTeX") + QString(" (*.tex *.ltx *.latex)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t latex -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
    }

    if (mmdIsAvailable)
    {
        exporter = new CommandLineExporter("MultiMarkdown");
        exporter->setFileFormatFilter(QObject::tr("MultiMarkdown to LaTeX") + QString(" (*.tex *.ltx *.latex)"));
        exporter->setFileExportCommand("multimarkdown -t latex -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
    }

    if (pandocIsAvailable)
    {
        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to LaTeX beamer slideshow") + QString(" (*.txt)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t beamer -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to LaTeX beamer slideshow PDF") + QString(" (*.pdf)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t beamer-pdf -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
    }

    if (mmdIsAvailable)
    {
        exporter = new CommandLineExporter("MultiMarkdown");
        exporter->setFileFormatFilter(QObject::tr("MultiMarkdown to LaTeX beamer slideshow") + QString(" (*.txt)"));
        exporter->setFileExportCommand("multimarkdown -t beamer -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("MultiMarkdown");
        exporter->setFileFormatFilter(QObject::tr("MultiMarkdown to memoir") + QString(" (*.tex *.ltx *.latex)"));
        exporter->setFileExportCommand("multimarkdown -t memoir -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
    }

    if (pandocIsAvailable)
    {
        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to EPUB v2 Book") + QString(" (*.epub)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t epub -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to EPUB v3 Book") + QString(" (*.epub)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t epub3 -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to FictionBook2 e-book") + QString(" (*.fb2)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t fb2 -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to OPML") + QString(" (*.opml)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t opml -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("opml");
        fileExporters.append(exporter);
    }

    if (mmdIsAvailable)
    {
        exporter = new CommandLineExporter("MultiMarkdown");
        exporter->setFileFormatFilter(QObject::tr("MultiMarkdown to OPML") + QString(" (*.opml)"));
        exporter->setFileExportCommand("multimarkdown -t opml -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("MultiMarkdown");
        exporter->setFileFormatFilter(QObject::tr("MultiMarkdown to LyX") + QString(" (*.lyx)"));
        exporter->setFileExportCommand("multimarkdown -t lyx -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
    }

    if (pandocIsAvailable)
    {
        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to Plain Text") + QString(" (*)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t plain -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to ConTeXt") + QString(" (*.tex *.txt)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t context -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("tex");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to groff man page") + QString(" (*.man)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t man -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("man");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to MediaWiki") + QString(" (*.mediawiki *.txt)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t mediawiki -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to DokuWiki") + QString(" (*)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t dokuwiki -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to Textile") + QString(" (*.textile *.txt)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t textile -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to Emacs Org-Mode") + QString(" (*.org)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t org -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("org");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to GNU Texinfo") + QString(" (*.texinfo)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t texinfo -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("texinfo");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to DocBook") + QString(" (*.dbk *.xml)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t docbook -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to Haddock markup") + QString(" (*.haddock)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t haddock -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("haddock");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to AsciiDoc") + QString(" (*)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t asciidoc -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to reStructuredText") + QString(" (*)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t rst -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to InDesign ICML") + QString(" (*.icml)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t icml -o ${OUTPUT_FILE_PATH}");
        exporter->setDefaultOutputFileExtension("icml");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to Slidy HTML/JavaScript Slide Show") + QString(" (*.html *.htm)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t slidy -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to Slideous HTML/JavaScript Slide Show") + QString(" (*.html *.htm)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t slideous -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to DZSlides HTML/JavaScript Slide Show") + QString(" (*.html *.htm)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t dzslides -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to reveal.js HTML/JavaScript Slide Show") + QString(" (*.html *.htm)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t revealjs -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);

        exporter = new CommandLineExporter("Pandoc");
        exporter->setFileFormatFilter(QObject::tr("Pandoc to S5 HTML/JavaScript Slide Show") + QString(" (*.html *.htm)"));
        exporter->setFileExportCommand("pandoc --smart -f markdown -t s5 -o ${OUTPUT_FILE_PATH}");
        fileExporters.append(exporter);
    }
}

bool ExporterFactory::isCommandAvailable(const QString& testCommand) const
{
    QProcess process;
    process.start(testCommand);

    if (!process.waitForStarted(500))
    {
        return false;
    }
    else
    {
        return true;
    }
}
