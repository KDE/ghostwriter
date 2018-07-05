/***********************************************************************
 *
 * Copyright (C) 2017-2018 wereturtle
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

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QFileInfo>

#include "PreviewOptionsDialog.h"
#include "AppSettings.h"

PreviewOptionsDialog::PreviewOptionsDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle(tr("Preview Options"));

    exporterFactory = ExporterFactory::getInstance();
    defaultStyleSheets.append(":/resources/github.css");
    defaultStyleSheets.append(":/resources/github-dark.css");

    QWidget* mainContents = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);
    layout->addWidget(mainContents);

    QFormLayout* optionsLayout = new QFormLayout();
    mainContents->setLayout(optionsLayout);

    previewerComboBox = new QComboBox();

    QList<Exporter*> exporters = exporterFactory->getHtmlExporters();
    Exporter* currentExporter = AppSettings::getInstance()->getCurrentHtmlExporter();

    int currentExporterIndex = 0;

    for (int i = 0; i < exporters.length(); i++)
    {
        Exporter* exporter = exporters.at(i);
        previewerComboBox->addItem(exporter->getName(), qVariantFromValue((void *) exporter));

        if (exporter == currentExporter)
        {
            currentExporterIndex = i;
        }
    }

    previewerComboBox->setCurrentIndex(currentExporterIndex);

    connect(previewerComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onExporterChanged(int)));

    optionsLayout->addRow(tr("Markdown Flavor"), previewerComboBox);

    styleSheetComboBox = new QComboBox(this);
    buildStyleSheetComboBox();
    optionsLayout->addRow(tr("Style Sheet"), styleSheetComboBox);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);

    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));

    connect(AppSettings::getInstance(), SIGNAL(customCssFilesChanged(QStringList)), this, SLOT(onCustomCssFilesChanged(QStringList)));
}

PreviewOptionsDialog::~PreviewOptionsDialog()
{
    ;
}

void PreviewOptionsDialog::onExporterChanged(int index)
{
    QVariant exporterVariant = previewerComboBox->itemData(index);
    Exporter* exporter = (Exporter*) exporterVariant.value<void*>();
    AppSettings::getInstance()->setCurrentHtmlExporter(exporter);
}

void PreviewOptionsDialog::onStyleSheetChanged(int index)
{
    QString filePath = styleSheetComboBox->itemData(index).toString();
    AppSettings::getInstance()->setCurrentCssFile(filePath);
}

void PreviewOptionsDialog::onCustomCssFilesChanged(const QStringList& fileList)
{
    Q_UNUSED(fileList);

    buildStyleSheetComboBox();
}

void PreviewOptionsDialog::buildStyleSheetComboBox()
{
    styleSheetComboBox->disconnect();
    styleSheetComboBox->clear();
    styleSheetComboBox->addItem(tr("Github (Default)"));
    styleSheetComboBox->setItemData(0, QVariant(defaultStyleSheets.at(0)));
    styleSheetComboBox->addItem(tr("Github Dark"));
    styleSheetComboBox->setItemData(1, QVariant(defaultStyleSheets.at(1)));

    int customCssIndexStart = defaultStyleSheets.size();

    AppSettings* appSettings = AppSettings::getInstance();
    QString currentCssFile = appSettings->getCurrentCssFile();
    QStringList customCssFiles = appSettings->getCustomCssFiles();

    for (int i = 0; i < customCssFiles.size(); i++)
    {
        QFileInfo fileInfo(customCssFiles.at(i));

        if (fileInfo.exists())
        {
            styleSheetComboBox->addItem(fileInfo.completeBaseName());
            styleSheetComboBox->setItemData(customCssIndexStart + i, QVariant(customCssFiles.at(i)));
        }
    }

    bool foundLastUsedCssFile = false;

    // Find the last used style sheet, and set it as selected in the combo box.
    for (int i = 0; i < styleSheetComboBox->count(); i++)
    {
        if (styleSheetComboBox->itemData(i).toString() == currentCssFile)
        {
            styleSheetComboBox->setCurrentIndex(i);
            foundLastUsedCssFile = true;
            break;
        }
    }

    if (!foundLastUsedCssFile)
    {
        styleSheetComboBox->setCurrentIndex(0);
        AppSettings::getInstance()->setCurrentCssFile(defaultStyleSheets.first());
    }

    connect(styleSheetComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onStyleSheetChanged(int)));
}
