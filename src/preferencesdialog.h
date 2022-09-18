/*
 * SPDX-FileCopyrightText: 2016-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

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
};
} // namespace ghostwriter

#endif // PREFERENCESDIALOG_H
