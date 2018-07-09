/***********************************************************************
 *
 * Copyright (C) 2015-2016 wereturtle
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

#include "ExportFormat.h"

// Definitions of common export format constants.
const ExportFormat* const ExportFormat::HTML =
    new ExportFormat("HTML", "(*.html *.htm)", "html");
const ExportFormat* const ExportFormat::HTML5
    = new ExportFormat("HTML 5", "(*.html *.htm)", "html");
const ExportFormat* const ExportFormat::ODT
    = new ExportFormat("OpenDocument Text", "(*.odt)", "odt", true);
const ExportFormat* const ExportFormat::ODF
    = new ExportFormat("OpenDocument Flat XML Format", "(*.odt *.fodt *.xml)", "odt");
const ExportFormat* const ExportFormat::RTF
    = new ExportFormat("Rich Text Format", "(*.rtf)", "rtf", true);
const ExportFormat* const ExportFormat::DOCX
    = new ExportFormat("Word Document", "(*.docx)", "docx", true);
const ExportFormat* const ExportFormat::PDF
    = new ExportFormat("PDF", "(*.pdf)", "pdf", true);
const ExportFormat* const ExportFormat::PDF_LATEX
    = new ExportFormat("PDF (LaTeX)", "(*.pdf)", "pdf", true);
const ExportFormat* const ExportFormat::PDF_CONTEXT
    = new ExportFormat("PDF (ConTeXt)", "(*.pdf)", "pdf", true);
const ExportFormat* const ExportFormat::PDF_WKHTML
    = new ExportFormat("PDF (wkhtmltopdf)", "(*.pdf)", "pdf", true);
const ExportFormat* const ExportFormat::EPUBV2
    = new ExportFormat("EPUB v2 Book", "(*.epub)", "epub", true);
const ExportFormat* const ExportFormat::EPUBV3
    = new ExportFormat("EPUB v3 Book", "(*.epub)", "epub", true);
const ExportFormat* const ExportFormat::FICTIONBOOK2
    = new ExportFormat("FictionBook2 e-book", "(*.fb2)", "fb2", true);
const ExportFormat* const ExportFormat::LATEX
    = new ExportFormat("LaTeX", "(*.tex *.ltx *.latex)", "tex");
const ExportFormat* const ExportFormat::LYX
    = new ExportFormat("LyX", "(*.lyx)", "lyx", true);
const ExportFormat* const ExportFormat::MEMOIR
    = new ExportFormat("memoir", "(*.tex *.ltx *.latex)", "tex");
const ExportFormat* const ExportFormat::GROFFMAN
    = new ExportFormat("groff man page", "(*.man *.1 *.2 *.3 *.4 *.5 *.6 *.7 *.8)", "man", true);
const ExportFormat* const ExportFormat::MANPAGE
    = new ExportFormat("man page", "(*.man *.1 *.2 *.3 *.4 *.5 *.6 *.7 *.8)", "man", true);


ExportFormat::ExportFormat()
    : fileExtensionMandatoryFlag(false)
{
}

ExportFormat::ExportFormat
(
    const QString& name,
    const QString& fileFilter,
    const QString& defaultFileExtension,
    bool fileExtensionMandatory
)
    : defaultFileExtension(defaultFileExtension),
      fileExtensionMandatoryFlag(fileExtensionMandatory)
{
    setName(name);
    setFileFilter(fileFilter);
}

ExportFormat::~ExportFormat()
{

}

QString ExportFormat::getName() const
{
    return name;
}

void ExportFormat::setName(const QString& value)
{
    name = value;
    createNamedFilter();
}

QString ExportFormat::getFileFilter() const
{
    return fileFilter;
}

void ExportFormat::setFileFilter(const QString& value)
{
    fileFilter = value;
    createNamedFilter();
}

QString ExportFormat::getNamedFilter() const
{
    return namedFilter;
}

QString ExportFormat::getDefaultFileExtension() const
{
    return defaultFileExtension;
}

void ExportFormat::setDefaultFileExtension(const QString& value)
{
    defaultFileExtension = value;
}

bool ExportFormat::isFileExtensionMandatory() const
{
    return fileExtensionMandatoryFlag;
}

void ExportFormat::setFileExtenstionMandatory(bool mandatory)
{
    fileExtensionMandatoryFlag = mandatory;
}

void ExportFormat::createNamedFilter()
{
    namedFilter = "";

    if (!name.isNull() && !name.isEmpty())
    {
        namedFilter = name + QString(" ");
    }

    if (!fileFilter.isNull() && !fileFilter.isEmpty())
    {
        namedFilter += fileFilter;
    }
}
