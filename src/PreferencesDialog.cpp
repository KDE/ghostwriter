#include <QTabWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QHash>

#include "PreferencesDialog.h"

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    this->setWindowTitle(tr("Preferences"));

    QTabWidget* tabWidget = new QTabWidget(this);

    QVBoxLayout* dialogLayout = new QVBoxLayout();
    dialogLayout->addWidget(tabWidget);
    this->setLayout(dialogLayout);

    // General tab
    QWidget* tab = new QWidget();
    tabWidget->addTab(tab, tr("General"));
    tabWidget->addTab(tab, tr("Editor"));
    tabWidget->addTab(tab, tr("HUD"));
    tabWidget->addTab(tab, tr("CSS"));
    tabWidget->addTab(tab, tr("Export"));
    tabWidget->addTab(tab, tr("Themes"));
}

PreferencesDialog::~PreferencesDialog()
{

}
