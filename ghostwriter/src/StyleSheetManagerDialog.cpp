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

#include <QListWidget>
#include <QMessageBox>
#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileInfo>
#include <QFileDialog>

#include "StyleSheetManagerDialog.h"
#include "MessageBoxHelper.h"

StyleSheetManagerDialog::StyleSheetManagerDialog
(
    const QStringList& initialStyleSheets,
    QWidget* parent
) : QDialog(parent)
{
    this->setWindowTitle(tr("Custom Style Sheets"));

    QGridLayout* layout = new QGridLayout();

    styleSheets = initialStyleSheets;
    styleSheets.sort();

    styleSheetListWidget = new QListWidget(this);
    styleSheetListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Populate the list widget.
    foreach (QString styleSheetPath, styleSheets)
    {
        styleSheetListWidget->addItem(styleSheetPath);

        if (!QFileInfo(styleSheetPath).exists())
        {
            // If the style sheet file went missing, set it's font color to
            // red in the list.
            //
            styleSheetListWidget->item(styleSheetListWidget->count() - 1)->setForeground
                (QColor(Qt::red));
        }
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton* addButton = new QPushButton("+");
    addButton->setToolTip(tr("Add new style sheet"));
    QPushButton* removeButton = new QPushButton("-");
    removeButton->setToolTip(tr("Remove selected style sheet(s) from list. "
        "(No files will be deleted from the hard disk.)"));

    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(removeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);

    layout->addWidget(styleSheetListWidget, 0, 0, 1, 1);
    layout->addWidget(buttonBox, 1, 0, 1, 1);

    this->setLayout(layout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addNewStyleSheet()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeSelectedStyleSheet()));
}

StyleSheetManagerDialog::~StyleSheetManagerDialog()
{

}

QStringList StyleSheetManagerDialog::getStyleSheets() const
{
    return styleSheets;
}

void StyleSheetManagerDialog::accept()
{
    this->styleSheets.sort();
    QDialog::accept();
}

void StyleSheetManagerDialog::reject()
{
    QDialog::reject();
}

void StyleSheetManagerDialog::addNewStyleSheet()
{
    QStringList filePaths =
        QFileDialog::getOpenFileNames
        (
            this,
            tr("Select CSS File"),
            QString(),
            tr("CSS") + QString(" (*.css);;") + tr("All") + QString(" (*)")
        );

    foreach (QString filePath, filePaths)
    {
        // Add each new file to the list if it isn't a duplicate.
        if (!this->styleSheets.contains(filePath))
        {
            this->styleSheets.append(filePath);
            this->styleSheetListWidget->addItem(filePath);
        }
    }
}

void StyleSheetManagerDialog::removeSelectedStyleSheet()
{
    QList<QListWidgetItem*> selectedStyleSheets =
        styleSheetListWidget->selectedItems();

    if (selectedStyleSheets.isEmpty())
    {
        // Ignore if no style sheet is selected.
        return;
    }

    int result =
        MessageBoxHelper::question
        (
            this,
            tr("Don't worry! No files will be deleted from the hard disk. "
                "But are you sure you wish to remove the selected style "
                "sheet(s) from the list?"),
            QString(),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (QMessageBox::Yes == result)
    {
        for (int i = 0; i < selectedStyleSheets.size(); i++)
        {
            styleSheets.removeAll(selectedStyleSheets[i]->text());

            int row = styleSheetListWidget->row(selectedStyleSheets[i]);
            QListWidgetItem* item = styleSheetListWidget->takeItem(row);

            if (NULL != item)
            {
                delete item;
                item = NULL;
            }
        }
    }
}
