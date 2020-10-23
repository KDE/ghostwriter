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

#include <QCheckBox>
#include <QColor>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QIcon>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QSize>
#include <QString>
#include <QStringList>

#include "3rdparty/QtAwesome/QtAwesome.h"

#include "colorschemepreviewer.h"
#include "messageboxhelper.h"
#include "themeeditordialog.h"
#include "themerepository.h"
#include "themeselectiondialog.h"

#define GW_LIST_WIDGET_ICON_WIDTH 300
#define GW_LIST_WIDGET_ICON_HEIGHT 175

namespace ghostwriter
{
class ThemeSelectionDialogPrivate
{
    Q_DECLARE_PUBLIC(ThemeSelectionDialog)

public:
    ThemeSelectionDialogPrivate(ThemeSelectionDialog *q_ptr)
        : q_ptr(q_ptr),
          currentTheme(ThemeRepository::instance()->defaultTheme())
    {
        this->awesome = new QtAwesome(q_ptr);
        this->awesome->initFontAwesome();
    }

    ~ThemeSelectionDialogPrivate()
    {
        ;
    }

    ThemeSelectionDialog *q_ptr;
    QtAwesome *awesome;

    bool darkModeEnabled;
    QListWidget *themeListWidget;
    Theme currentTheme;
    bool currentThemeIsValid;
    bool currentThemeIsNew;
    int iconWidth;
    int iconHeight;

    // Use this list to count where the custom
    // themes start in the theme selection list.
    //
    QStringList builtInThemes;

    void buildThemeList(const QString &currentThemeName = nullptr);
    void loadSelectedTheme();
    void createNewTheme();
    void deleteTheme();
    void editTheme();
    void updateTheme(const Theme &theme);
};

ThemeSelectionDialog::ThemeSelectionDialog
(
    const QString &currentThemeName,
    const bool darkModeEnabled,
    QWidget *parent
)
    : QDialog(parent),
      d_ptr(new ThemeSelectionDialogPrivate(this))
{
    qreal dpr = 1.0;

#if QT_VERSION >= 0x050600
    dpr = devicePixelRatioF();
#endif

    d_func()->iconWidth = GW_LIST_WIDGET_ICON_WIDTH * dpr;
    d_func()->iconHeight = GW_LIST_WIDGET_ICON_HEIGHT * dpr;

    d_func()->darkModeEnabled = darkModeEnabled;

    this->setWindowTitle(tr("Themes"));
    this->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout *layout = new QGridLayout();

    // Find view sizes
    int focush = style()->pixelMetric(QStyle::PM_FocusFrameHMargin);
    int focusv = style()->pixelMetric(QStyle::PM_FocusFrameVMargin);
    int frame = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int scrollbar = style()->pixelMetric(QStyle::PM_SliderThickness) +
                    style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    const int spacing = 10;
    const int rowCount = 3;
    const int columnCount = 3;
    QSize viewSize
    (
        ((d_func()->iconWidth + frame + focush + (spacing * 2)) * columnCount) + scrollbar,
        (d_func()->iconHeight + fontMetrics().height() + frame + focusv + (spacing * 2)) * rowCount
    );

    // Set up theme list
    d_func()->themeListWidget = new QListWidget(this);
    d_func()->themeListWidget->setSortingEnabled(true);
    d_func()->themeListWidget->setFlow(QListView::LeftToRight);
    d_func()->themeListWidget->setViewMode(QListView::IconMode);
    d_func()->themeListWidget->setIconSize(QSize(d_func()->iconWidth, d_func()->iconHeight));
    d_func()->themeListWidget->setSpacing(spacing);
    d_func()->themeListWidget->setMovement(QListView::Static);
    d_func()->themeListWidget->setResizeMode(QListView::Adjust);
    d_func()->themeListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    d_func()->themeListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_func()->themeListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    d_func()->themeListWidget->setWordWrap(true);
    d_func()->themeListWidget->setUniformItemSizes(true);
    d_func()->themeListWidget->setMinimumSize(viewSize);

    d_func()->currentThemeIsValid = true;
    d_func()->currentThemeIsNew = false;

    d_func()->buildThemeList(currentThemeName);

    QCheckBox *darkModeCheckbox = new QCheckBox("Preview in dark mode", this);
    darkModeCheckbox->setChecked(darkModeEnabled);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton *newThemeButton = new QPushButton(QChar(fa::plus));
    newThemeButton->setFont(d_func()->awesome->font(style::stfas, newThemeButton->font().pointSize()));
    QPushButton *deleteThemeButton = new QPushButton(QChar(fa::minus));
    deleteThemeButton->setFont(d_func()->awesome->font(style::stfas, deleteThemeButton->font().pointSize()));
    QPushButton *editThemeButton = new QPushButton(tr("Edit..."));

    buttonBox->addButton(newThemeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(deleteThemeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(editThemeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(QDialogButtonBox::Close);

    layout->addWidget(d_func()->themeListWidget, 0, 0, 1, 2);
    layout->addWidget(darkModeCheckbox, 1, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(buttonBox, 1, 1, 1, 1, Qt::AlignRight);

    this->setLayout(layout);

    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
    this->connect
    (
        newThemeButton,
        &QPushButton::clicked,
        [this]() {
            d_func()->createNewTheme();
        }
    );
    this->connect
    (
        deleteThemeButton,
        &QPushButton::clicked,
        [this]() {
            d_func()->deleteTheme();
        }
    );
    d_func()->themeListWidget;
    this->connect
    (
        darkModeCheckbox,
        &QCheckBox::toggled,
        [this](bool enabled) {
            d_func()->darkModeEnabled = enabled;
            d_func()->buildThemeList();
        }
    );
    this->connect
    (
        editThemeButton,
        &QPushButton::clicked,
        [this]() {
            d_func()->editTheme();
        }
    );
    this->connect
    (
        d_func()->themeListWidget,
        &QListWidget::itemSelectionChanged,
        [this]() {
            d_func()->loadSelectedTheme();
        }
    );
}

ThemeSelectionDialog::~ThemeSelectionDialog()
{
    ;
}

const Theme &ThemeSelectionDialog::theme() const
{
    return d_func()->currentTheme;
}

void ThemeSelectionDialogPrivate::buildThemeList(const QString &currentThemeName)
{
    qreal dpr = 1.0;

#if QT_VERSION >= 0x050600
    dpr = this->themeListWidget->devicePixelRatioF();
#endif

    QStringList availableThemes =
        ThemeRepository::instance()->availableThemes();
    QList<QListWidgetItem *> selected = this->themeListWidget->selectedItems();
    QString currentName = currentThemeName;

    if (!selected.isEmpty()) {
        currentName = selected.first()->text();
    }

    this->themeListWidget->clear();

    for (int i = 0; i < availableThemes.size(); i++) {
        QString themeName = availableThemes[i];
        QString err;
        QIcon themeIcon;

        Theme theme = ThemeRepository::instance()->loadTheme(themeName, err);

        ColorScheme colors;

        if (this->darkModeEnabled && theme.hasDarkColorScheme()) {
            colors = theme.darkColorScheme();
        } else {
            colors = theme.lightColorScheme();
        }

        ColorSchemePreviewer previewer
        (
            colors,
            theme.isReadOnly(),
            (err.isNull() || err.isEmpty()),
            GW_LIST_WIDGET_ICON_WIDTH,
            GW_LIST_WIDGET_ICON_HEIGHT,
            dpr
        );

        themeIcon = previewer.icon();

        QListWidgetItem *item = new QListWidgetItem
        (
            themeIcon,
            themeName,
            this->themeListWidget
        );

        if (!err.isNull() && !err.isEmpty()) {
            item->setToolTip(err);
        }

        this->themeListWidget->insertItem(this->themeListWidget->count(), item);

        if (themeName == currentName) {
            this->themeListWidget->setCurrentItem(item);
            this->currentTheme = theme;
            this->currentThemeIsValid = (err.isNull() || err.isEmpty());
        }
    }
}

void ThemeSelectionDialogPrivate::loadSelectedTheme()
{
    QList<QListWidgetItem *> selectedThemes = themeListWidget->selectedItems();
    QString themeName;

    if
    (
        selectedThemes.isEmpty() ||
        selectedThemes[0]->text().isNull() ||
        selectedThemes[0]->text().isEmpty()
    ) {
        return;
    }

    currentThemeIsValid = true;
    themeName = selectedThemes[0]->text();

    QString err;

    if (!currentThemeIsNew) {
        this->currentTheme = ThemeRepository::instance()->loadTheme(themeName, err);
    }
}

void ThemeSelectionDialogPrivate::createNewTheme()
{
    QString err = QString();

    QList<QListWidgetItem *> selectedThemes = themeListWidget->selectedItems();

    Theme newTheme = ThemeRepository::instance()->defaultTheme();

    if (selectedThemes.size() > 0) {
        newTheme = ThemeRepository::instance()->loadTheme
        (
            selectedThemes[0]->text(),
            err
        );
    }

    // It doesn't matter if there was an error, since loadTheme will
    // return a default, built-in theme if it fails.
    //
    err = QString();

    QIcon themeIcon;

    ColorScheme colors = newTheme.lightColorScheme();

    if (darkModeEnabled && newTheme.hasDarkColorScheme()) {
        colors = newTheme.darkColorScheme();
    }

    ColorSchemePreviewer previewer
    (
        colors,
        false,
        false,
        iconWidth,
        iconHeight
    );

    themeIcon = previewer.icon();

    newTheme.setName(ThemeRepository::instance()->generateUntitledThemeName());
    newTheme.setReadOnly(false);

    ThemeRepository::instance()->saveTheme(newTheme.name(), newTheme, err);

    QListWidgetItem *item =
        new QListWidgetItem(themeIcon, newTheme.name(), themeListWidget);
    themeListWidget->insertItem(themeListWidget->count(), item);

    currentThemeIsNew = true;
    currentTheme = newTheme;
    currentThemeIsValid = true;
    this->themeListWidget->setCurrentItem(item);

    ThemeEditorDialog *themeEditorDialog = new ThemeEditorDialog(newTheme, q_func());
    themeEditorDialog->setAttribute(Qt::WA_DeleteOnClose);

    q_func()->connect
    (
        themeEditorDialog,
        &ThemeEditorDialog::finished,
        [this, themeEditorDialog](int result) {
            updateTheme(themeEditorDialog->theme());
        }
    );

    themeEditorDialog->open();
}


void ThemeSelectionDialogPrivate::deleteTheme()
{
    QList<QListWidgetItem *> selectedThemes = themeListWidget->selectedItems();

    if (selectedThemes.isEmpty()) {
        // Ignore if no theme is selected.
        return;
    }

    // If the theme is one of the built in themes, display an error.
    //
    if (currentTheme.isReadOnly()) {
        MessageBoxHelper::critical
        (
            q_func(),
            QObject::tr("Cannot delete theme."),
            QObject::tr("Sorry, this is a built-in theme that cannot be deleted.")
        );
        return;
    }

    QString themeName = selectedThemes[0]->text();

    int result =
        MessageBoxHelper::question
        (
            q_func(),
            QObject::tr("Are you sure you want to permanently delete the '%1' theme?")
            .arg(themeName),
            QString(),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (QMessageBox::Yes == result) {
        QString err;

        ThemeRepository::instance()->deleteTheme(themeName, err);

        if (!err.isNull()) {
            MessageBoxHelper::critical
            (
                q_func(),
                QObject::tr("Failed to delete theme."),
                err
            );
        } else {
            int row = themeListWidget->row(selectedThemes[0]);
            QListWidgetItem *item = themeListWidget->takeItem(row);

            if (nullptr != item) {
                delete item;
                item = nullptr;
            }
        }
    }
}

void ThemeSelectionDialogPrivate::editTheme()
{
    QList<QListWidgetItem *> selectedThemes = themeListWidget->selectedItems();

    if (!selectedThemes.isEmpty()) {
        // If the theme is one of the built in themes, display an error.
        //
        if (currentTheme.isReadOnly()) {
            MessageBoxHelper::critical
            (
                q_func(),
                QObject::tr("Cannot edit theme."),
                QObject::tr("Sorry, this is a built-in theme that cannot be edited.")
            );
            return;
        }

        Theme themeToEdit = currentTheme;

        if (!currentThemeIsValid) {
            QString err;
            themeToEdit = ThemeRepository::instance()->loadTheme("", err);
            themeToEdit.setName(currentTheme.name());
        }

        ThemeEditorDialog *themeEditorDialog = new ThemeEditorDialog(themeToEdit, q_func());
        themeEditorDialog->setAttribute(Qt::WA_DeleteOnClose);

        q_func()->connect
        (
            themeEditorDialog,
            &ThemeEditorDialog::finished,
            [this, themeEditorDialog](int result) {
                if (QDialog::Accepted == result) {
                    updateTheme(themeEditorDialog->theme());
                }
            }
        );

        themeEditorDialog->open();
    }
}

void ThemeSelectionDialogPrivate::updateTheme(const Theme &theme)
{
    // Update theme name and preview icon, as applicable.
    //
    QList<QListWidgetItem *> selectedThemes = themeListWidget->selectedItems();

    if (!selectedThemes.isEmpty()) {
        QListWidgetItem *item = selectedThemes[0];

        if (nullptr != item) {
            if (theme.name() != currentTheme.name()) {
                item->setText(theme.name());
            }

            currentTheme = theme;
            currentThemeIsValid = true;
            ColorScheme colors;

            if (darkModeEnabled && theme.hasDarkColorScheme()) {
                colors = theme.darkColorScheme();
            } else {
                colors = theme.lightColorScheme();
            }

            ColorSchemePreviewer previewer
            (
                colors,
                theme.isReadOnly(),
                true,
                iconWidth,
                iconHeight
            );

            QIcon themeIcon = previewer.icon();
            item->setIcon(themeIcon);
        }
    }

    currentThemeIsNew = false;
}
} // namespace ghostwriter
