/*
 * SPDX-FileCopyrightText: 2016-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFileDialog>
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
#include "../messageboxhelper.h"
#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

namespace ghostwriter
{

class PreferencesDialogPrivate : public QObject
{
    Q_OBJECT
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
    void initializeGeneralTab();
    void initializeEditorTab();
    QWidget *initializeSpellCheckTab();
};

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new PreferencesDialogPrivate(this))
    , m_ui(new Ui_PreferencesDialog)
{
    Q_D(PreferencesDialog);

    m_ui->setupUi(this);

    d->appSettings = AppSettings::instance();

    d->initializeGeneralTab();
    d->initializeEditorTab();
    m_ui->tabWidget->addTab(d->initializeSpellCheckTab(), tr("Spell Check"));

    connect(m_ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &PreferencesDialog::close);
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

void PreferencesDialogPrivate::initializeGeneralTab()
{
    Q_Q(PreferencesDialog);

    auto ui = q->m_ui;

    ui->clockCheckBox->setChecked(appSettings->displayTimeInFullScreenEnabled());
    connect(ui->clockCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setDisplayTimeInFullScreenEnabled);

    ui->menuBarCheckBox->setChecked(appSettings->hideMenuBarInFullScreenEnabled());
    connect(ui->menuBarCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setHideMenuBarInFullScreenEnabled);

    ui->unbreakableSpaceCheckBox->setChecked(appSettings->showUnbreakableSpaceEnabled());
    connect(ui->unbreakableSpaceCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setShowUnbreakableSpaceEnabled);

    ui->cornersComboBox->addItem(PreferencesDialog::tr("Rounded"), QVariant(InterfaceStyleRounded));
    ui->cornersComboBox->addItem(PreferencesDialog::tr("Square"), QVariant(InterfaceStyleSquare));
    ui->cornersComboBox->setCurrentIndex(static_cast<int>(appSettings->interfaceStyle()));

    q->connect(ui->cornersComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, ui](int index) {
        appSettings->setInterfaceStyle(static_cast<InterfaceStyle>(ui->cornersComboBox->itemData(index).toInt()));
    });

    ui->autoSaveCheckBox->setChecked(appSettings->autoSaveEnabled());
    connect(ui->autoSaveCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setAutoSaveEnabled);

    ui->backupCheckBox->setChecked(appSettings->backupFileEnabled());
    connect(ui->backupCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setBackupFileEnabled);

    q->connect(ui->openDraftDirButton, &QPushButton::clicked, [this]() {
        QDesktopServices::openUrl(QUrl(appSettings->draftLocation()));
    });

    ui->currentBackupFolderText->setText(appSettings->backupLocation());

    q->connect(ui->selectBackupFolderButton, &QPushButton::clicked, [this, ui]() {
        QString originalDir = appSettings->backupLocation();
        QFileDialog backFileDialog(q_ptr);
        backFileDialog.setFilter(QDir::Hidden | QDir::AllEntries);

        QString dir =
            backFileDialog.getExistingDirectory(NULL, tr("Select Backup Directory"), originalDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        ui->currentBackupFolderText->setText(dir);
        appSettings->setBackupLocation(dir + QDir::separator());
    });

    ui->rememberHistoryCheckBox->setChecked(appSettings->fileHistoryEnabled());
    connect(ui->rememberHistoryCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setFileHistoryEnabled);

    ui->restoreSessionCheckBox->setChecked(appSettings->restoreSessionEnabled());
    q->connect(ui->restoreSessionCheckBox, &QPushButton::toggled, appSettings, &AppSettings::setRestoreSessionEnabled);
}

void PreferencesDialogPrivate::initializeEditorTab()
{
    Q_Q(PreferencesDialog);

    auto ui = q->m_ui;

    ui->spacesCheckBox->setChecked(appSettings->insertSpacesForTabsEnabled());
    connect(ui->spacesCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setInsertSpacesForTabsEnabled);

    ui->tabWidthInput->setRange(appSettings->MIN_TAB_WIDTH, appSettings->MAX_TAB_WIDTH);

    ui->tabWidthInput->setValue(appSettings->tabWidth());
    connect(ui->tabWidthInput, QOverload<int>::of(&QSpinBox::valueChanged), appSettings, &AppSettings::setTabWidth);

    /// TODO move to UI file
    ui->focusModeCombo->addItem(PreferencesDialog::tr("Sentence"), FocusModeSentence);
    ui->focusModeCombo->addItem(PreferencesDialog::tr("Current Line"), FocusModeCurrentLine);
    ui->focusModeCombo->addItem(PreferencesDialog::tr("Three Lines"), FocusModeThreeLines);
    ui->focusModeCombo->addItem(PreferencesDialog::tr("Paragraph"), FocusModeParagraph);
    ui->focusModeCombo->addItem(PreferencesDialog::tr("Typewriter"), FocusModeTypewriter);
    ui->focusModeCombo->setCurrentIndex(appSettings->focusMode() - 1);

    q->connect(ui->focusModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, ui](int index) {
        appSettings->setFocusMode(static_cast<FocusMode>(ui->focusModeCombo->itemData(index).toInt()));
    });

    /// TODO move to UI file
    ui->editorWidthCombo->addItem(PreferencesDialog::tr("Narrow"), EditorWidthNarrow);
    ui->editorWidthCombo->addItem(PreferencesDialog::tr("Medium"), EditorWidthMedium);
    ui->editorWidthCombo->addItem(PreferencesDialog::tr("Wide"), EditorWidthWide);
    ui->editorWidthCombo->addItem(PreferencesDialog::tr("Full"), EditorWidthFull);
    ui->editorWidthCombo->setCurrentIndex(appSettings->editorWidth());

    q->connect(ui->editorWidthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, ui](int index) {
        appSettings->setEditorWidth(static_cast<EditorWidth>(ui->editorWidthCombo->itemData(index).toInt()));
    });

    ui->blockquoteStyleCombo->addItem(PreferencesDialog::tr("Plain"), false);
    ui->blockquoteStyleCombo->addItem(PreferencesDialog::tr("Italic"), true);
    ui->blockquoteStyleCombo->setCurrentIndex(appSettings->italicizeBlockquotes() ? 1 : 0);

    q->connect(ui->blockquoteStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, ui](int index) {
        appSettings->setItalicizeBlockquotes(ui->blockquoteStyleCombo->itemData(index).toBool());
    });

    ui->underlineCombo->addItem(PreferencesDialog::tr("Italic"), false);
    ui->underlineCombo->addItem(PreferencesDialog::tr("Underline"), true);
    ui->underlineCombo->setCurrentIndex(appSettings->useUnderlineForEmphasis() ? 1 : 0);

    q->connect(ui->underlineCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, ui](int index) {
        appSettings->setUseUnderlineForEmphasis(ui->underlineCombo->itemData(index).toBool());
    });

    ui->largeHeadingsCheckBox->setChecked(appSettings->largeHeadingSizesEnabled());
    connect(ui->largeHeadingsCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setLargeHeadingSizesEnabled);

    ui->cycleBulletPointsCheckBox->setChecked(appSettings->bulletPointCyclingEnabled());
    connect(ui->cycleBulletPointsCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setBulletPointCyclingEnabled);

    ui->autoMatchCheckBox->setChecked(appSettings->autoMatchEnabled());
    connect(ui->autoMatchCheckBox, &QCheckBox::toggled, appSettings, &AppSettings::setAutoMatchEnabled);

    q->connect(ui->matchedCharsButton, &QPushButton::pressed, this, [this]() {
        this->showAutoMatchFilterDialog();
    });
}

QWidget *PreferencesDialogPrivate::initializeSpellCheckTab()
{
    Q_Q(PreferencesDialog);

    Sonnet::ConfigWidget *tab = new Sonnet::ConfigWidget(q);

    q->connect(tab, &Sonnet::ConfigWidget::configChanged, tab, &Sonnet::ConfigWidget::save);

    q->connect(tab, &Sonnet::ConfigWidget::configChanged, appSettings, &AppSettings::spellCheckSettingsChanged);

    return (QWidget *)tab;
}
} // namespace ghostwriter

#include "preferencesdialog.moc"
