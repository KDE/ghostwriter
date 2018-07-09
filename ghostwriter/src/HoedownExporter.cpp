#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

#include "HoedownExporter.h"

#include "hoedown/html.h"

HoedownExporter::HoedownExporter() : Exporter("Hoedown")
{
    supportedFormats.append(ExportFormat::HTML);
}

HoedownExporter::~HoedownExporter()
{

}

void HoedownExporter::exportToHtml(const QString &text, QString &html)
{
    QByteArray latin1Text = text.toUtf8().data();
    hoedown_renderer* renderer;
    hoedown_document* document;
    hoedown_buffer* htmlOutputBuffer;

    hoedown_html_flags flags = hoedown_html_flags(0);

    renderer = hoedown_html_renderer_new(flags, 0);

    hoedown_extensions extensions = hoedown_extensions(HOEDOWN_EXT_TABLES | HOEDOWN_EXT_FENCED_CODE
            | HOEDOWN_EXT_SPACE_HEADERS | HOEDOWN_EXT_SUPERSCRIPT | HOEDOWN_EXT_STRIKETHROUGH |HOEDOWN_EXT_AUTOLINK);

    document = hoedown_document_new(renderer, extensions, 16);

    htmlOutputBuffer = hoedown_buffer_new(1024);
    hoedown_document_render(document, htmlOutputBuffer, (const uint8_t*) latin1Text.data(), latin1Text.length());

    hoedown_html_renderer_free(renderer);
    hoedown_document_free(document);

    if (this->getSmartTypographyEnabled())
    {
    // Run HTML through smarty pants to get fancy quotation marks, etc.
    hoedown_buffer* smartyPantsBuffer = hoedown_buffer_new(1024);
    hoedown_html_smartypants
    (
        smartyPantsBuffer,
        htmlOutputBuffer->data,
        htmlOutputBuffer->size
    );

    // Use QString::fromUtf8 to ensure proper encoding in case there are
    // unicode characters in the output HTML.
    //
    html = QString::fromUtf8
        (
            (char*) smartyPantsBuffer->data,
            smartyPantsBuffer->size
        );

    hoedown_buffer_free(smartyPantsBuffer);
    }
    else
    {
    // Use QString::fromUtf8 to ensure proper encoding in case there are
    // unicode characters in the output HTML.
    //
    html = QString::fromUtf8
        (
            (char*) htmlOutputBuffer->data,
            htmlOutputBuffer->size
        );
    }

    hoedown_buffer_free(htmlOutputBuffer);
}

void HoedownExporter::exportToFile
(
    const ExportFormat* format,
    const QString& inputFilePath,
    const QString& text,
    const QString& outputFilePath,
    QString& err
)
{
    Q_UNUSED(inputFilePath);

    QString html;

    if (ExportFormat::HTML != format)
    {
        err = QObject::tr("%1 format is unsupported by the Hoedown processor.")
            .arg(format->getName());
        return;
    }

    exportToHtml(text, html);

    if (html.isNull())
    {
        err = "Export failed";
        return;
    }

    QFile outputFile(outputFilePath);

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        err = outputFile.errorString();
        return;
    }

    // Write contents to disk.
    QTextStream outStream(&outputFile);

    // Specify the character set (UTF-8) for the HTML document.
    // Browsers typically can't tell if the HTML has unicode characters
    // unless UTF-8 is specified in the <head> section.
    //
    outStream << "<html><head><meta http-equiv=\"Content-Type\" "
        "content=\"text/html; charset=utf-8\" />"
        "<title></title></head><body>";

    outStream << html;
    outStream << "</body></html>";

    if (QFile::NoError != outputFile.error())
    {
        err = outputFile.errorString();
    }

    // Close the file.  All done!
    outputFile.close();
}
