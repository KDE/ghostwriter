/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QVBoxLayout>

#include <KConfigWidgets/KLanguageButton>

#include "localedialog.h"
#include "appsettings.h"
#include "messageboxhelper.h"

namespace ghostwriter
{

class LocaleDialogPrivate
{
public:

    LocaleDialogPrivate() { }
    ~LocaleDialogPrivate() { }

    KLanguageButton *languageButton;
};

LocaleDialog::LocaleDialog
(
    QWidget *parent
) : QDialog(parent), d(new LocaleDialogPrivate())
{
    setWindowTitle(tr("Set Application Language"));
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *layout = new QVBoxLayout();

    d->languageButton = new KLanguageButton(this);

    auto currentLanguageCode = AppSettings::instance()->locale();
    auto languageCodes = AppSettings::instance()->availableTranslations();

    if (languageCodes.isEmpty()) {
        MessageBoxHelper::critical(
            this,
            tr("No translations are available!"),
            tr("Please reinstall this application for more language options.")
        );

        QLocale defaultLocale;
        QString languageCode = defaultLocale.name();

        d->languageButton->insertLanguage(languageCode);
    } else {
        for (auto languageCode : languageCodes) {
            d->languageButton->insertLanguage(languageCode);
            QLocale locale(languageCode);
            QLocale currentLocale(currentLanguageCode);

            if (locale == currentLocale) {
                d->languageButton->setCurrentItem(languageCode);
            }
        }
    }

    d->languageButton->showLanguageCodes(true);
    layout->addWidget(d->languageButton);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(
        this,
        &LocaleDialog::accepted,
        [this]() {
            QString languageCode = d->languageButton->current();

            if (!AppSettings::instance()->setLocale(languageCode)) {
                MessageBoxHelper::critical(
                    this,
                    tr("Sorry!"),
                    tr("Could not load translation.")
                );
            } else {
                QMessageBox::information
                (
                    this,
                    QApplication::applicationName(),
                    tr("Please restart the application for changes to take effect.")
                );
            }
        }
    );

    this->setLayout(layout);
}

LocaleDialog::~LocaleDialog()
{
    ;
}

} // namespace ghostwriter
