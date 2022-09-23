/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-FileCopyrightText: 2010-2014 Graeme Gott <graeme@gottcode.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QVBoxLayout>
#include <QComboBox>
#include <QFileInfo>
#include <QDir>
#include <QDialogButtonBox>

#include "localedialog.h"
#include "messageboxhelper.h"

namespace ghostwriter
{
LocaleDialog::LocaleDialog
(
    const QString &initialLocale,
    const QString &translationsPath,
    QWidget *parent
) : QDialog(parent)
{
    setWindowTitle(tr("Set Application Language"));

    QVBoxLayout *layout = new QVBoxLayout();

    localeComboBox = new QComboBox();

    if (!QFileInfo(translationsPath).exists()) {
        MessageBoxHelper::critical
        (
            this,
            tr("The translations folder is missing."),
            tr("Please reinstall this application for more language options.")
        );

        QLocale defaultLocale;
        QString languageCode = defaultLocale.name();

        localeComboBox->addItem(localeDescription(languageCode), languageCode);
    } else {
        QDir translationsDir(translationsPath);
        QStringList fileNameFilters;
        fileNameFilters.append("ghostwriter_*.qm");

        QFileInfoList fileInfos =
            translationsDir.entryInfoList
            (
                fileNameFilters,
                QDir::Files | QDir::NoSymLinks | QDir::Readable
            );

        int currentIndex = 0;
        int row = 0;

        foreach (QFileInfo info, fileInfos) {
            if (!info.isDir()) {
                // Grab the language/country from the file name.

                QString baseName = info.baseName();

                int endIndex = baseName.lastIndexOf('.');

                int appPrefixLength = sizeof("ghostwriter_") - 1;

                QString languageCode = baseName.mid(appPrefixLength, endIndex - appPrefixLength - 1);
                QLocale locale(languageCode);

                // Add the locale to the combo box if it is valid.
                if (locale.language() != QLocale::C) {
                    localeComboBox->addItem(localeDescription(languageCode), languageCode);

                    if (languageCode == initialLocale) {
                        currentIndex = row;
                    }

                    row++;
                }
            }
        }

        localeComboBox->setCurrentIndex(currentIndex);
    }

    layout->addWidget(localeComboBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    this->setLayout(layout);
}

LocaleDialog::~LocaleDialog()
{

}

QString LocaleDialog::selectedLocale() const
{
    return localeComboBox->itemData(localeComboBox->currentIndex()).toString();
}

QString LocaleDialog::locale
(
    bool *ok,
    const QString &initialLocale,
    const QString &translationsPath,
    QWidget *parent
)
{
    LocaleDialog dialog(initialLocale, translationsPath, parent);

    int status = dialog.exec();

    QString locale;
    *ok = false;

    if (QDialog::Accepted == status) {
        locale = dialog.selectedLocale();
        *ok = true;
    }

    return locale;
}

// Lifted from FocusWriter
QString LocaleDialog::localeDescription(const QString &languageCode)
{
    QLocale locale(languageCode.left(5));
    QString localeDescription = locale.nativeLanguageName();

    if (languageCode.length() > 2) {
        if (locale.name() == languageCode) {
            localeDescription +=
                " (" + locale.nativeCountryName() + ")";
        } else {
            localeDescription +=
                " (" + languageCode + ")";
        }
    }

    if (locale.textDirection() == Qt::RightToLeft) {
        localeDescription.prepend(QChar(0x202b));
    }

    return localeDescription;
}
} // namespace ghostwriter
