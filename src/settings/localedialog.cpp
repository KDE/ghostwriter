/*
 * SPDX-FileCopyrightText: 2016-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QVBoxLayout>

#include <KLanguageButton>

#include "localedialog.h"
#include "appsettings.h"
#include "../messageboxhelper.h"

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
    d->languageButton->showLanguageCodes(true);

    auto currentLanguageCode = AppSettings::instance()->locale();
    auto languageCodes = AppSettings::instance()->availableTranslations();

    if (currentLanguageCode.isNull() || currentLanguageCode.isEmpty()) {
        currentLanguageCode = QLocale().name();
    }

    QString selectedLanguage;

    if (languageCodes.isEmpty()) {
        MessageBoxHelper::critical(
            this,
            tr("No translations are available!"),
            tr("Please reinstall this application for more language options.")
        );
    } else {
        const QLocale currentLocale(currentLanguageCode);

        if (QLatin1String("C") != currentLocale.name()) {
            enum {
                NoMatch,
                LanguageMatch,
                LanguageCountryMatch,
                PerfectMatch
            } matchAccuracy = NoMatch;

            for (auto languageCode : languageCodes) {
                const QLocale locale(languageCode);
                d->languageButton->insertLanguage(languageCode);

                if ((matchAccuracy < LanguageMatch)
                        && (locale.language() == currentLocale.language())) {
                    matchAccuracy = LanguageMatch;
                    selectedLanguage = languageCode;
                }

                if ((LanguageMatch == matchAccuracy) && (locale.territory() == currentLocale.territory())) {
                    matchAccuracy = LanguageCountryMatch;
                    selectedLanguage = languageCode;
                }

                if ((LanguageCountryMatch == matchAccuracy)
                        && locale.script() == currentLocale.script()) {
                    selectedLanguage = languageCode;
                    matchAccuracy = PerfectMatch;
                }

                // This case covers local dialects, such as "ca@valencia".
                // See QTBUG-7100 for details.
                if (languageCode == currentLanguageCode) {
                    selectedLanguage = languageCode;
                    matchAccuracy = PerfectMatch;
                }
            }
        }
    }

    if (selectedLanguage.isNull()) {
        selectedLanguage = "en";
    }

    if (d->languageButton->count() <= 0) {
        // Insert fall back language.
        selectedLanguage = "en";
        d->languageButton->insertLanguage(selectedLanguage);
    }

    d->languageButton->setCurrentItem(selectedLanguage);
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
                QMessageBox::information(
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
