/*
 * SPDX-FileCopyrightText: 2017-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QFileInfo>
#include <QLineEdit>

#include "previewoptionsdialog.h"
#include "appsettings.h"
#include "exporterfactory.h"
#include "simplefontdialog.h"

namespace ghostwriter
{
class PreviewOptionsDialogPrivate
{
public:
    PreviewOptionsDialogPrivate()
    = default;

    ~PreviewOptionsDialogPrivate()
    = default;

    void onExporterChanged(int index) const;
    [[nodiscard]] static QString fontToString(const QFont &font) ;

    AppSettings *appSettings;
    ExporterFactory *exporterFactory;
    QComboBox *previewerComboBox;
};

PreviewOptionsDialog::PreviewOptionsDialog(QWidget *parent)
    : QDialog(parent),
      d_ptr(new PreviewOptionsDialogPrivate())
{
    Q_D(PreviewOptionsDialog);

    this->setWindowTitle(tr("Preview Options"));
    d->appSettings = AppSettings::instance();
    d->exporterFactory = ExporterFactory::instance();

    auto *mainContents = new QWidget(this);
    auto *layout = new QVBoxLayout();
    this->setLayout(layout);
    layout->addWidget(mainContents);

    auto *optionsLayout = new QFormLayout();
    mainContents->setLayout(optionsLayout);

    d->previewerComboBox = new QComboBox(this);

    QList<Exporter *> exporters = d->exporterFactory->htmlExporters();
    Exporter *currentExporter = d->appSettings->currentHtmlExporter();

    int currentExporterIndex = 0;

    for (int i = 0; i < exporters.length(); i++) {
        Exporter *exporter = exporters.at(i);
        d->previewerComboBox->addItem(exporter->name(), QVariant::fromValue((void *) exporter));

        if (exporter == currentExporter) {
            currentExporterIndex = i;
        }
    }

    d->previewerComboBox->setCurrentIndex(currentExporterIndex);

    ghostwriter::PreviewOptionsDialog::connect
    (
        d->previewerComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [d](int index) {
            d->onExporterChanged(index);
        }
    );

    optionsLayout->addRow(tr("Markdown Flavor"), d->previewerComboBox);

    auto *fontLayout = new QHBoxLayout();

    auto *currentTextFont = new QLineEdit(ghostwriter::PreviewOptionsDialogPrivate::fontToString(d->appSettings->previewTextFont()));
    currentTextFont->setReadOnly(true);
    fontLayout->addWidget(currentTextFont);

    auto *chooseButton = new QPushButton(tr("Choose..."));
    fontLayout->addWidget(chooseButton);

    QPushButton::connect(chooseButton,
        &QPushButton::clicked,
        [this, d, currentTextFont]() {
            bool success = false;

            QFont font = SimpleFontDialog::font(&success,
                d->appSettings->previewTextFont(),
                this);

            if (success) {
                currentTextFont->setText(ghostwriter::PreviewOptionsDialogPrivate::fontToString(font));
                d->appSettings->setPreviewTextFont(font);
            }
        });

    optionsLayout->addRow(tr("Text Font:"), fontLayout);

    fontLayout = new QHBoxLayout();

    auto *currentCodeFont = new QLineEdit(ghostwriter::PreviewOptionsDialogPrivate::fontToString(d->appSettings->previewCodeFont()));
    currentCodeFont->setReadOnly(true);
    fontLayout->addWidget(currentCodeFont);

    chooseButton = new QPushButton(tr("Choose..."));
    fontLayout->addWidget(chooseButton);

    QPushButton::connect(chooseButton,
        &QPushButton::clicked,
        [this, d, currentCodeFont]() {
            bool success = false;

            QFont font = SimpleFontDialog::monospaceFont(&success,
                d->appSettings->previewTextFont(),
                this);

            if (success) {
                currentCodeFont->setText(ghostwriter::PreviewOptionsDialogPrivate::fontToString(font));
                d->appSettings->setPreviewCodeFont(font);
            }
        });

    optionsLayout->addRow(tr("Code Font:"), fontLayout);

    auto *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);


    auto *paramsLineEdit = new QLineEdit();
    paramsLineEdit->setText(d->appSettings->currentHtmlExporter()->options());
    optionsLayout->addRow(tr("Command line options:"), paramsLineEdit);
    QLineEdit::connect(paramsLineEdit, &QLineEdit::textChanged, [=](const QString& obj) {
        Exporter *exporter = d->appSettings->currentHtmlExporter();
        exporter->setOptions(obj);
        d->appSettings->setCurrentHtmlExporter(exporter);
    });


    QDialogButtonBox::connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
}

PreviewOptionsDialog::~PreviewOptionsDialog()
= default;

void PreviewOptionsDialogPrivate::onExporterChanged(int index) const
{
    QVariant exporterVariant = this->previewerComboBox->itemData(index);
    auto *exporter = (Exporter *) exporterVariant.value<void *>();
    exporter->setOptions(this->appSettings->currentHtmlExporter()->options());
    appSettings->setCurrentHtmlExporter(exporter);
}

QString PreviewOptionsDialogPrivate::fontToString(const QFont &font)
{
    return PreviewOptionsDialog::tr("%1 %2pt")
        .arg(font.family())
        .arg(font.pointSize());
}

} // namespace ghostwriter
