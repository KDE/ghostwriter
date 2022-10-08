/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QUrl>
#include <QVBoxLayout>

#include <Sonnet/ConfigWidget>

#include "appsettings.h"
#include "messageboxhelper.h"
#include "preferencesdialog.h"

namespace ghostwriter
{

class PreferencesDialogPrivate : public QObject
{
    Q_DECLARE_PUBLIC(PreferencesDialog)

public:
    PreferencesDialogPrivate(PreferencesDialog *q_ptr)
        : q_ptr(q_ptr)
    {
        ;
    }

    ~PreferencesDialogPrivate()
    {
        ;
    }

    PreferencesDialog *q_ptr;

    AppSettings *appSettings;

public:
    void showAutoMatchFilterDialog();
    QWidget *initializeGeneralTab();
    QWidget *initializeEditorTab();
    QWidget *initializeSpellCheckTab();
};

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent),
      d_ptr(new PreferencesDialogPrivate(this))
{
    Q_D(PreferencesDialog);

    this->setWindowTitle(tr("Preferences"));
    d->appSettings = AppSettings::instance();

    QTabWidget *tabWidget = new QTabWidget(this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tabWidget);
    this->setLayout(layout);

    tabWidget->addTab(d->initializeGeneralTab(), tr("General"));
    tabWidget->addTab(d->initializeEditorTab(), tr("Editor"));
    tabWidget->addTab(d->initializeSpellCheckTab(), tr("Spell Check"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);

    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));

    tabWidget->setCurrentIndex(0);
}

PreferencesDialog::~PreferencesDialog()
{
    ;
}

void PreferencesDialogPrivate::showAutoMatchFilterDialog()
{
    Q_Q(PreferencesDialog);

    QDialog autoMatchFilterDialog(q);
    autoMatchFilterDialog.setWindowTitle(PreferencesDialog::tr("Matched Characters"));

    QGridLayout *layout = new QGridLayout();

    QCheckBox *autoMatchDoubleQuotesCheckbox = new QCheckBox("\"");
    autoMatchDoubleQuotesCheckbox->setCheckable(true);
    autoMatchDoubleQuotesCheckbox->setChecked(appSettings->autoMatchCharEnabled('\"'));
    layout->addWidget(autoMatchDoubleQuotesCheckbox, 0, 0);

    QCheckBox *autoMatchSingleQuotesCheckbox = new QCheckBox("\'");
    autoMatchSingleQuotesCheckbox->setCheckable(true);
    autoMatchSingleQuotesCheckbox->setChecked(appSettings->autoMatchCharEnabled('\''));
    layout->addWidget(autoMatchSingleQuotesCheckbox, 1, 0);

    QCheckBox *autoMatchParenthesesCheckbox = new QCheckBox("( )");
    autoMatchParenthesesCheckbox->setCheckable(true);
    autoMatchParenthesesCheckbox->setChecked(appSettings->autoMatchCharEnabled('('));
    layout->addWidget(autoMatchParenthesesCheckbox, 2, 0);

    QCheckBox *autoMatchSquareBracketsCheckbox = new QCheckBox("[ ]");
    autoMatchSquareBracketsCheckbox->setCheckable(true);
    autoMatchSquareBracketsCheckbox->setChecked(appSettings->autoMatchCharEnabled('['));
    layout->addWidget(autoMatchSquareBracketsCheckbox, 0, 1);

    QCheckBox *autoMatchBracesCheckbox = new QCheckBox("{ }");
    autoMatchBracesCheckbox->setCheckable(true);
    autoMatchBracesCheckbox->setChecked(appSettings->autoMatchCharEnabled('{'));
    layout->addWidget(autoMatchBracesCheckbox, 1, 1);

    QCheckBox *autoMatchAsterisksCheckbox = new QCheckBox("*");
    autoMatchAsterisksCheckbox->setCheckable(true);
    autoMatchAsterisksCheckbox->setChecked(appSettings->autoMatchCharEnabled('*'));
    layout->addWidget(autoMatchAsterisksCheckbox, 2, 1);

    QCheckBox *autoMatchUnderscoresCheckbox = new QCheckBox("_");
    autoMatchUnderscoresCheckbox->setCheckable(true);
    autoMatchUnderscoresCheckbox->setChecked(appSettings->autoMatchCharEnabled('_'));
    layout->addWidget(autoMatchUnderscoresCheckbox, 0, 2);

    QCheckBox *autoMatchBackticksCheckbox = new QCheckBox("`");
    autoMatchBackticksCheckbox->setCheckable(true);
    autoMatchBackticksCheckbox->setChecked(appSettings->autoMatchCharEnabled('`'));
    layout->addWidget(autoMatchBackticksCheckbox, 1, 2);

    QCheckBox *autoMatchAngleBracketsCheckbox = new QCheckBox("< >");
    autoMatchAngleBracketsCheckbox->setCheckable(true);
    autoMatchAngleBracketsCheckbox->setChecked(appSettings->autoMatchCharEnabled('<'));
    layout->addWidget(autoMatchAngleBracketsCheckbox, 2, 2);

    autoMatchFilterDialog.setLayout(layout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, q);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox, 3, 0, 1, 3);

    connect(buttonBox, SIGNAL(accepted()), &autoMatchFilterDialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &autoMatchFilterDialog, SLOT(reject()));

    int status = autoMatchFilterDialog.exec();

    if (QDialog::Accepted == status) {
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

QWidget *PreferencesDialogPrivate::initializeGeneralTab()
{
    Q_Q(PreferencesDialog);

    QWidget *tab = new QWidget();

    QVBoxLayout *tabLayout = new QVBoxLayout();
    tab->setLayout(tabLayout);

    QGroupBox *displayGroupBox = new QGroupBox(PreferencesDialog::tr("Display"));
    tabLayout->addWidget(displayGroupBox);

    QFormLayout *displayGroupLayout = new QFormLayout();
    displayGroupBox->setLayout(displayGroupLayout);

    QCheckBox *clockCheckBox = new QCheckBox(PreferencesDialog::tr("Show current time in full screen mode"));
    clockCheckBox->setCheckable(true);
    clockCheckBox->setChecked(appSettings->displayTimeInFullScreenEnabled());
    connect(clockCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setDisplayTimeInFullScreenEnabled(bool)));
    displayGroupLayout->addRow(clockCheckBox);

    QCheckBox *menuBarCheckBox = new QCheckBox(PreferencesDialog::tr("Hide menu bar in full screen mode"));
    menuBarCheckBox->setCheckable(true);
    menuBarCheckBox->setChecked(appSettings->hideMenuBarInFullScreenEnabled());
    connect(menuBarCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setHideMenuBarInFullScreenEnabled(bool)));
    displayGroupLayout->addRow(menuBarCheckBox);

    QComboBox *cornersComboBox = new QComboBox(q);
    cornersComboBox->addItem(PreferencesDialog::tr("Rounded"), QVariant(InterfaceStyleRounded));
    cornersComboBox->addItem(PreferencesDialog::tr("Square"), QVariant(InterfaceStyleSquare));
    cornersComboBox->setCurrentIndex((int) appSettings->interfaceStyle());

    q->connect(
        cornersComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this, cornersComboBox](int index) {
            appSettings->setInterfaceStyle((InterfaceStyle) cornersComboBox->itemData(index).toInt());
        }
    );

    displayGroupLayout->addRow(PreferencesDialog::tr("Interface style"), cornersComboBox);

    QGroupBox *savingGroupBox = new QGroupBox(PreferencesDialog::tr("File Saving"));
    tabLayout->addWidget(savingGroupBox);

    QFormLayout *savingGroupLayout = new QFormLayout();
    savingGroupBox->setLayout(savingGroupLayout);

    QCheckBox *autoSaveCheckBox = new QCheckBox(PreferencesDialog::tr("Auto save"));
    autoSaveCheckBox->setCheckable(true);
    autoSaveCheckBox->setChecked(appSettings->autoSaveEnabled());
    connect(autoSaveCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setAutoSaveEnabled(bool)));

    savingGroupLayout->addRow(autoSaveCheckBox);

    QCheckBox *backupCheckBox = new QCheckBox(PreferencesDialog::tr("Backup file on save"));
    backupCheckBox->setCheckable(true);
    backupCheckBox->setChecked(appSettings->backupFileEnabled());
    connect(backupCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setBackupFileEnabled(bool)));
    savingGroupLayout->addRow(backupCheckBox);

    QPushButton *openDraftDirButton = new QPushButton(PreferencesDialog::tr("View untitled drafts..."));
    q->connect(
        openDraftDirButton,
        &QPushButton::clicked,
        [this]() {
            QDesktopServices::openUrl(QUrl(appSettings->draftLocation()));
        }
    );
    savingGroupLayout->addRow(openDraftDirButton);

    QGroupBox *sessionGroupBox = new QGroupBox(PreferencesDialog::tr("Session"));
    tabLayout->addWidget(sessionGroupBox);

    QFormLayout *sessionGroupLayout = new QFormLayout();
    sessionGroupBox->setLayout(sessionGroupLayout);

    QCheckBox *rememberHistoryCheckBox = new QCheckBox(PreferencesDialog::tr("Remember recent files"));
    rememberHistoryCheckBox->setCheckable(true);
    rememberHistoryCheckBox->setChecked(appSettings->fileHistoryEnabled());
    connect(rememberHistoryCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setFileHistoryEnabled(bool)));
    sessionGroupLayout->addRow(rememberHistoryCheckBox);

    QCheckBox *restoreSessionCheckBox = new QCheckBox(PreferencesDialog::tr("Open last file on startup"));
    restoreSessionCheckBox->setCheckable(true);
    restoreSessionCheckBox->setChecked(appSettings->restoreSessionEnabled());
    q->connect(
        restoreSessionCheckBox,
        &QPushButton::toggled,
        appSettings,
        &AppSettings::setRestoreSessionEnabled
    );
    sessionGroupLayout->addRow(restoreSessionCheckBox);

    return tab;
}

QWidget *PreferencesDialogPrivate::initializeEditorTab()
{
    Q_Q(PreferencesDialog);

    QWidget *tab = new QWidget();

    QVBoxLayout *tabLayout = new QVBoxLayout();
    tab->setLayout(tabLayout);

    QGroupBox *tabsGroupBox = new QGroupBox(PreferencesDialog::tr("Tabulation"));
    tabLayout->addWidget(tabsGroupBox);

    QFormLayout *tabsGroupLayout = new QFormLayout();
    tabsGroupBox->setLayout(tabsGroupLayout);

    QCheckBox *spacesCheckBox = new QCheckBox(PreferencesDialog::tr("Insert spaces for tabs"));
    spacesCheckBox->setCheckable(true);
    spacesCheckBox->setChecked(appSettings->insertSpacesForTabsEnabled());
    connect(spacesCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setInsertSpacesForTabsEnabled(bool)));
    tabsGroupLayout->addRow(spacesCheckBox);

    QSpinBox *tabWidthInput = new QSpinBox();

    tabWidthInput->setRange(
        appSettings->MIN_TAB_WIDTH,
        appSettings->MAX_TAB_WIDTH
    );

    tabWidthInput->setValue(appSettings->tabWidth());
    connect(tabWidthInput, SIGNAL(valueChanged(int)), appSettings, SLOT(setTabWidth(int)));
    tabsGroupLayout->addRow(PreferencesDialog::tr("Tabulation width"), tabWidthInput);

    QGroupBox *stylingGroupBox = new QGroupBox(PreferencesDialog::tr("Styling"));
    tabLayout->addWidget(stylingGroupBox);

    QFormLayout *stylingGroupLayout = new QFormLayout();
    stylingGroupBox->setLayout(stylingGroupLayout);

    QComboBox *focusModeCombo = new QComboBox();
    focusModeCombo->addItem(PreferencesDialog::tr("Sentence"), FocusModeSentence);
    focusModeCombo->addItem(PreferencesDialog::tr("Current Line"), FocusModeCurrentLine);
    focusModeCombo->addItem(PreferencesDialog::tr("Three Lines"), FocusModeThreeLines);
    focusModeCombo->addItem(PreferencesDialog::tr("Paragraph"), FocusModeParagraph);
    focusModeCombo->addItem(PreferencesDialog::tr("Typewriter"), FocusModeTypewriter);
    focusModeCombo->setCurrentIndex(appSettings->focusMode() - 1);

    q->connect(
        focusModeCombo,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this, focusModeCombo](int index) {
            appSettings->setFocusMode((FocusMode) focusModeCombo->itemData(index).toInt());
        }
    );

    stylingGroupLayout->addRow(PreferencesDialog::tr("Focus mode"), focusModeCombo);

    QComboBox *editorWidthCombo = new QComboBox();
    editorWidthCombo->addItem(PreferencesDialog::tr("Narrow"), EditorWidthNarrow);
    editorWidthCombo->addItem(PreferencesDialog::tr("Medium"), EditorWidthMedium);
    editorWidthCombo->addItem(PreferencesDialog::tr("Wide"), EditorWidthWide);
    editorWidthCombo->addItem(PreferencesDialog::tr("Full"), EditorWidthFull);
    editorWidthCombo->setCurrentIndex(appSettings->editorWidth());

    q->connect(
        editorWidthCombo,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this, editorWidthCombo](int index) {
            appSettings->setEditorWidth((EditorWidth) editorWidthCombo->itemData(index).toInt());
        }
    );

    stylingGroupLayout->addRow(PreferencesDialog::tr("Editor width"), editorWidthCombo);

    QComboBox *blockquoteStyleCombo = new QComboBox();
    blockquoteStyleCombo->addItem(PreferencesDialog::tr("Plain"), false);
    blockquoteStyleCombo->addItem(PreferencesDialog::tr("Italic"), true);
    blockquoteStyleCombo->setCurrentIndex(appSettings->italicizeBlockquotes() ? 1 : 0);

    q->connect(
        blockquoteStyleCombo,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this, blockquoteStyleCombo](int index) {
            appSettings->setItalicizeBlockquotes(blockquoteStyleCombo->itemData(index).toBool());
        }
    );

    stylingGroupLayout->addRow(PreferencesDialog::tr("Blockquote style"), blockquoteStyleCombo);

    QComboBox *underlineCombo = new QComboBox();
    underlineCombo->addItem(PreferencesDialog::tr("Italic"), false);
    underlineCombo->addItem(PreferencesDialog::tr("Underline"), true);
    underlineCombo->setCurrentIndex(appSettings->useUnderlineForEmphasis() ? 1 : 0);

    q->connect(
        underlineCombo,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this, underlineCombo](int index) {
            appSettings->setUseUnderlineForEmphasis(underlineCombo->itemData(index).toBool());
        }
    );

    stylingGroupLayout->addRow(PreferencesDialog::tr("Emphasis style"), underlineCombo);

    QCheckBox *largeHeadingsCheckBox = new QCheckBox(PreferencesDialog::tr("Use large headings"));
    largeHeadingsCheckBox->setCheckable(true);
    largeHeadingsCheckBox->setChecked(appSettings->largeHeadingSizesEnabled());
    connect(largeHeadingsCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setLargeHeadingSizesEnabled(bool)));
    stylingGroupLayout->addRow(largeHeadingsCheckBox);

    QGroupBox *typingGroupBox = new QGroupBox(PreferencesDialog::tr("Typing"));
    tabLayout->addWidget(typingGroupBox);

    QFormLayout *typingGroupLayout = new QFormLayout();
    typingGroupBox->setLayout(typingGroupLayout);

    QCheckBox *cycleBulletPointsCheckBox = new QCheckBox(PreferencesDialog::tr("Cycle bullet point markers"));
    cycleBulletPointsCheckBox->setCheckable(true);
    cycleBulletPointsCheckBox->setChecked(appSettings->bulletPointCyclingEnabled());
    connect(cycleBulletPointsCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setBulletPointCyclingEnabled(bool)));
    typingGroupLayout->addRow(cycleBulletPointsCheckBox);

    QCheckBox *autoMatchCheckBox = new QCheckBox(PreferencesDialog::tr("Automatically match characters"));
    autoMatchCheckBox->setCheckable(true);
    autoMatchCheckBox->setChecked(appSettings->autoMatchEnabled());
    connect(autoMatchCheckBox, SIGNAL(toggled(bool)), appSettings, SLOT(setAutoMatchEnabled(bool)));
    typingGroupLayout->addRow(autoMatchCheckBox);

    QPushButton *matchedCharsButton = new QPushButton(PreferencesDialog::tr("Customize matched characters..."));

    q->connect(
        matchedCharsButton,
        &QPushButton::pressed,
        [this]() {
            this->showAutoMatchFilterDialog();
        }
    );

    typingGroupLayout->addRow(matchedCharsButton);

    return tab;
}

QWidget *PreferencesDialogPrivate::initializeSpellCheckTab()
{
    Q_Q(PreferencesDialog);

    Sonnet::ConfigWidget *tab = new Sonnet::ConfigWidget(q);

    q->connect(
        tab,
        &Sonnet::ConfigWidget::configChanged,
        tab,
        &Sonnet::ConfigWidget::save
    );

    q->connect(
        tab,
        &Sonnet::ConfigWidget::configChanged,
        appSettings,
        &AppSettings::spellCheckSettingsChanged
    );

    return (QWidget *) tab;
}
} // namespace ghostwriter
