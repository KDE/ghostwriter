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

#ifndef COMMAND_LINE_EXPORTER_H
#define COMMAND_LINE_EXPORTER_H

#include <QString>
#include <QMap>

#include "Exporter.h"

/**
 * Executes commands for a command line tool that can export text to another
 * format.  Please note the comments for the OUTPUT_FILE_PATH_VAR and
 * SMART_TYPOGRAPHY_ARG constant class members below.
 */
class CommandLineExporter : public Exporter
{
    public:
        /**
         * Constructor.  Takes unique name (that is, unique within the scope of
         * the application) of the exporter as parameter.
         */
        CommandLineExporter(const QString& name);

        /**
         * Destructor.
         */
        ~CommandLineExporter();

        /**
         * Sets the command to execute for rendering text to HTML format for
         * use in the Live HTML Preview.  Note that you can add the value
         * of the SMART_TYPOGRAPHY_ARG constant to the command string if
         * the program being executed supports smart typography.
         */
        void setHtmlRenderCommand(const QString& command);

        /**
         * Adds a command to execute for exporting text to the specified
         * export format.
         */
        void addFileExportCommand
        (
            const ExportFormat* format,
            const QString& command
        );

        /**
         * Gets the command line argument used to enable smart typography
         * in the exporter.
         */
        QString getSmartTypographyOnArgument() const;

        /**
         * Sets the command line argument used to enable smart typography
         * in the exporter.
         */
        void setSmartTypographyOnArgument(const QString& argument);

        /**
         * Gets the command line argument used to disable smart typography
         * in the exporter.
         */
        QString getSmartTypographyOffArgument() const;

        /**
         * Sets the command line argument used to disable smart typography
         * in the exporter.
         */
        void setSmartTypographyOffArgument(const QString& argument);

        /**
         * Exports the given text to html, returning the HTML in the html
         * parameter for use in the Live HTML Preview.
         */
        void exportToHtml(const QString& text, QString& html);

        /**
         * Exports the given text to the given format and output file path.
         * If the command to export fails, err will be set to a non-null
         * string containing an error message.
         */
        void exportToFile
        (
            const ExportFormat* format,
            const QString& inputFilePath,
            const QString& text,
            const QString& outputFilePath,
            QString& err
        );

        /**
         * Contains the variable string for output file path.  Callers can
         * set this exporter to use a command having the output file path
         * variable in it, which will be replaced on execution with the actual
         * file path passed in to the exportToFile() method.  For example,
         * assume that OUTPUT_FILE_PATH_VAR is set to the value of
         * "${OUTPUT_FILE_PATH}".  Using this constant, your exporter can be set
         * to execute the following command for exporting to an HTML file:
         *
         *      pandoc -f markdown -t html ${OUTPUT_FILE_PATH}
         *
         * When exportToFile() is called (or similarly exportToHtml()),
         * "${OUTPUT_FILE_PATH}" will be replaced with the actual output file
         * path, such as "~/Documents/text.html".  The above command would then
         * be executed as:
         *
         *      pandoc -f markdown -t html ~/Documents/text.html
         *
         * Note that if this string value does not appear in the command string,
         * stdout will be directed to the outputFilePath provided when
         * exportToFile() is called.  For example:
         *
         *      pandoc -f markdown -t html
         *
         * When exportToFile() is called, the above command will have its stdout
         * output written to the file path specified by the outputFilePath
         * parameter passed to the method.
         */
        static const QString OUTPUT_FILE_PATH_VAR;

        /**
         * Contains the variable string for the smart typography command line
         * argument.  Callers can set this exporter to use an argument having
         * the smart typography argument variable in it, which will be replaced
         * on execution with the actual argument set with either the
         * setSmartTypographyOnArgument() or setSmartTypographyOffArgument()
         * methods, depending on whether smart typography is enabled on export.
         * For example, assume that SMART_TYPOGRAPHY_ARG is set to the value of
         * "${SMART_TYPOGRAPHY_ARG}".  Using this constant, your exporter can be
         * set to execute the following command for rendering to HTML:
         *
         *      pandoc ${SMART_TYPOGRAPHY_ARG} -f markdown -t html
         *
         * When exportToHtml() is called (or similarly exportToFile()),
         * "${SMART_TYPOGRAPHY_ARG}" will be replaced with the actual argument
         * (either for enabling or disabling smart typography), such as
         * "--smart".  The above command would then be executed as:
         *
         *      pandoc --smart -f markdown -t html
         *
         * Note that if your program does not support smart typography, you can
         * simply not use this constant when building your command.  You can
         * likewise use empty strings with calls to setSmartTypographyOnArgument
         * or setSmartTypographyOffArgument.
         */
        static const QString SMART_TYPOGRAPHY_ARG;

    private:
        QMap<const ExportFormat*, QString> formatToCommandMap;
        QString smartTypographyOnArgument;
        QString smartTypographyOffArgument;
        QString htmlRenderCommand;

        bool executeCommand
        (
            const QString& command,
            const QString& inputFilePath,
            const QString& textInput,
            const QString& outputFilePath,
            QString& stdoutOutput,
            QString& stderrOutput
        );


};

#endif // COMMAND_LINE_EXPORTER_H
