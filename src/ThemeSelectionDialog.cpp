/***********************************************************************
 *
 * Copyright (C) 2014-2018 wereturtle
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

#define GW_LIST_WIDGET_ICON_WIDTH 200
#define GW_LIST_WIDGET_ICON_HEIGHT 125

ThemeSelectionDialog::ThemeSelectionDialog
(
    const QString& currentThemeName,
    QWidget* parent
)
    : QDialog(parent)
{
    qreal dpr = 1.0;

#if QT_VERSION >= 0x050600
    dpr = devicePixelRatioF();
#endif

    iconWidth = GW_LIST_WIDGET_ICON_WIDTH * dpr;
    iconHeight = GW_LIST_WIDGET_ICON_HEIGHT * dpr;

    this->setWindowTitle(tr("Themes"));
    this->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* layout = new QGridLayout();

    QStringList availableThemes =
        ThemeFactory::getInstance()->getAvailableThemes();

    // Find view sizes
    int focush = style()->pixelMetric(QStyle::PM_FocusFrameHMargin);
    int focusv = style()->pixelMetric(QStyle::PM_FocusFrameVMargin);
    int frame = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int scrollbar = style()->pixelMetric(QStyle::PM_SliderThickness) +
            style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    const int spacing = 10;
    const int rowCount = 3;
    const int columnCount = 4;
    QSize viewSize
    (
        ((GW_LIST_WIDGET_ICON_WIDTH + frame + focush + (spacing * 2)) * columnCount) + scrollbar,
        (GW_LIST_WIDGET_ICON_HEIGHT + fontMetrics().height() + frame + focusv + (spacing * 2)) * rowCount
    );

    // Set up theme list
    themeListWidget = new QListWidget(this);
    themeListWidget->setSortingEnabled(true);
    themeListWidget->setFlow(QListView::LeftToRight);
    themeListWidget->setViewMode(QListView::IconMode);
    themeListWidget->setIconSize(QSize(GW_LIST_WIDGET_ICON_WIDTH, GW_LIST_WIDGET_ICON_HEIGHT));
    themeListWidget->setSpacing(spacing);
    themeListWidget->setMovement(QListView::Static);
    themeListWidget->setResizeMode(QListView::Adjust);
    themeListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    themeListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    themeListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    themeListWidget->setWordWrap(true);
    themeListWidget->setUniformItemSizes(true);
    themeListWidget->setMinimumSize(viewSize);

    for (int i = 0; i < availableThemes.size(); i++)
    {
        QString themeName = availableThemes[i];
        QString err;
        QIcon themeIcon;

        Theme theme = ThemeFactory::getInstance()->loadTheme(themeName, err);

        if (!err.isNull())
        {
            themeIcon = QIcon(":resources/images/unavailable.svg");
        }
        else
        {
            ThemePreviewer previewer(theme, iconWidth, iconHeight);
            themeIcon = previewer.getIcon();
        }

        QListWidgetItem* item = new QListWidgetItem
            (
                themeIcon,
                themeName,
                themeListWidget
            );
        themeListWidget->insertItem(themeListWidget->count(), item);

        if (themeName == currentThemeName)
        {
            themeListWidget->setCurrentItem(item);
            currentTheme = theme;
        }
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton* newThemeButton = new QPushButton(" + ");
    QPushButton* deleteThemeButton = new QPushButton(" - ");
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

    if
    (
        selectedThemes.isEmpty() ||
        selectedThemes[0]->text().isNull() ||
        selectedThemes[0]->text().isEmpty()
    )
    {
        return;
    }

    currentThemeIsValid = true;
    themeName = selectedThemes[0]->text();

    QString err;

    if (!currentThemeIsNew)
    {
        theme = ThemeFactory::getInstance()->loadTheme(themeName, err);
        currentTheme = theme;

        if (!err.isNull())
        {
            // Show an error dialog telling the user that the theme could not
            // be loaded.
            //
            MessageBoxHelper::critical
            (
                this,
                tr("Unable to load theme."),
                err
            );
            currentTheme.setName(themeName);
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
    QString err = QString();
    QList<QListWidgetItem*> selectedThemes = themeListWidget->selectedItems();

    Theme newTheme = ThemeFactory::getInstance()->loadTheme
        (
            selectedThemes[0]->text(),
            err
        );

    newTheme.setBuiltIn(false);

    // It doesn't matter if there was an error, since loadTheme will
    // return a default, built-in theme if it fails.
    //
    err = QString();

    QIcon themeIcon;

    if (!err.isNull())
    {
        themeIcon = QIcon(":resources/images/unavailable.svg");
    }
    else
    {
        ThemePreviewer previewer(newTheme, iconWidth, iconHeight);
        themeIcon = previewer.getIcon();
    }

    newTheme.setName(ThemeFactory::getInstance()->generateUntitledThemeName());
    ThemeFactory::getInstance()->saveTheme(newTheme.getName(), newTheme, err);

    QListWidgetItem* item =
        new QListWidgetItem(themeIcon, newTheme.getName(), themeListWidget);
    themeListWidget->insertItem(themeListWidget->count(), item);

    currentThemeIsNew = true;
    currentTheme = newTheme;
    currentThemeIsValid = true;
    this->themeListWidget->setCurrentRow(themeListWidget->count() - 1);

    ThemeEditorDialog themeEditorDialog(newTheme, this);
    connect(&themeEditorDialog, SIGNAL(themeUpdated(Theme)), this, SLOT(onThemeUpdated(Theme)));
    themeEditorDialog.exec();
}


void ThemeSelectionDialog::onDeleteTheme()
{
    QList<QListWidgetItem*> selectedThemes = themeListWidget->selectedItems();

    if (selectedThemes.isEmpty())
    {
        // Ignore if no theme is selected.
        return;
    }

    // If the theme is one of the built in themes, display an error.
    //
    if (currentTheme.isBuiltIn())
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
        // If the theme is one of the built in themes, display an error.
        //
        if (currentTheme.isBuiltIn())
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
            QString err;
            themeToEdit = ThemeFactory::getInstance()->loadTheme("", err);
            themeToEdit.setName(currentTheme.getName());
        }

        ThemeEditorDialog themeEditorDialog(themeToEdit, this);
        connect(&themeEditorDialog, SIGNAL(themeUpdated(Theme)), this, SLOT(onThemeUpdated(Theme)));

        themeEditorDialog.exec();
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
            if (theme.getName() != currentTheme.getName())
            {
                item->setText(theme.getName());
            }

            currentTheme = theme;
            currentThemeIsValid = true;

            ThemePreviewer previewer(theme, iconWidth, iconHeight);
            QIcon themeIcon = previewer.getIcon();
            item->setIcon(themeIcon);
        }
    }

    currentThemeIsNew = false;

    // Notify of theme change so it can be applied.
    emit applyTheme(theme);
}
