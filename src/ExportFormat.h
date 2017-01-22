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

#ifndef EXPORTFORMAT_H
#define EXPORTFORMAT_H

#include <QString>

/**
 * Encapsulates an export format for exporting to a specific file format.
 */
class ExportFormat
{
    public:
        /**
         * Constructor.
         */
        ExportFormat();

        /**
         * Constructor that initializes this ExportFormat with a name, file
         * filter, default file extension for the format, and a boolean flag
         * indicating whether the file extension is mandatory for this file
         * format.
         */
        ExportFormat
        (
            const QString& name,
            const QString& fileFilter,
            const QString& defaultFileExtension = QString(),
            bool fileExtensionMandatory = false
        );

        /**
         * Destructor.
         */
        ~ExportFormat();

        /**
         * Gets the format's name.
         */
        QString getName() const;

        /**
         * Sets the format's name.
         */
        void setName(const QString& value);

        /**
         * Gets the file filter (extensions only) for use with QFileDialog.
         * For example, it might return the following for an HTML format,
         * including the parentheses:
         *
         *      (*.html *.htm)
         */
        QString getFileFilter() const;

        /**
         * Sets the file filter (extensions only) for use with QFileDialog. Note
         * that the filter must be enclosed with paretheses, since a named
         * filter will be built in conjunction with the ExportFormat's name.
         * Below is an example for an HTML format:
         *
         *      (*.html *.htm)
         */
        void setFileFilter(const QString& value);

        /**
         * Gets a named filter for direct use with QFileDialog, which combines
         * this ExportFormat's name with its file filter.
         */
        QString getNamedFilter() const;

        /**
         * Gets the default file extension for this format.  For example, for
         * an HTML file format, it would return "html" without the
         * quotation marks.
         */
        QString getDefaultFileExtension() const;

        /**
         * Sets the default file extension for this format.  For example, for
         * an HTML file format, you might set the value to "html" without the
         * quotation marks.
         */
        void setDefaultFileExtension(const QString& value);

        /**
         * Returns true if the file extension is mandatory for this format.
         */
        bool isFileExtensionMandatory() const;

        /**
         * Set to true if the file extension is mandatory for this format.
         */
        void setFileExtenstionMandatory(bool mandatory);

        // Some common file formats.
        static const ExportFormat* const HTML;
        static const ExportFormat* const HTML5;
        static const ExportFormat* const ODT;
        static const ExportFormat* const ODF;
        static const ExportFormat* const RTF;
        static const ExportFormat* const DOCX;
        static const ExportFormat* const PDF;
        static const ExportFormat* const PDF_LATEX;
        static const ExportFormat* const PDF_CONTEXT;
        static const ExportFormat* const PDF_WKHTML;
        static const ExportFormat* const EPUBV2;
        static const ExportFormat* const EPUBV3;
        static const ExportFormat* const FICTIONBOOK2;
        static const ExportFormat* const LATEX;
        static const ExportFormat* const LYX;
        static const ExportFormat* const MEMOIR;
        static const ExportFormat* const GROFFMAN;
        static const ExportFormat* const MANPAGE;

    private:
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


#endif // EXPORTFORMAT_H
