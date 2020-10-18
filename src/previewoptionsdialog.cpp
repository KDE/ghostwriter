/***********************************************************************
 *
 * Copyright (C) 2017-2020 wereturtle
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

#include "previewoptionsdialog.h"
#include "appsettings.h"
#include "exporterfactory.h"

namespace ghostwriter
{
class PreviewOptionsDialogPrivate
{
public:
    PreviewOptionsDialogPrivate()
    {
        ;
    }

    ~PreviewOptionsDialogPrivate()
    {
        ;
    }

    void onExporterChanged(int index);

    ExporterFactory *exporterFactory;
    QComboBox *previewerComboBox;

    void buildStyleSheetComboBox();
};

PreviewOptionsDialog::PreviewOptionsDialog(QWidget *parent)
    : QDialog(parent),
      d_ptr(new PreviewOptionsDialogPrivate())
{
    this->setWindowTitle(tr("Preview Options"));

    d_func()->exporterFactory = ExporterFactory::instance();

    QWidget *mainContents = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout();
    this->setLayout(layout);
    layout->addWidget(mainContents);

    QFormLayout *optionsLayout = new QFormLayout();
    mainContents->setLayout(optionsLayout);

    d_func()->previewerComboBox = new QComboBox(this);

    QList<Exporter *> exporters = d_func()->exporterFactory->htmlExporters();
    Exporter *currentExporter = AppSettings::instance()->currentHtmlExporter();

    int currentExporterIndex = 0;

    for (int i = 0; i < exporters.length(); i++) {
        Exporter *exporter = exporters.at(i);
        d_func()->previewerComboBox->addItem(exporter->name(), QVariant::fromValue((void *) exporter));

        if (exporter == currentExporter) {
            currentExporterIndex = i;
        }
    }

    d_func()->previewerComboBox->setCurrentIndex(currentExporterIndex);

    this->connect
    (
        d_func()->previewerComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this](int index) {
            d_func()->onExporterChanged(index);
        }
    );

    optionsLayout->addRow(tr("Markdown Flavor"), d_func()->previewerComboBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);

    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
}

PreviewOptionsDialog::~PreviewOptionsDialog()
{
    ;
}

void PreviewOptionsDialogPrivate::onExporterChanged(int index)
{
    QVariant exporterVariant = this->previewerComboBox->itemData(index);
    Exporter *exporter = (Exporter *) exporterVariant.value<void *>();
    AppSettings::instance()->setCurrentHtmlExporter(exporter);
}
} // namespace ghostwriter
