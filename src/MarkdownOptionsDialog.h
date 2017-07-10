#ifndef MARKDOWNOPTIONSDIALOG_H
#define MARKDOWNOPTIONSDIALOG_H

#include <QObject>
#include <QDialog>

#include "ExporterFactory.h"

class QComboBox;

class MarkdownOptionsDialog : public QDialog
{
    Q_OBJECT

    public:
        MarkdownOptionsDialog(QWidget* parent = NULL);
        virtual ~MarkdownOptionsDialog();

    private slots:
        void onExporterChanged(int index);
        void onStyleSheetChanged(int index);
        void onCustomCssFilesChanged(const QStringList& fileList);


    private:
        ExporterFactory* exporterFactory;
        QStringList defaultStyleSheets;
        QComboBox* previewerComboBox;
        QComboBox* styleSheetComboBox;
        QStringList customCssFiles;

        void buildStyleSheetComboBox();
};

#endif // MARKDOWNOPTIONSDIALOG_H
