/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

#include <KColorButton>

#include "../settings/appsettings.h"
#include "themeeditordialog.h"
#include "theme.h"
#include "themerepository.h"
#include "messageboxhelper.h"

namespace ghostwriter
{
class ThemeEditorDialogPrivate
{
    Q_DECLARE_PUBLIC(ThemeEditorDialog)

public:
    ThemeEditorDialogPrivate
    (
        ThemeEditorDialog *q_ptr,
        const Theme &theme
    )
        : q_ptr(q_ptr),
          theme(theme),
          repo(AppSettings::instance()->themeDirectoryPath())
    {
        Q_Q(ThemeEditorDialog);
    
        this->lightColors = theme.lightColorScheme();
        this->darkColors = theme.darkColorScheme();
        this->oldThemeName = theme.name();
        this->themeNameEdit = new QLineEdit(theme.name(), q);
    }

    ~ThemeEditorDialogPrivate()
    {
        ;
    }

    ThemeEditorDialog *q_ptr;

    QLineEdit *themeNameEdit;
    Theme theme;
    ThemeRepository repo;
    ColorScheme lightColors;
    ColorScheme darkColors;
    QString oldThemeName;

    void addColorRowToLayout
    (
        QGridLayout *layout,
        const int rowIndex,
        const QString &description,
        const QVector<QColor *> &colors
    );
    void addColorsToLayout(QGridLayout *layout);
    bool saveTheme();
};

ThemeEditorDialog::ThemeEditorDialog(const Theme &theme, QWidget *parent)
    : QDialog(parent),
      d_ptr(new ThemeEditorDialogPrivate(this, theme))
{
    Q_D(ThemeEditorDialog);

    this->setWindowTitle(tr("Edit Theme"));

    QFormLayout *nameLayout = new QFormLayout();
    nameLayout->addRow(tr("Theme Name"), d->themeNameEdit);

    QWidget *nameSettingsWidget = new QWidget();
    nameSettingsWidget->setLayout(nameLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(nameSettingsWidget);
    this->setLayout(layout);

    QGridLayout *themeLayout = new QGridLayout();

    d->addColorsToLayout(themeLayout);
    layout->addLayout(themeLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}

ThemeEditorDialog::~ThemeEditorDialog()
{
    ;
}

const Theme &ThemeEditorDialog::theme() const
{
    Q_D(const ThemeEditorDialog);
    
    return d->theme;
}

void ThemeEditorDialog::accept()
{
    Q_D(ThemeEditorDialog);
    
    if (d->saveTheme()) {
        QDialog::accept();
    }
}

void ThemeEditorDialog::reject()
{
    QDialog::reject();
}

void ThemeEditorDialogPrivate::addColorRowToLayout
(
    QGridLayout *layout,
    const int rowIndex,
    const QString &description,
    const QVector<QColor *> &colors
)
{
    Q_Q(ThemeEditorDialog);
    
    layout->addWidget(new QLabel(description), rowIndex, 0);

    for (int i = 0; i < colors.size(); i++) {
        QColor *colorPtr = colors.at(i);

        if (nullptr != colorPtr) {
            KColorButton *button = new KColorButton(*colorPtr, q);
            layout->addWidget(button, rowIndex, (i + 1));

            q->connect(
                button,
                &KColorButton::changed,
                [colorPtr](const QColor & color) {
                    *colorPtr = color;
                }
            );
        }
    }
}

void ThemeEditorDialogPrivate::addColorsToLayout(QGridLayout *layout)
{
    int row = 0;

    layout->addWidget(new QLabel(ThemeEditorDialog::tr("Element")), 0, 0, 1, 1);
    layout->addWidget(new QLabel(ThemeEditorDialog::tr("Light Mode")), 0, 1, 1, 1);
    layout->addWidget(new QLabel(ThemeEditorDialog::tr("Dark Mode")), 0, 2, 1, 1);

    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Foreground"), { &lightColors.foreground,     &darkColors.foreground     });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Background"), { &lightColors.background,     &darkColors.background     });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Selection"),  { &lightColors.selection,      &darkColors.selection      });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Cursor"),     { &lightColors.cursor,         &darkColors.cursor         });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Markup"),     { &lightColors.emphasisMarkup, &darkColors.emphasisMarkup });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Accent"),     { &lightColors.listMarkup,     &darkColors.listMarkup     });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Heading"),    { &lightColors.headingText,    &darkColors.headingText    });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Emphasis"),   { &lightColors.emphasisText,   &darkColors.emphasisText   });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Block Text"), { &lightColors.blockquoteText, &darkColors.blockquoteText });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Link"),       { &lightColors.link,           &darkColors.link           });
    addColorRowToLayout(layout, ++row, ThemeEditorDialog::tr("Error"),      { &lightColors.error,          &darkColors.error          });
}

bool ThemeEditorDialogPrivate::saveTheme()
{
    Q_Q(ThemeEditorDialog);
    
    lightColors.headingMarkup = lightColors.emphasisMarkup;
    lightColors.divider = lightColors.headingMarkup;
    lightColors.image = lightColors.link;
    lightColors.inlineHtml = lightColors.emphasisMarkup;
    lightColors.blockquoteMarkup = lightColors.emphasisMarkup;
    lightColors.codeText = lightColors.blockquoteText;
    lightColors.codeMarkup = lightColors.emphasisMarkup;

    darkColors.headingMarkup = darkColors.emphasisMarkup;
    darkColors.divider = darkColors.headingMarkup;
    darkColors.image = darkColors.link;
    darkColors.inlineHtml = darkColors.emphasisMarkup;
    darkColors.blockquoteMarkup = darkColors.emphasisMarkup;
    darkColors.codeText = darkColors.blockquoteText;
    darkColors.codeMarkup = darkColors.emphasisMarkup;

    theme = Theme(themeNameEdit->text(), lightColors, darkColors, false);

    QString error;

    repo.saveTheme(oldThemeName, theme, error);

    if (!error.isNull()) {
        MessageBoxHelper::critical
        (
            q,
            ThemeEditorDialog::tr("Unable to save theme."),
            error
        );
        theme.setName(oldThemeName);
        return false;
    }

    return true;
}
} // namespace ghostwriter
