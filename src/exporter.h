/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _EXPORTER_H
#define _EXPORTER_H

#include <QString>
#include <QList>

#include "exportformat.h"

namespace ghostwriter
{
/**
 * Abstract class to export text to another format (i.e., Markdown text to
 * HTML).  Subclass this class to create a custom exporter that can export
 * to different file formats.
 */
class Exporter
{
public:
    /**
     * Constructor.  Takes unique name (that is, unique within the scope of
     * the application) of the exporter as parameter.
     */
    Exporter(const QString &name, const QString &options);

    /**
     * Destructor.
     */
    virtual ~Exporter();

    /**
     * Gets the name of the exporter.  The name should be unique among
     * other exporters used in this application.
     */
    QString name() const;

    /**
     * Get the parameters for the exporter.
     */
    QString options() const;

    /**
     * Set the parameters for the exporter.
     */
    void setOptions(const QString &options);

    /**
     * Sets the name of the exporter.  The name should be unique among
     * other exporters used in this application.
     */
    void setName(const QString &name);

    /**
     * Returns the supported formats to which this exporter can export.
     * This method will return the value of the protected field
     * supportedFormats.  Implementors of this class should add
     * their supported export formats to supportedFormats.
     */
    const QList<const ExportFormat *> supportedFormats() const;

    /**
     * Returns true if smart typography is enabled.
     */
    bool smartTypographyEnabled() const;

    /**
     * Set to true if smart typography should be enabled.  See comment
     * for the protected member field smartTypographyEnabled for further
     * information.
     */
    void setSmartTypographyEnabled(bool enabled);

    /**
     * Returns true if this exporter supports tex-based math, false otherwise.
     */
    bool supportsMath() const;

    /**
     * Override this method to transform the given text into HTML for
     * use in the Live HTML Preview.  By default, this method will set the
     * html parameter to have HTML-formatted error text indicating that HTML
     * is not supported by the export processor.
     */
    virtual void exportToHtml(const QString &text, QString &html);

    /**
     * Implement this method to export the given text to a file of the
     * given format.  Set the err variable to an error string if
     * an error occurs during export.  Note that even is export is
     * successful, it is recommended that you set the value of err
     * to a null QString (call the QString() constructor) to indicate
     * success, in case the method's caller accidentally passed in
     * a non-null, non-empty QString value.  If there is no input
     * file path due to the document being new and untitled, then
     * specify a null or empty inputFilePath value.
     */
    virtual void exportToFile
    (
        const ExportFormat *format,
        const QString &inputFilePath,
        const QString &text,
        const QString &outputFilePath,
        QString &err
    ) = 0;

protected:
    /*
    * Implementors of this class should add their supported export formats
    * to this field.
    */
    QList<const ExportFormat *> m_supportedFormats;

    /*
    * Use this flag to determine whether to export using smart typography
    * (i.e., fancy quotation marks, etc., typically using Smarty Pants).
    * Note that the implementation of smart typography is optional.  It
    * is a feature that happens to be widely supported across processors,
    * and so it was included as a built-in option that the user can
    * toggle.
    */
    bool m_smartTypographyEnabled;

    /*
    * Use this flag to indicate that tex-based math is supported.
    */
    bool m_mathSupported;

    QString m_options;

private:
    QString m_name;
};
} // namespace ghostwriter

#endif
