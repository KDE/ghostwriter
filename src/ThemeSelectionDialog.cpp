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

#include <QStringList>
#include <QListWidget>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QList>
#include <QString>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QPainter>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QBrush>
#include <QColor>
#include <QSize>

#include "ThemeSelectionDialog.h"
#include "ThemeFactory.h"
#include "ThemePreviewer.h"
#include "ThemeEditorDialog.h"
#include "MessageBoxHelper.h"

ThemeSelectionDialog::ThemeSelectionDialog
(
    const QString& currentThemeName,
    QWidget* parent
)
    : QDialog(parent)
{
    this->setWindowTitle(tr("Themes"));
    this->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* layout = new QGridLayout();

    QStringList availableThemes = ThemeFactory::getInstance()->getAvailableThemes();
    themeListWidget = new QListWidget(this);
    themeListWidget->setIconSize(QSize(150, 100));

    // Add built-in themes to the front.
    availableThemes.insert(0, ThemeFactory::DARK_THEME_NAME);
    availableThemes.insert(0, ThemeFactory::LIGHT_THEME_NAME);

    for (int i = 0; i < availableThemes.size(); i++)
    {
        QString themeName = availableThemes[i];
        Theme theme;
        QString err;
        QIcon themeIcon;

        if (0 == i)
        {
            ThemeFactory::getInstance()->loadLightTheme(theme);
        }
        else if (1 == i)
        {
            ThemeFactory::getInstance()->loadDarkTheme(theme);
        }
        else
        {
            ThemeFactory::getInstance()->loadTheme(themeName, theme, err);
        }

        if (!err.isNull())
        {
            themeIcon = QIcon(":resources/icons/unavailable.svg");
        }
        else
        {
            ThemePreviewer previewer(theme, 100, 70);
            themeIcon = previewer.getIcon();
        }

        QListWidgetItem* item = new QListWidgetItem(themeIcon, themeName, themeListWidget);
        themeListWidget->insertItem(themeListWidget->count(), item);

        if (themeName == currentThemeName)
        {
            themeListWidget->setCurrentRow(themeListWidget->count() - 1);
            currentTheme = theme;
        }
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton* newThemeButton = new QPushButton("+");
    QPushButton* deleteThemeButton = new QPushButton("-");
    QPushButton* editThemeButton = new QPushButton(tr("Edit..."));

    buttonBox->addButton(newThemeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(deleteThemeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(editThemeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(QDialogButtonBox::Close);

    layout->addWidget(themeListWidget, 0, 0, 1, 1);
    layout->addWidget(buttonBox, 1, 0, 1, 1);

    this->setLayout(layout);

    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
    connect(newThemeButton, SIGNAL(clicked()), this, SLOT(onNewTheme()));
    connect(deleteThemeButton, SIGNAL(clicked()), this, SLOT(onDeleteTheme()));
    connect(editThemeButton, SIGNAL(clicked()), this, SLOT(onEditTheme()));
    connect(themeListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onThemeSelected()));

    currentThemeIsValid = true;
    currentThemeIsNew = false;
}

ThemeSelectionDialog::~ThemeSelectionDialog()
{
    ;
}

void ThemeSelectionDialog::onThemeSelected()
{

    QList<QListWidgetItem*> selectedThemes = themeListWidget->selectedItems();
    QString themeName;
    Theme theme;

    if (selectedThemes.isEmpty() || selectedThemes[0]->text().isNull() || selectedThemes[0]->text().isEmpty())
    {
        return;
    }

    currentThemeIsValid = true;
    themeName = selectedThemes[0]->text();

    QString err;

    if (!currentThemeIsNew)
    {
        int selectedRow = themeListWidget->row(selectedThemes[0]);

        if (0 == selectedRow)
        {
            ThemeFactory::getInstance()->loadLightTheme(theme);
        }
        else if (1 == selectedRow)
        {
            ThemeFactory::getInstance()->loadDarkTheme(theme);
        }
        else
        {
            ThemeFactory::getInstance()->loadTheme(themeName, theme, err);
        }

        currentTheme = theme;

        if (!err.isNull())
        {
            // Show an error dialog telling the user that the theme could not be loaded.
            MessageBoxHelper::critical
            (
                this,
                tr("Unable to load theme."),
                err
            );
            currentTheme.name = themeName;
            currentThemeIsValid = false;
            return;
        }
    }
    else
    {
        theme = currentTheme;
    }

    emit applyTheme(theme);
}

void ThemeSelectionDialog::onNewTheme()
{
    Theme newTheme;
    QString err = QString();

    QList<QListWidgetItem*> selectedThemes = themeListWidget->selectedItems();

    if (selectedThemes.isEmpty() || (QString("Classic Light") == selectedThemes[0]->text()))
    {
        ThemeFactory::getInstance()->loadLightTheme(newTheme);
    }
    else if (QString("Classic Dark") == selectedThemes[0]->text())
    {
        ThemeFactory::getInstance()->loadDarkTheme(newTheme);
    }
    else
    {
        ThemeFactory::getInstance()->loadTheme(selectedThemes[0]->text(), newTheme, err);

        if (!err.isNull())
        {
            ThemeFactory::getInstance()->loadLightTheme(newTheme);
            err = QString();
        }
    }

    QIcon themeIcon;

    if (!err.isNull())
    {
        themeIcon = QIcon(":resources/icons/Faenza/process-stop-symbolic.svg");
    }
    else
    {
        ThemePreviewer previewer(newTheme, 100, 70);
        themeIcon = previewer.getIcon();
    }

    newTheme.name = ThemeFactory::getInstance()->generateUntitledThemeName();
    ThemeFactory::getInstance()->saveTheme(newTheme.name, newTheme, err);

    QListWidgetItem* item = new QListWidgetItem(themeIcon, newTheme.name, themeListWidget);
    themeListWidget->insertItem(themeListWidget->count(), item);

    currentThemeIsNew = true;
    currentTheme = newTheme;
    currentThemeIsValid = true;
    this->themeListWidget->setCurrentRow(themeListWidget->count() - 1);

    ThemeEditorDialog* themeEditorDialog = new ThemeEditorDialog(newTheme, this);
    connect(themeEditorDialog, SIGNAL(themeUpdated(Theme)), this, SLOT(onThemeUpdated(Theme)));
    themeEditorDialog->exec();

    if (NULL != themeEditorDialog)
    {
        delete themeEditorDialog;
    }
}


void ThemeSelectionDialog::onDeleteTheme()
{
    QList<QListWidgetItem*> selectedThemes = themeListWidget->selectedItems();

    if (selectedThemes.isEmpty())
    {
        // Ignore if no theme is selected.
        return;
    }

    int selectedRow = themeListWidget->row(selectedThemes[0]);

    // If the theme is one of the built in themes (which are always at the
    // front of the list), display an error.
    //
    if ((0 == selectedRow) || (1 == selectedRow))
    {
        MessageBoxHelper::critical
        (
            this,
            tr("Cannot delete theme."),
            tr("Sorry, this is a built-in theme that cannot be deleted.")
        );
        return;
    }

    QString themeName = selectedThemes[0]->text();

    int result =
        MessageBoxHelper::question
        (
            this,
            tr("Are you sure you want to permanently delete the '%1' theme?")
                .arg(themeName),
            QString(),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (QMessageBox::Yes == result)
    {
        QString err;

        ThemeFactory::getInstance()->deleteTheme(themeName, err);

        if (!err.isNull())
        {
            MessageBoxHelper::critical
            (
                this,
                tr("Failed to delete theme."),
                err
            );
        }
        else
        {
            int row = themeListWidget->row(selectedThemes[0]);
            QListWidgetItem* item = themeListWidget->takeItem(row);

            if (NULL != item)
            {
                delete item;
                item = NULL;
            }
        }
    }
}

void ThemeSelectionDialog::onEditTheme()
{
    QList<QListWidgetItem*> selectedThemes = themeListWidget->selectedItems();

    if (!selectedThemes.isEmpty())
    {
        int selectedRow = themeListWidget->row(selectedThemes[0]);

        // If the theme is one of the built in themes (which are always at the
        // front of the list), display an error.
        //
        if ((0 == selectedRow) || (1 == selectedRow))
        {
            MessageBoxHelper::critical
            (
                this,
                tr("Cannot edit theme."),
                tr("Sorry, this is a built-in theme that cannot be edited.")
            );
            return;
        }

        Theme themeToEdit = currentTheme;

        if (!currentThemeIsValid)
        {
            ThemeFactory::getInstance()->loadLightTheme(themeToEdit);
            themeToEdit.name = currentTheme.name;
        }

        ThemeEditorDialog* themeEditorDialog = new ThemeEditorDialog(themeToEdit, this);
        connect(themeEditorDialog, SIGNAL(themeUpdated(Theme)), this, SLOT(onThemeUpdated(Theme)));

        themeEditorDialog->exec();

        if (NULL != themeEditorDialog)
        {
            delete themeEditorDialog;
        }
    }
}

void ThemeSelectionDialog::onThemeUpdated(const Theme& theme)
{
    // Update theme name and preview icon, as applicable.
    //
    QList<QListWidgetItem*> selectedThemes = themeListWidget->selectedItems();

    if (!selectedThemes.isEmpty())
    {
        QListWidgetItem* item = selectedThemes[0];

        if (NULL != item)
        {
            if (theme.name != currentTheme.name)
            {
                item->setText(theme.name);
            }

            currentTheme = theme;
            currentThemeIsValid = true;

            ThemePreviewer previewer(theme, 100, 70);
            QIcon themeIcon = previewer.getIcon();

            item->setIcon(themeIcon);
        }
    }

    currentThemeIsNew = false;

    // Notify of theme change so it can be applied.
    emit applyTheme(theme);
}
