/***********************************************************************
 *
 * Copyright (C) 2014-2017 wereturtle
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
#include <QVBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

#include "ThemeEditorDialog.h"
#include "Theme.h"
#include "ThemeFactory.h"
#include "image_button.h"
#include "color_button.h"
#include "MessageBoxHelper.h"

ThemeEditorDialog::ThemeEditorDialog(const Theme& theme, QWidget* parent)
    : QDialog(parent), theme(theme)
{
    qreal dpr = 1.0;

#if QT_VERSION >= 0x050600
    dpr = devicePixelRatioF();
#endif

    this->setWindowTitle(tr("Edit Theme"));
    oldThemeName = theme.getName();
    themeNameEdit = new QLineEdit(this);
    themeNameEdit->setText(theme.getName());

    textColorButton = new ColorButton(this);
    textColorButton->setColor(theme.getDefaultTextColor());

    markupColorButton = new ColorButton(this);
    markupColorButton->setColor(theme.getMarkupColor());

    linkColorButton = new ColorButton(this);
    linkColorButton->setColor(theme.getLinkColor());

    emphasisColorButton = new ColorButton(this);
    emphasisColorButton->setColor(theme.getEmphasisColor());

    blockTextColorButton = new ColorButton(this);
    blockTextColorButton->setColor(theme.getBlockquoteColor());

    spellcheckColorButton = new ColorButton(this);
    spellcheckColorButton->setColor(theme.getSpellingErrorColor());

    backgroundColorButton = new ColorButton(this);
    backgroundColorButton->setColor(theme.getBackgroundColor());

    editorBackgroundColorButton = new ColorButton(this);
    QColor editorBgColorNoAlpha = theme.getEditorBackgroundColor();
    editorBgColorNoAlpha.setAlpha(255);
    editorBackgroundColorButton->setColor(editorBgColorNoAlpha);

    hudForegroundColorButton = new ColorButton(this);
    hudForegroundColorButton->setColor(theme.getHudForegroundColor());
    hudBackgroundColorButton = new ColorButton(this);
    hudBackgroundColorButton->setColor(theme.getHudBackgroundColor());

    backgroundImageButton = new ImageButton(dpr, this);

    if
    (
        (PictureAspectNone != theme.getBackgroundImageAspect())
        && !theme.getBackgroundImageUrl().isNull()
        && !theme.getBackgroundImageUrl().isEmpty()
    )
    {
        QFileInfo bgImgInfo(theme.getBackgroundImageUrl());
        QDir bgImgParentDir = bgImgInfo.dir();
        bgImgParentDir.cdUp();

        if
        (
            (bgImgInfo.dir() == ThemeFactory::getInstance()->getDirectoryForTheme(theme.getName()))
            || (ThemeFactory::getInstance()->getThemeDirectory() == bgImgParentDir)
        )
        {
            backgroundImageButton->setImage(theme.getBackgroundImageUrl(), QString());
        }
        else
        {
            backgroundImageButton->setImage(theme.getBackgroundImageUrl(), theme.getBackgroundImageUrl());
        }
    }

    cornersComboBox = new QComboBox(this);
    cornersComboBox->addItem(tr("Rounded"), QVariant(EditorCornersRounded));
    cornersComboBox->addItem(tr("Square"), QVariant(EditorCornersSquare));
    cornersComboBox->setCurrentIndex((int) theme.getEditorCorners());

    editorAspectComboBox = new QComboBox(this);
    editorAspectComboBox->addItem(tr("Stretch"), QVariant(EditorAspectStretch));
    editorAspectComboBox->addItem(tr("Center"), QVariant(EditorAspectCenter));
    editorAspectComboBox->setCurrentIndex((int) theme.getEditorAspect());

    pictureAspectComboBox = new QComboBox(this);
    pictureAspectComboBox->addItem(tr("None"), QVariant(PictureAspectNone));
    pictureAspectComboBox->addItem(tr("Tile"), QVariant(PictureAspectTile));
    pictureAspectComboBox->addItem(tr("Center"), QVariant(PictureAspectCenter));
    pictureAspectComboBox->addItem(tr("Stretch"), QVariant(PictureAspectStretch));
    pictureAspectComboBox->addItem(tr("Scale"), QVariant(PictureAspectScale));
    pictureAspectComboBox->addItem(tr("Zoom"), QVariant(PictureAspectZoom));
    pictureAspectComboBox->setCurrentIndex((int) theme.getBackgroundImageAspect());

    editorOpacitySlider = new QSlider(Qt::Horizontal, this);
    editorOpacitySlider->setMinimum(0);
    editorOpacitySlider->setMaximum(255);
    editorOpacitySlider->setValue(theme.getEditorBackgroundColor().alpha());

    QTabWidget* tabWidget = new QTabWidget(this);

    QFormLayout* nameLayout = new QFormLayout();
    nameLayout->addRow(tr("Theme Name"), themeNameEdit);

    QWidget* nameSettingsWidget = new QWidget();
    nameSettingsWidget->setLayout(nameLayout);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(nameSettingsWidget);
    layout->addWidget(tabWidget);
    this->setLayout(layout);

    QFormLayout* editorColorsLayout = new QFormLayout();
    editorColorsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    editorColorsLayout->addRow(tr("Text Color"), textColorButton);
    editorColorsLayout->addRow(tr("Markup Color"), markupColorButton);
    editorColorsLayout->addRow(tr("Link Color"), linkColorButton);
    editorColorsLayout->addRow(tr("Emphasis Color"), emphasisColorButton);
    editorColorsLayout->addRow(tr("Block Text Color"), blockTextColorButton);
    editorColorsLayout->addRow(tr("Spelling Error Color"), spellcheckColorButton);
    editorColorsLayout->addRow(tr("Text Area Background Color"), editorBackgroundColorButton);

    QWidget* tab = new QWidget();
    tabWidget->addTab(tab, tr("Text Editor"));
    tab->setLayout(editorColorsLayout);

    QFormLayout* backgroundSettingsLayout = new QFormLayout();
    backgroundSettingsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    backgroundSettingsLayout->addRow(tr("Background Image"), pictureAspectComboBox);
    backgroundSettingsLayout->addRow(tr("Background Color"), backgroundColorButton);
    backgroundSettingsLayout->addWidget(backgroundImageButton);
    backgroundSettingsLayout->addRow(tr("Editor Aspect"), editorAspectComboBox);
    backgroundSettingsLayout->addRow(tr("Editor Corners"), cornersComboBox);
    backgroundSettingsLayout->addRow(tr("Editor Opacity"), editorOpacitySlider);

    tab = new QWidget();
    tabWidget->addTab(tab, tr("Background"));
    tab->setLayout(backgroundSettingsLayout);

    QFormLayout* hudSettingsLayout = new QFormLayout();
    hudSettingsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    hudSettingsLayout->addRow(tr("HUD Foreground Color"), hudForegroundColorButton);
    hudSettingsLayout->addRow(tr("HUD Background Color"), hudBackgroundColorButton);

    tab = new QWidget();
    tabWidget->addTab(tab, tr("HUD"));
    tab->setLayout(hudSettingsLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Apply);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));

}

ThemeEditorDialog::~ThemeEditorDialog()
{
    ;
}

void ThemeEditorDialog::accept()
{
    if (saveTheme())
    {
        QDialog::accept();
    }
}

void ThemeEditorDialog::reject()
{
    emit themeUpdated(theme);
    QDialog::reject();
}

void ThemeEditorDialog::apply()
{
    if (saveTheme())
    {
        oldThemeName = theme.getName();
    }
}

bool ThemeEditorDialog::saveTheme()
{
    theme.setName(themeNameEdit->text());
    theme.setDefaultTextColor(textColorButton->color());
    theme.setMarkupColor(markupColorButton->color());
    theme.setLinkColor(linkColorButton->color());
    theme.setHeadingColor(emphasisColorButton->color());
    theme.setEmphasisColor(emphasisColorButton->color());
    theme.setBlockquoteColor(blockTextColorButton->color());
    theme.setCodeColor(blockTextColorButton->color());
    theme.setSpellingErrorColor(spellcheckColorButton->color());
    theme.setBackgroundColor(backgroundColorButton->color());
    theme.setBackgroundImageAspect((PictureAspect) pictureAspectComboBox->currentIndex());

    QColor editorBgColor = editorBackgroundColorButton->color();
    editorBgColor.setAlpha(editorOpacitySlider->value());
    theme.setEditorBackgroundColor(editorBgColor);
    theme.setEditorAspect((EditorAspect) editorAspectComboBox->currentIndex());
    theme.setEditorCorners((EditorCorners) cornersComboBox->currentIndex());

    if (PictureAspectNone == theme.getBackgroundImageAspect())
    {
        theme.setBackgroundImageUrl("NA");
    }
    else
    {
        theme.setBackgroundImageUrl(backgroundImageButton->image());
    }

    theme.setHudForegroundColor(hudForegroundColorButton->color());
    theme.setHudBackgroundColor(hudBackgroundColorButton->color());

    QString error;

    ThemeFactory::getInstance()->saveTheme(oldThemeName, theme, error);

    if (!error.isNull())
    {
        MessageBoxHelper::critical
        (
            this,
            tr("Unable to save theme."),
            error
        );
        return false;
    }

    // If the theme was renamed, delete the old theme file and its
    // background image (if any).
    //
    if (oldThemeName != theme.getName())
    {
        ThemeFactory::getInstance()->deleteTheme(oldThemeName, error);

        if (!error.isNull())
        {
            MessageBoxHelper::critical
            (
                this,
                tr("Failed to rename theme."),
                error
            );
            return false;
        }
    }

    emit themeUpdated(theme);

    return true;
}
