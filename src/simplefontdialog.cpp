/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#include <QGridLayout>
#include <QLineEdit>
#include <QList>
#include <QLabel>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QFontInfo>

#include "simplefontdialog.h"

namespace ghostwriter
{

class SimpleFontDialogPrivate
{
public:
    SimpleFontDialogPrivate()
    {
        ;
    }

    ~SimpleFontDialogPrivate()
    {
        ;
    }

    QFont font;
    QLineEdit *fontPreview;

    void onFontFamilyChanged(const QFont &font);
    void onFontSizeChanged(const QString &sizeText);
};

SimpleFontDialog::SimpleFontDialog(QWidget *parent)
    : d_ptr(new SimpleFontDialogPrivate())
{
    SimpleFontDialog(QFont(), parent);
}

SimpleFontDialog::SimpleFontDialog(const QFont &initial, QWidget *parent)
    : QDialog(parent),
      d_ptr(new SimpleFontDialogPrivate())
{
    Q_D(SimpleFontDialog);

    QFontComboBox *fontComboBox = new QFontComboBox(this);
    fontComboBox->setCurrentFont(initial);
    d->font = initial;

    QVBoxLayout *familyLayout = new QVBoxLayout();
    familyLayout->addWidget(new QLabel(tr("Family")));
    familyLayout->addWidget(fontComboBox);

    QList<int> sizes = QFontDatabase::standardSizes();

    QComboBox *sizeComboBox = new QComboBox(this);
    sizeComboBox->setEditable(true);

    QIntValidator *sizeValidator = new QIntValidator(1, 512, this);
    sizeComboBox->setValidator(sizeValidator);

    int currentSizeIndex = 0;
    QFontInfo fontInfo(initial);
    bool currentFontInserted = false;

    for (int i = 0; i < sizes.size(); i++) {
        int size = sizes[i];

        // If the current font size is a non-standard size, then insert
        // it in the appropriate place in the sorted list of fonts.
        //
        if ((fontInfo.pointSize() < size) && !currentFontInserted) {
            currentSizeIndex = i;
            sizeComboBox->addItem(QString("%1").arg(fontInfo.pointSize()), fontInfo.pointSize());
            currentFontInserted = true;
        }
        // Else current font size is in the standard font sizes list.  Set
        // the index in the combo box to its position in the list.
        //
        else {
            if (fontInfo.pointSize() == size) {
                currentSizeIndex = i;
                currentFontInserted = true;
            }
        }

        sizeComboBox->addItem(QString("%1").arg(size), size);
    }

    if (!currentFontInserted) {
        sizeComboBox->addItem(QString("%1").arg(fontInfo.pointSize()), fontInfo.pointSize());
        currentSizeIndex = sizeComboBox->count() - 1;
    }

    sizeComboBox->setCurrentIndex(currentSizeIndex);

    QVBoxLayout *sizeLayout = new QVBoxLayout();
    sizeLayout->addWidget(new QLabel(tr("Size")));
    sizeLayout->addWidget(sizeComboBox);

    d->fontPreview = new QLineEdit(tr("AaBbCcXxYyZz"), this);
    d->fontPreview->setFont(initial);

    QVBoxLayout *previewLayout = new QVBoxLayout();
    previewLayout->addWidget(new QLabel(tr("Preview")));
    previewLayout->addWidget(d->fontPreview);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);

    QGridLayout *layout = new QGridLayout();
    layout->addItem(familyLayout, 0, 0);
    layout->addItem(sizeLayout, 0, 1);
    layout->addItem(previewLayout, 1, 0, 1, 2, Qt::AlignCenter);
    layout->addWidget(buttonBox, 2, 0, 1, 2);

    setLayout(layout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    this->connect
    (
        fontComboBox,
        &QFontComboBox::currentFontChanged,
        [d](const QFont & newFont) {
            d->onFontFamilyChanged(newFont);
        }
    );
    this->connect
    (
        sizeComboBox,
        static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
        [d](const QString & sizeText) {
            d->onFontSizeChanged(sizeText);
        }
    );
    this->connect
    (
        sizeComboBox,
        &QComboBox::editTextChanged,
        [d](const QString & sizeText) {
            d->onFontSizeChanged(sizeText);
        }
    );
}

SimpleFontDialog::~SimpleFontDialog()
{
    ;
}

QFont SimpleFontDialog::selectedFont() const
{
    Q_D(const SimpleFontDialog);

    return d->font;
}

QFont SimpleFontDialog::font
(
    bool *ok,
    const QFont &initial,
    QWidget *parent
)
{
    *ok = false;

    SimpleFontDialog *dlg = new SimpleFontDialog(initial, parent);
    int result = dlg->exec();

    if (QDialog::Accepted == result) {
        *ok = true;
    }

    return dlg->selectedFont();
}

QFont SimpleFontDialog::font(bool *ok, QWidget *parent)
{
    return font(ok, QFont(), parent);
}

void SimpleFontDialogPrivate::onFontFamilyChanged(const QFont &font)
{
    int size = this->font.pointSize();
    this->font = font;
    this->font.setPointSize(size);
    fontPreview->setFont(this->font);
}

void SimpleFontDialogPrivate::onFontSizeChanged(const QString &sizeText)
{
    bool ok = false;
    int size = sizeText.toInt(&ok);

    if (ok) {
        font.setPointSize(size);
        fontPreview->setFont(font);
    }
}
} // namespace ghostwriter
