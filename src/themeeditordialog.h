/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef THEMEEDITORDIALOG_H
#define THEMEEDITORDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "theme.h"

namespace ghostwriter
{
/**
 * Theme editor dialog which allows the user modify a theme for the
 * application's look and feel.
 */
class ThemeEditorDialogPrivate;
class ThemeEditorDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ThemeEditorDialog)

public:
    /**
     * Constructor.  Takes the theme to to be edited as a parameter.
     */
    ThemeEditorDialog(const Theme &theme, QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~ThemeEditorDialog();

    /**
     * Returns the theme selected if action was to accept.
     */
    const Theme &theme() const;

private slots:
    void accept();
    void reject();

private:
    QScopedPointer<ThemeEditorDialogPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // THEMEEDITORDIALOG_H
