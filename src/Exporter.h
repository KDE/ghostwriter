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

#ifndef _EXPORTER_H
#define _EXPORTER_H

#include <QString>
#include <QList>

#include "ExportFormat.h"

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
        Exporter(const QString& name);

        /**
         * Destructor.
         */
        virtual ~Exporter();

        /**
         * Gets the name of the exporter.  The name should be unique among
         * other exporters used in this application.
         */
        QString getName() const;

        /**
         * Sets the name of the exporter.  The name should be unique among
         * other exporters used in this application.
         */
        void setName(const QString& name);

        /**
         * Returns the supported formats to which this exporter can export.
         * This method will return the value of the protected field
         * supportedFormats.  Implementors of this class should add
         * their supported export formats to supportedFormats.
         */
        const QList<const ExportFormat*> getSupportedFormats() const;

        /**
         * Returns true if smart typography is enabled.
         */
        bool getSmartTypographyEnabled() const;

        /**
         * Set to true if smart typography should be enabled.  See comment
         * for the protected member field smartTypographyEnabled for further
         * information.
         */
        void setSmartTypographyEnabled(bool enabled);

        /**
         * Override this method to transform the given text into HTML for
         * use in the Live HTML Preview.  By default, this method will set the
         * html paramter to have HTML-formatted error text indicating that HTML
         * is not supported by the export processor.
         */
        virtual void exportToHtml(const QString& text, QString& html);

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
            const ExportFormat* format,
            const QString& inputFilePath,
            const QString& text,
            const QString& outputFilePath,
            QString& err
        ) = 0;

    protected:
        /*
         * Implementors of this class should add their supported export formats
         * to this field.
         */
        QList<const ExportFormat*> supportedFormats;

        /*
         * Use this flag to determine whether to export using smart typography
         * (i.e., fancy quotation marks, etc., typically using Smarty Pants).
         * Note that the implementation of smart typography is optional.  It
         * is a feature that happens to be widely supported across processors,
         * and so it was included as a built-in option that the user can
         * toggle.
         */
        bool smartTypographyEnabled;

    private:
        QString name;
};

#endif
