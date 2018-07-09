/***********************************************************************
 *
 * Copyright (C) 2016 wereturtle
 * Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#include <QTabWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

#include "PreferencesDialog.h"
#include "AppSettings.h"
#include "LocaleDialog.h"
#include "MessageBoxHelper.h"
#include "dictionary_manager.h"

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    this->setWindowTitle(tr("Preferences"));
    appSettings = AppSettings::getInstance();

    QTabWidget* tabWidget = new QTabWidget(this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(tabWidget);
    this->setLayout(layout);

    tabWidget->addTab(initializeGeneralTab(), tr("General"));
    tabWidget->addTab(initializeEditorTab(), tr("Editor"));
    tabWidget->addTab(initializeSpellCheckTab(), tr("Spell Check"));
    tabWidget->addTab(initializeHudTab(), tr("HUD"));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);

    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));

    tabWidget->setCurrentIndex(0);
}

PreferencesDialog::~PreferencesDialog()
{

}

void PreferencesDialog::onFocusModeChanged(int index)
{
    QComboBox* combo = (QComboBox*) sender();
    appSettings->setFocusMode((FocusMode) combo->itemData(index).toInt());
}

void PreferencesDialog::onEditorWidthChanged(int index)
{
    QComboBox* combo = (QComboBox*) sender();
    appSettings->setEditorWidth((EditorWidth) combo->itemData(index).toInt());
}

void PreferencesDialog::onInterfaceStyleChanged(int index)
{
    QComboBox* combo = (QComboBox*) sender();
    appSettings->setInterfaceStyle((InterfaceStyle) combo->itemData(index).toInt());
}

void PreferencesDialog::onBlockquoteStyleChanged(int index)
{
    QComboBox* combo = (QComboBox*) sender();
    appSettings->setBlockquoteStyle((BlockquoteStyle) combo->itemData(index).toInt());
}

void PreferencesDialog::onEmphasisStyleChanged(int index)
{
    QComboBox* combo = (QComboBox*) sender();
    appSettings->setUseUnderlineForEmphasis(combo->itemData(index).toBool());
}

void PreferencesDialog::onDictionaryChanged(int index)
{
    QComboBox* combo = (QComboBox*) sender();
    QString language = combo->itemData(index).toString();

    DictionaryManager::instance().setDefaultLanguage(language);
    appSettings->setDictionaryLanguage(language);
}

void PreferencesDialog::onHudWindowButtonLayoutChanged(int index)
{
    QComboBox* combo = (QComboBox*) sender();
    appSettings->setHudButtonLayout((HudWindowButtonLayout) combo->itemData(index).toInt());
}

void PreferencesDialog::showAutoMatchFilterDialog()
{
    QDialog autoMatchFilterDialog(this);
    autoMatchFilterDialog.setWindowTitle(tr("Matched Characters"));

    QGridLayout* layout = new QGridLayout();

    QCheckBox* autoMatchDoubleQuotesCheckbox = new QCheckBox("\"");
    autoMatchDoubleQuotesCheckbox->setCheckable(true);
    autoMatchDoubleQuotesCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('\"'));
    layout->addWidget(autoMatchDoubleQuotesCheckbox, 0, 0);

    QCheckBox* autoMatchSingleQuotesCheckbox = new QCheckBox("\'");
    autoMatchSingleQuotesCheckbox->setCheckable(true);
    autoMatchSingleQuotesCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('\''));
    layout->addWidget(autoMatchSingleQuotesCheckbox, 1, 0);

    QCheckBox* autoMatchParenthesesCheckbox = new QCheckBox("( )");
    autoMatchParenthesesCheckbox->setCheckable(true);
    autoMatchParenthesesCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('('));
    layout->addWidget(autoMatchParenthesesCheckbox, 2, 0);

    QCheckBox* autoMatchSquareBracketsCheckbox = new QCheckBox("[ ]");
    autoMatchSquareBracketsCheckbox->setCheckable(true);
    autoMatchSquareBracketsCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('['));
    layout->addWidget(autoMatchSquareBracketsCheckbox, 0, 1);

    QCheckBox* autoMatchBracesCheckbox = new QCheckBox("{ }");
    autoMatchBracesCheckbox->setCheckable(true);
    autoMatchBracesCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('{'));
    layout->addWidget(autoMatchBracesCheckbox, 1, 1);

    QCheckBox* autoMatchAsterisksCheckbox = new QCheckBox("*");
    autoMatchAsterisksCheckbox->setCheckable(true);
    autoMatchAsterisksCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('*'));
    layout->addWidget(autoMatchAsterisksCheckbox, 2, 1);

    QCheckBox* autoMatchUnderscoresCheckbox = new QCheckBox("_");
    autoMatchUnderscoresCheckbox->setCheckable(true);
    autoMatchUnderscoresCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('_'));
    layout->addWidget(autoMatchUnderscoresCheckbox, 0, 2);

    QCheckBox* autoMatchBackticksCheckbox = new QCheckBox("`");
    autoMatchBackticksCheckbox->setCheckable(true);
    autoMatchBackticksCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('`'));
    layout->addWidget(autoMatchBackticksCheckbox, 1, 2);

    QCheckBox* autoMatchAngleBracketsCheckbox = new QCheckBox("< >");
    autoMatchAngleBracketsCheckbox->setCheckable(true);
    autoMatchAngleBracketsCheckbox->setChecked(appSettings->getAutoMatchCharEnabled('<'));
    layout->addWidget(autoMatchAngleBracketsCheckbox, 2, 2);

    autoMatchFilterDialog.setLayout(layout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox, 3, 0, 1, 3);

    connect(buttonBox, SIGNAL(accepted()), &autoMatchFilterDialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &autoMatchFilterDialog, SLOT(reject()));

    int status = autoMatchFilterDialog.exec();

    if (QDialog::Accepted == status)
    {
        appSettings->setAutoMatchCharEnabled('\"', autoMatchDoubleQuotesCheckbox->isChecked());
        appSettings->setAutoMatchCharEnabled('\'', autoMatchSingleQuotesCheckbox->isChecked());
        appSettings->setAutoMatchCharEnabled('(', autoMatchParenthesesCheckbox->isChecked());
        appSettings->setAutoMatchCharEnabled('[', autoMatchSquareBracketsCheckbox->isChecked());
        appSettings->setAutoMatchCharEnabled('{', autoMatchBracesCheckbox->isChecked());
        appSettings->setAutoMatchCharEnabled('*', autoMatchAsterisksCheckbox->isChecked());
        appSettings->setAutoMatchCharEnabled('_', autoMatchUnderscoresCheckbox->isChecked());
        appSettings->setAutoMatchCharEnabled('`', autoMatchBackticksCheckbox->isChecked());
        appSettings->setAutoMatchCharEnabled('<', autoMatchAngleBracketsCheckbox->isChecked());
    }
}

QWidget* PreferencesDialog::initializeGeneralTab()
{
    QWidget* tab = new QWidget();

    QVBoxLayout* tabLayout = new QVBoxLayout();
    tab->setLayout(tabLayout);

    QGroupBox* displayGroupBox = new QGroupBox(tr("Display"));
    tabLayout->addWidget(displayGroupBox);

    QFormLayout* displayGroupLayout = new QFormLayout();
    displayGroupBox->setLayout(displayGroupLayout);

    QCheckBox* clockCheckBox = new QCheckBox(tr("Show current time in full screen mode"));
    clockCheckBox->setCheckable(true);
    clockCheckBox->setChecked(appSettings->getDisplayTimeInFullScreenEnabled());
    connect(clockCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setDisplayTimeInFullScreenEnabled(bool)));
    displayGroupLayout->addRow(clockCheckBox);

    QCheckBox* menuBarCheckBox = new QCheckBox(tr("Hide menu bar in full screen mode"));
    menuBarCheckBox->setCheckable(true);
    menuBarCheckBox->setChecked(appSettings->getHideMenuBarInFullScreenEnabled());
    connect(menuBarCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setHideMenuBarInFullScreenEnabled(bool)));
    displayGroupLayout->addRow(menuBarCheckBox);

    QComboBox* cornersComboBox = new QComboBox(this);
    cornersComboBox->addItem(tr("Rounded"), QVariant(InterfaceStyleRounded));
    cornersComboBox->addItem(tr("Square"), QVariant(InterfaceStyleSquare));
    cornersComboBox->setCurrentIndex((int) appSettings->getInterfaceStyle());

    connect(cornersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onInterfaceStyleChanged(int)));
    displayGroupLayout->addRow(tr("Interface style"), cornersComboBox);

    QGroupBox* savingGroupBox = new QGroupBox(tr("File Saving"));
    tabLayout->addWidget(savingGroupBox);

    QFormLayout* savingGroupLayout = new QFormLayout();
    savingGroupBox->setLayout(savingGroupLayout);

    QCheckBox* autoSaveCheckBox = new QCheckBox(tr("Auto save"));
    autoSaveCheckBox->setCheckable(true);
    autoSaveCheckBox->setChecked(appSettings->getAutoSaveEnabled());
    connect(autoSaveCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setAutoSaveEnabled(bool)));
    savingGroupLayout->addRow(autoSaveCheckBox);

    QCheckBox* backupCheckBox = new QCheckBox(tr("Backup file on save"));
    backupCheckBox->setCheckable(true);
    backupCheckBox->setChecked(appSettings->getBackupFileEnabled());
    connect(backupCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setBackupFileEnabled(bool)));
    savingGroupLayout->addRow(backupCheckBox);

    QGroupBox* historyGroupBox = new QGroupBox(tr("History"));
    tabLayout->addWidget(historyGroupBox);

    QFormLayout* historyGroupLayout = new QFormLayout();
    historyGroupBox->setLayout(historyGroupLayout);

    QCheckBox* rememberHistoryCheckBox = new QCheckBox(tr("Remember file history"));
    rememberHistoryCheckBox->setCheckable(true);
    rememberHistoryCheckBox->setChecked(appSettings->getFileHistoryEnabled());
    connect(rememberHistoryCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setFileHistoryEnabled(bool)));
    historyGroupLayout->addRow(rememberHistoryCheckBox);

    return tab;
}

QWidget* PreferencesDialog::initializeEditorTab()
{
    QWidget* tab = new QWidget();

    QVBoxLayout* tabLayout = new QVBoxLayout();
    tab->setLayout(tabLayout);

    QGroupBox* tabsGroupBox = new QGroupBox(tr("Tabulation"));
    tabLayout->addWidget(tabsGroupBox);

    QFormLayout* tabsGroupLayout = new QFormLayout();
    tabsGroupBox->setLayout(tabsGroupLayout);

    QCheckBox* spacesCheckBox = new QCheckBox(tr("Insert spaces for tabs"));
    spacesCheckBox->setCheckable(true);
    spacesCheckBox->setChecked(appSettings->getInsertSpacesForTabsEnabled());
    connect(spacesCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setInsertSpacesForTabsEnabled(bool)));
    tabsGroupLayout->addRow(spacesCheckBox);

    QSpinBox* tabWidthInput = new QSpinBox();

    tabWidthInput->setRange
    (
        appSettings->MIN_TAB_WIDTH,
        appSettings->MAX_TAB_WIDTH
    );

    tabWidthInput->setValue(appSettings->getTabWidth());
    connect(tabWidthInput, SIGNAL(valueChanged(int)), appSettings, SLOT(setTabWidth(int)));
    tabsGroupLayout->addRow(tr("Tabulation width"), tabWidthInput);

    QGroupBox* stylingGroupBox = new QGroupBox(tr("Styling"));
    tabLayout->addWidget(stylingGroupBox);

    QFormLayout* stylingGroupLayout = new QFormLayout();
    stylingGroupBox->setLayout(stylingGroupLayout);

    QComboBox* focusModeCombo = new QComboBox();
    focusModeCombo->addItem(tr("Sentence"), FocusModeSentence);
    focusModeCombo->addItem(tr("Current Line"), FocusModeCurrentLine);
    focusModeCombo->addItem(tr("Three Lines"), FocusModeThreeLines);
    focusModeCombo->addItem(tr("Paragraph"), FocusModeParagraph);
    focusModeCombo->addItem(tr("Typewriter"), FocusModeTypewriter);
    focusModeCombo->setCurrentIndex(appSettings->getFocusMode() - 1);

    connect(focusModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onFocusModeChanged(int)));
    stylingGroupLayout->addRow(tr("Focus mode"), focusModeCombo);

    QComboBox* editorWidthCombo = new QComboBox();
    editorWidthCombo->addItem(tr("Narrow"), EditorWidthNarrow);
    editorWidthCombo->addItem(tr("Medium"), EditorWidthMedium);
    editorWidthCombo->addItem(tr("Wide"), EditorWidthWide);
    editorWidthCombo->addItem(tr("Full"), EditorWidthFull);
    editorWidthCombo->setCurrentIndex(appSettings->getEditorWidth());

    connect(editorWidthCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorWidthChanged(int)));
    stylingGroupLayout->addRow(tr("Editor width"), editorWidthCombo);

    QComboBox* blockQuoteCombo = new QComboBox();
    blockQuoteCombo->addItem(tr("Plain"), BlockquoteStylePlain);
    blockQuoteCombo->addItem(tr("Italic"), BlockquoteStyleItalic);
    blockQuoteCombo->addItem(tr("Fancy"), BlockquoteStyleFancy);
    blockQuoteCombo->setCurrentIndex(appSettings->getBlockquoteStyle());

    connect(blockQuoteCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onBlockquoteStyleChanged(int)));
    stylingGroupLayout->addRow(tr("Blockquote style"), blockQuoteCombo);

    QComboBox* underlineCombo = new QComboBox();
    underlineCombo->addItem(tr("Italic"), false);
    underlineCombo->addItem(tr("Underline"), true);
    underlineCombo->setCurrentIndex(appSettings->getUseUnderlineForEmphasis() ? 1 : 0);

    connect(underlineCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onEmphasisStyleChanged(int)));
    stylingGroupLayout->addRow(tr("Emphasis style"), underlineCombo);

    QCheckBox* largeHeadingsCheckBox = new QCheckBox(tr("Use large headings"));
    largeHeadingsCheckBox->setCheckable(true);
    largeHeadingsCheckBox->setChecked(appSettings->getLargeHeadingSizesEnabled());
    connect(largeHeadingsCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setLargeHeadingSizesEnabled(bool)));
    stylingGroupLayout->addRow(largeHeadingsCheckBox);

    QCheckBox* lineBreaksCheckBox = new QCheckBox(tr("Highlight line breaks"));
    lineBreaksCheckBox->setCheckable(true);
    lineBreaksCheckBox->setChecked(appSettings->getHighlightLineBreaks());
    connect(lineBreaksCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setHighlightLineBreaks(bool)));
    stylingGroupLayout->addRow(lineBreaksCheckBox);

    QGroupBox* typingGroupBox = new QGroupBox(tr("Typing"));
    tabLayout->addWidget(typingGroupBox);

    QFormLayout* typingGroupLayout = new QFormLayout();
    typingGroupBox->setLayout(typingGroupLayout);

    QCheckBox* cycleBulletPointsCheckBox = new QCheckBox(tr("Cycle bullet point markers"));
    cycleBulletPointsCheckBox->setCheckable(true);
    cycleBulletPointsCheckBox->setChecked(appSettings->getBulletPointCyclingEnabled());
    connect(cycleBulletPointsCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setBulletPointCyclingEnabled(bool)));
    typingGroupLayout->addRow(cycleBulletPointsCheckBox);

    QCheckBox* autoMatchCheckBox = new QCheckBox(tr("Automatically match characters"));
    autoMatchCheckBox->setCheckable(true);
    autoMatchCheckBox->setChecked(appSettings->getAutoMatchEnabled());
    connect(autoMatchCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setAutoMatchEnabled(bool)));
    typingGroupLayout->addRow(autoMatchCheckBox);

    QPushButton* matchedCharsButton = new QPushButton(tr("Customize matched characters..."));
    connect(matchedCharsButton, SIGNAL(pressed()), this, SLOT(showAutoMatchFilterDialog()));
    typingGroupLayout->addRow(matchedCharsButton);

    return tab;
}

QWidget* PreferencesDialog::initializeSpellCheckTab()
{
    QWidget* tab = new QWidget();

    QVBoxLayout* tabLayout = new QVBoxLayout();
    tab->setLayout(tabLayout);

    QCheckBox* spellcheckCheckBox = new QCheckBox(tr("Live spellcheck enabled"));
    spellcheckCheckBox->setCheckable(true);
    spellcheckCheckBox->setChecked(appSettings->getLiveSpellCheckEnabled());
    connect(spellcheckCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setLiveSpellCheckEnabled(bool)));
    tabLayout->addWidget(spellcheckCheckBox);

    QGroupBox* languageGroupBox = new QGroupBox(tr("Language"));
    tabLayout->addWidget(languageGroupBox);

    QFormLayout* languageGroupLayout = new QFormLayout();
    languageGroupBox->setLayout(languageGroupLayout);

    QComboBox* dictionaryComboBox = new QComboBox();

    QStringList languages = DictionaryManager::instance().availableDictionaries();
    languages.sort();

    int currentDictionaryIndex = 0;

    for (int i = 0; i < languages.length(); i++)
    {
        QString language = languages[i];
        dictionaryComboBox->addItem(languageName(language), language);

        if (appSettings->getDictionaryLanguage() == language)
        {
            currentDictionaryIndex = i;
        }
    }

    dictionaryComboBox->setCurrentIndex(currentDictionaryIndex);
    connect(dictionaryComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDictionaryChanged(int)));
    languageGroupLayout->addRow(tr("Dictionary"), dictionaryComboBox);

    return tab;
}

QWidget* PreferencesDialog::initializeHudTab()
{
    QWidget* tab = new QWidget();

    QFormLayout* tabLayout = new QFormLayout();
    tab->setLayout(tabLayout);

    QComboBox* buttonLayoutCombo = new QComboBox();
    buttonLayoutCombo->addItem(tr("Right"), HudWindowButtonLayoutRight);
    buttonLayoutCombo->addItem(tr("Left"), HudWindowButtonLayoutLeft);
    buttonLayoutCombo->setCurrentIndex((int) appSettings->getHudButtonLayout());

    connect(buttonLayoutCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onHudWindowButtonLayoutChanged(int)));
    tabLayout->addRow(tr("Window button layout"), buttonLayoutCombo);

    QCheckBox* alternateRowColorsCheckBox = new QCheckBox(tr("Alternate row colors"));
    alternateRowColorsCheckBox->setCheckable(true);
    alternateRowColorsCheckBox->setChecked(appSettings->getAlternateHudRowColorsEnabled());
    connect(alternateRowColorsCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setAlternateHudRowColorsEnabled(bool)));
    tabLayout->addRow(alternateRowColorsCheckBox);

    QCheckBox* compositingCheckBox = new QCheckBox(tr("Enable desktop compositing effects"));
    compositingCheckBox->setCheckable(true);
    compositingCheckBox->setChecked(appSettings->getDesktopCompositingEnabled());
    connect(compositingCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setDesktopCompositingEnabled(bool)));
    tabLayout->addRow(compositingCheckBox);

    QCheckBox* hideHudsWhenTypingCheckBox = new QCheckBox(tr("Auto-hide HUD windows when typing"));
    hideHudsWhenTypingCheckBox->setCheckable(true);
    hideHudsWhenTypingCheckBox->setChecked(appSettings->getHideHudsWhenTypingEnabled());
    connect(hideHudsWhenTypingCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setHideHudsWhenTypingEnabled(bool)));
    tabLayout->addRow(hideHudsWhenTypingCheckBox);

    QCheckBox* hideHudsOnPreviewCheckBox = new QCheckBox(tr("Auto-hide HUD windows when previewing HTML"));
    hideHudsOnPreviewCheckBox->setCheckable(true);
    hideHudsOnPreviewCheckBox->setChecked(appSettings->getHideHudsOnPreviewEnabled());
    connect(hideHudsOnPreviewCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setHideHudsOnPreviewEnabled(bool)));
    tabLayout->addRow(hideHudsOnPreviewCheckBox);

    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(0);
    slider->setMaximum(255);
    slider->setValue(appSettings->getHudOpacity());
    connect(slider, SIGNAL(valueChanged(int)), appSettings, SLOT(setHudOpacity(int)));
    tabLayout->addRow(tr("Opacity"), slider);

    return tab;
}

// Lifted from FocusWriter
QString PreferencesDialog::languageName(const QString& language)
{
    QString lang_code = language.left(5);
    QLocale locale(lang_code);
    QString name;
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
    if (lang_code.length() > 2) {
        if (locale.name() == lang_code) {
            name = locale.nativeLanguageName() + " (" + locale.nativeCountryName() + ")";
        } else {
            name = locale.nativeLanguageName() + " (" + language + ")";
        }
    } else {
        name = locale.nativeLanguageName();
    }
    if (locale.textDirection() == Qt::RightToLeft) {
        name.prepend(QChar(0x202b));
    }
#else
    if (lang_code.length() > 2) {
        if (locale.name() == lang_code) {
            name = QLocale::languageToString(locale.language()) + " (" + QLocale::countryToString(locale.country()) + ")";
        } else {
            name = QLocale::languageToString(locale.language()) + " (" + language + ")";
        }
    } else {
        name = QLocale::languageToString(locale.language());
    }
#endif
    return name;
}
