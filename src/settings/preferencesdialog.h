/*
 * SPDX-FileCopyrightText: 2016-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

class Ui_PreferencesDialog;

namespace ghostwriter
{
/**
 * Displays application preferences.
 */
class PreferencesDialogPrivate;
class PreferencesDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PreferencesDialog)

public:
    /**
     * Constructor.
     */
    PreferencesDialog(QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~PreferencesDialog();

private:
    QScopedPointer<PreferencesDialogPrivate> d_ptr;
    Ui_PreferencesDialog *m_ui;
};
} // namespace ghostwriter

#endif // PREFERENCESDIALOG_H
