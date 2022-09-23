/*
 * SPDX-FileCopyrightText: 2015-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "exportformat.h"

namespace ghostwriter
{
class ExportFormatPrivate
{
public:
    ExportFormatPrivate()
    {
        ;
    }

    ~ExportFormatPrivate()
    {
        ;
    }

    QString name;
    QString fileFilter;
    QString namedFilter;
    QString defaultFileExtension;
    bool fileExtensionMandatoryFlag;

    /*
    * Combines the name with the filter for use as a named filter
    * with QFileDialog.
    */
    void createNamedFilter();
};

// Definitions of common export format constants.
const ExportFormat *const ExportFormat::HTML =
    new ExportFormat("HTML", "(*.html *.htm)", "html");
const ExportFormat *const ExportFormat::HTML5
    = new ExportFormat("HTML 5", "(*.html *.htm)", "html");
const ExportFormat *const ExportFormat::ODT
    = new ExportFormat("OpenDocument Text", "(*.odt)", "odt", true);
const ExportFormat *const ExportFormat::ODF
    = new ExportFormat("OpenDocument Flat XML Format", "(*.odt *.fodt *.xml)", "odt");
const ExportFormat *const ExportFormat::RTF
    = new ExportFormat("Rich Text Format", "(*.rtf)", "rtf", true);
const ExportFormat *const ExportFormat::DOCX
    = new ExportFormat("Word Document", "(*.docx)", "docx", true);
const ExportFormat *const ExportFormat::PDF
    = new ExportFormat("PDF", "(*.pdf)", "pdf", true);
const ExportFormat *const ExportFormat::PDF_LATEX
    = new ExportFormat("PDF (LaTeX)", "(*.pdf)", "pdf", true);
const ExportFormat *const ExportFormat::PDF_CONTEXT
    = new ExportFormat("PDF (ConTeXt)", "(*.pdf)", "pdf", true);
const ExportFormat *const ExportFormat::PDF_WKHTML
    = new ExportFormat("PDF (wkhtmltopdf)", "(*.pdf)", "pdf", true);
const ExportFormat *const ExportFormat::EPUBV2
    = new ExportFormat("EPUB v2 Book", "(*.epub)", "epub", true);
const ExportFormat *const ExportFormat::EPUBV3
    = new ExportFormat("EPUB v3 Book", "(*.epub)", "epub", true);
const ExportFormat *const ExportFormat::FICTIONBOOK2
    = new ExportFormat("FictionBook2 e-book", "(*.fb2)", "fb2", true);
const ExportFormat *const ExportFormat::LATEX
    = new ExportFormat("LaTeX", "(*.tex *.ltx *.latex)", "tex");
const ExportFormat *const ExportFormat::LYX
    = new ExportFormat("LyX", "(*.lyx)", "lyx", true);
const ExportFormat *const ExportFormat::MEMOIR
    = new ExportFormat("memoir", "(*.tex *.ltx *.latex)", "tex");
const ExportFormat *const ExportFormat::GROFFMAN
    = new ExportFormat("groff man page", "(*.man *.1 *.2 *.3 *.4 *.5 *.6 *.7 *.8)", "man", true);
const ExportFormat *const ExportFormat::MANPAGE
    = new ExportFormat("man page", "(*.man *.1 *.2 *.3 *.4 *.5 *.6 *.7 *.8)", "man", true);


ExportFormat::ExportFormat()
    : d_ptr(new ExportFormatPrivate())
{
    Q_D(ExportFormat);
    
    d->fileExtensionMandatoryFlag = false;
}

ExportFormat::ExportFormat
(
    const QString &name,
    const QString &fileFilter,
    const QString &defaultFileExtension,
    bool fileExtensionMandatory
) : d_ptr(new ExportFormatPrivate())
{
    Q_D(ExportFormat);
    
    setName(name);
    setFileFilter(fileFilter);

    d->defaultFileExtension = defaultFileExtension;
    d->fileExtensionMandatoryFlag = fileExtensionMandatory;
}

ExportFormat::~ExportFormat()
{
    ;
}

QString ExportFormat::name() const
{
    Q_D(const ExportFormat);
    
    return d->name;
}

void ExportFormat::setName(const QString &value)
{
    Q_D(ExportFormat);
    
    d->name = value;
    d->createNamedFilter();
}

QString ExportFormat::fileFilter() const
{
    Q_D(const ExportFormat);
    
    return d->fileFilter;
}

void ExportFormat::setFileFilter(const QString &value)
{
    Q_D(ExportFormat);
    
    d->fileFilter = value;
    d->createNamedFilter();
}

QString ExportFormat::namedFilter() const
{
    Q_D(const ExportFormat);
    
    return d->namedFilter;
}

QString ExportFormat::defaultFileExtension() const
{
    Q_D(const ExportFormat);
    
    return d->defaultFileExtension;
}

void ExportFormat::setDefaultFileExtension(const QString &value)
{
    Q_D(ExportFormat);
    
    d->defaultFileExtension = value;
}

bool ExportFormat::isFileExtensionMandatory() const
{
    Q_D(const ExportFormat);
    
    return d->fileExtensionMandatoryFlag;
}

void ExportFormat::setFileExtenstionMandatory(bool mandatory)
{
    Q_D(ExportFormat);
    
    d->fileExtensionMandatoryFlag = mandatory;
}

void ExportFormatPrivate::createNamedFilter()
{
    namedFilter = "";

    if (!name.isNull() && !name.isEmpty()) {
        namedFilter = name + QString(" ");
    }

    if (!fileFilter.isNull() && !fileFilter.isEmpty()) {
        namedFilter += fileFilter;
    }
}
} // namespace ghostwriter
